#include "Chasseur.h"
#include "Gardien.h"
#include <iostream>
#include <unistd.h>
#include "Personnage.h"

#define TP 2

/*
 *	Constructeur.
 */

Chasseur::Chasseur(Labyrinthe *l) : Personnage(l, 0, 5)
{
	// initialise les sons.
	_hunter_fire = new Sound("sons/hunter_fire.wav");
	_hunter_hit = new Sound("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound("sons/hit_wall.wav");
}


/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux(double dx, double dy)
{
	if(_life<=0){
		_life = 0;
		return false;
	}


	int posX = (int)((_x + dx) / Environnement::scale);
	int posY = (int)((_y + dy) / Environnement::scale);

	//vérifie si le Chasseur est proche du trésor
	check_treasor(dx, dy);

	if (EMPTY == _l->data(posX, posY) || (_l->data(posX, posY) >= 65 && _l->data(posX, posY) <= 90))
	{
		//empêche le Chasseur de se déplacer sur la position des Gardiens
		for(int i = 1; i < _l->_nguards; i++){
			Personnage *cible = _allPerso[i];
			//un gardien vivant nous bloque
			if(cible->getLife() > 0){
				if(check_collision_ennemi(_x+dx, _y+dy, cible->_x, cible->_y, Environnement::scale)){
					return false;
				}
			}
		}
		//on se déplace
		_x += dx;
		_y += dy;
		return true;
	}

	//si on est devant un téléporteur
	else if(_l->data(posX, posY) >= 97 && _l->data(posX, posY) <= 122)
	{
		teleportation(_l->data(posX, posY) - 32);	
	}

	return false;
}


/*
 *	Téléporte le chasseur
 */

void Chasseur::teleportation(int newPos){
	//int a;
	for(int i = 0; i < _l->width(); i++){
		for(int j = 0; j < _l->height(); j++){
			if(_l->data(i, j) == newPos){
				_x = i * Environnement::scale;
				_y = j * Environnement::scale;
			}
		}
	}
}


/*
 *	Vérifie si le Chasseur ne se trouve pas à côté du trésor
 */

void Chasseur::check_treasor(double dx, double dy){
	int treasorX = _l->_treasor._x;
	int treasorY = _l->_treasor._y;

	if(	(	(_x + dx)/Environnement::scale >= treasorX && (_x + dx)/Environnement::scale <  treasorX + 1) &&
		(	(_y + dy)/Environnement::scale >= treasorY && (_y + dy)/Environnement::scale <  treasorY + 1)){
			message("VICTOIRE, le trésor est à vous");
		}
	return;
}


/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball(float dx, float dy)
{
	float a = _fb->get_x();
	float b = _fb->get_y();

	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float x = (_x - _fb->get_x()) / Environnement::scale;
	float y = (_y - _fb->get_y()) / Environnement::scale;
	float dist2 = x * x + y * y;
	// on bouge que dans le vide!

	//
	for (int i = 1; i < (int)_allPerso.size(); i++)
	{
		Personnage *cible = _allPerso[i];

		if ((a >= cible->_x && a < cible->_x + Environnement::scale) &&
			(b >= cible->_y && b < cible->_y + Environnement::scale))
		{

			if(cible->getLife() > 0){
				cible->setLife(cible->getLife() - 1);
			}

			//l'ennemi est KO, il ne se relèvera pas
			if(cible->getLife() == 0){
				cible->rester_au_sol();

				//utilisé pour vérifier s'il y a encore des gardiens en vie
				bool fin = true;
				for (int j = 1; j < (int)_allPerso.size(); j++)
				{
					std::cout << "life : " << _allPerso[j]->_life << std::endl;
					if(_allPerso[j]->_life > 0){
						fin = false;
						break;
					}
				}
				if(fin){
					message("Victoire, les gardiens ne sont plus en état de se battre");
				}
				std::cout << std::endl;
			}

			//l'ennemi est touché, mais il peut encore se relever
			else{
				cible->tomber();
			}
			return false;
		}
	}
	int fireBallX = (int)((_fb->get_x() + dx) / Environnement::scale);
	int fireBallY = (int)((_fb->get_y() + dy) / Environnement::scale);

	if(EMPTY == _l->data(fireBallX, fireBallY)){
		// il y a la place.
		return true;
	}

	//on touche un mur cassable
	else if(_l->data(fireBallX, fireBallY) == '='){

		for (int i = 0; i < _l->_nwall; i++){
			if(	fireBallX >= _l->_walls[i]._x1 && fireBallX <= _l->_walls[i]._x2 &&
				fireBallY >= _l->_walls[i]._y1 && fireBallY <= _l->_walls[i]._y2){

					//on met à 0 les les coordonées d'un mur horizontal
					for(int k = _l->_walls[i]._x1; k < _l->_walls[i]._x2; k++){
						_myLaby->set_data(k, fireBallY, 0);
					}
					//on met à 0 les les coordonées d'un mur vertical
					for(int k = _l->_walls[i]._y1; k < _l->_walls[i]._y2; k++){
						_myLaby->set_data(fireBallX, k, 0);
					}
					//on supprime le mur touché
					for(int j = i; j < _l->_nwall - 1; j++){
						_l->_walls[j] = _l->_walls[j+1];
					}
					//on décrement wall
					_l->_nwall --;
					break;
			}
		}
		//regénère la visualisation
		_l->reconfigure();
	}

	// collision...
	// calculer la distance maximum en ligne droite.
	float dmax2 = (_l->width()) * (_l->width()) + (_l->height()) * (_l->height());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit->play(1. - dist2 / dmax2);
	return false;
}


/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire(int angle_vertical)
{

	if(_life > 0){

		_fb->init(/* position initiale de la boule */ _x, _y, 10.,
			  /* angles de vis�e */ angle_vertical, _angle);
	}
	
}


/*
 *	Vérifie si l'objet fictif de coordonnées X, y se trouve aux alentours de cx, cy
 */

bool Chasseur::check_collision_ennemi(float x, float y, float cx, float cy, float ecart){
	return (	(x >= cx && x < cx + ecart) &&
				(y >= cy && y < cy + ecart) );
	
}

