#include "Gardien.h"
#include <iostream>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "Sound.h"

Gardien::Gardien(Labyrinthe *l, const char *modele) : Personnage(l, modele, 3)
{
	// initialise les sons.
	_hunter_fire = new Sound("sons/hunter_fire.wav");
	_hunter_hit = new Sound("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound("sons/hit_wall.wav");

	//initialise l'aléatoire
	srand(time(NULL));


	//met l'angle de départ du gardien à 0
	_angle = 0;
	_direction = _angle;

	//initialise la distance de vue du gardien
	distanceVision = 120;

	_defenseur = false;
}


void Gardien::update(){
	//si le gardien n'a plus de vie, ses mouvements ne sont plus actualisés
	if(_life <= 0){
		_life = 0;
		return;
	}

	//détermine le mode du gardien (passif, alerte, aggressif)
	vision();

	//détermine l'action effectuée
	action();

	return;
}

void Gardien::action(){
	//conversion degré à radiant
	float piAngle = (_direction * M_PI / 180);

	//Mode alerte, le gardien se tourne progressivement vers le côté le plus rapide pour faire face à la cible
	if(_alerte){

		//récupère l'angle en direction du chasseur
		int angleCible = aim(_l->_guards[0]->_x, _l->_guards[0]->_y);

		if(_angle >= angleCible){

			if(_angle - angleCible <= 180){
				_direction-=3;
				ajusteAngle();
				ciblage(angleCible);
			}
			else{
				_direction+=3;
				ajusteAngle();
				ciblage(angleCible);
			}
		}
		else{

			if(angleCible - _angle <= 180){
				_direction+=3;
				ajusteAngle();
				ciblage(angleCible);
			}
			else{
				_direction-=3;
				ajusteAngle();
				ciblage(angleCible);
			}
		}
	}

	//Mode aggressif, le gardien poursuit et aimlock le chasseur en tirant
	if(_aggressif){

		// l'angle est locké sur le chasseur
		_angle = aim(_l->_guards[0]->_x, _l->_guards[0]->_y);
		float piAngle = (_angle * M_PI / 180);

		//Le gardien se dirige vers le Chasseur
		_direction = _angle;
		move(-sin(piAngle), cos(piAngle));

		//Le gardien tire, sa précision varie en fonction de sa vie
		attaque();
	}

	//Mode passif, le gardien bouge selon sa nature (défenseur ou non)
	else{
		_angle = _direction;
		move(sin(-piAngle), cos(piAngle));
	}

	return;
}


/*
 *	Réajuste l'angle pour qu'il soit toujours dans l'interval [0, 360]
 */
void Gardien::ajusteAngle(){
	if( _direction >= 360){
		_direction -= 360;
	}
	else if(_direction < 0){
		_direction += 360;
	}
}


/*
 *	Permet au gardien de passer en aggressif si le Chasseur se trouve devant le Gardien
 *	Le Chasseur sera ciblé s'il est dans l'angle de vision du gardien à +/- 5 degrés
 */
bool Gardien::ciblage(int angleCible){

	if(	((_angle + 365) % 360 >= angleCible && _angle <= angleCible) ||
		((_angle + 355) % 360 <= angleCible && _angle >= angleCible) )
	{
		_alerte = false;
		_aggressif = true;
	}
	return true;
}


/*
 *	Vérifie si un objet fictif de coordonnées x, y est proche d'un autre objet de coordonnées cx, cy
 */

bool Gardien::touche_cible(float x, float y, float cx, float cy){
	return ((x < cx + Environnement::scale) && (x >= cx - Environnement::scale) &&
			(y < cy + Environnement::scale) && (y >= cy - Environnement::scale));
}

/*
 *	Adapte les coordonnées flottantes x, y au graphe _data et vérifie que la case est égale à b
 */

bool Gardien::case_convert(float x, float y, int a){
	return ( a == (_l -> data(	(int)(x / Environnement::scale),
								(int)(y / Environnement::scale))) );
}


bool Gardien::vision(){

	Mover* cible = _l->_guards[0];

	int cibleAngle = aim(cible->_x, cible->_y);
	float cibleAngleRadiant = (cibleAngle * M_PI / 180);	

	//si la cible n'est pas dans le cercle de portée de vision du gardien, le gardien reste passif (les blocs et murs ne sont pour l'instant pas pris en compte)
	if(	!((_x - distanceVision <= cible->_x && cible->_x <= _x + distanceVision) &&
		(_y - distanceVision <= cible->_y && cible->_y <= _y + distanceVision)) )
	{
		_alerte = false;
		_aggressif = false;
		return false;
	}

	//sinon on vérifie s'il n'y a pas un obstacle entre la cible et le gardien
	else{

		float xfict = _x;
		float yfict = _y; 
	
		//on simule une fireball virtuelle pour voir si la vraie ne sera pas arreté par un obstacle
		while ( !touche_cible(xfict, yfict, cible->_x, cible->_y) )
		{
			//s'il y a un obstacle entre les deux le gardien redevient passif
			// Note : il se dirige vers le dernier emplacement du Chasseur connu
			if( case_convert(xfict, yfict, 1) ){
				_alerte = false;
				_aggressif = false;
				return false;
			}

			//on fait avancer notre objet virtuel dans la direction de la cible
			xfict += sin(-cibleAngleRadiant);
			yfict += cos(cibleAngleRadiant);
		}
	}
	_alerte = true;
	return true;
}


/*
 *	Le gardien tire sur le Chasseur. La précision varie en fonction de sa vie (TODO)
 */

bool Gardien::attaque(){
	int fireRate = rand()%20;
	//int erreurVisee = 20;

	//si la fireball n'est pas déjà tirée
	if(!_shot){
		if(fireRate == 1){
			int coeff = 3 - _life;

			//le gardien a toute sa vie, il tire droit
			if(coeff == 0){
				fire(180);
				switchShot();
			}

			//le gardien est blessé, ses tirs manquent de précision
			else{
				int borne = coeff * 10;
				int devisage =  rand() % (borne +1);
				devisage -= (borne/2);
				_angle += devisage;
				ajusteAngle();
				fire(180);
				switchShot();
			}
		}
	}
	return true;
}

//permet de viser les coordonnées x, y
int Gardien::aim(float x, float y){
	//on récupère l'angle
	float angle = atan((_x - x) / (_y - y));
	//on réajuste
	if (y > _y){
		angle += M_PI;
		if(x > _x){
			angle -= 2 * M_PI;
		}
	}
	return (180 - (angle * 180/M_PI));
}


bool Gardien::move(double dx, double dy)
{
	// On réduit la vitesse.
	dx /= 4;
	dy /= 4;

	// On prend le centre du personnage.
	float x = _x + Environnement::scale / 2;
	float y = _y + Environnement::scale / 2;

	// On regarde si la case est vide.
	if (EMPTY == _l->data(	(int)((x + dx) / Environnement::scale),
							(int)((y + dy) / Environnement::scale)))
	{
		// On ajoute le déplacement.
		x += dx;
		y += dy;

		Personnage *cible = _allPerso[0];
		if(check_collision_ennemi(x - Environnement::scale / 2, y - Environnement::scale / 2, cible->_x, cible->_y, Environnement::scale)){
			cible->_life = 0;
			message("Vous êtes mort");
			return false;
		}

		// On déplace le personnage.
		_x = x - Environnement::scale / 2;
		_y = y - Environnement::scale / 2;

		return true;
	}

	// Sinon, c'est qu'il y a collision et donc on oriente le personnage autre part.
	else
	{
		// Si le Gardien est un défenseur, il va vers le trésor une fois sur deux.
		if(_defenseur && rand() % 2 == 1)
		{

			// La case du gardien.
			int cX = (int)(x / Environnement::scale);
			int cY = (int)(y / Environnement::scale);

			// La où le gardien devra aller.
			int versX = 0;
			int versY = 0;

			// On cherche la plus petite case autour du gardien (qui conduira vers le trésor).
			int plusPetit = INT_MAX;

			// Pour toutes les cases autour.
			for(int sX = -1; sX <= 1; sX++)
			{
				for (int sY = -1; sY <= 1; sY++)
				{
					// On ne veut pas étudier la case du gardien, donc lorsqu'on est dessus, on saute.
					if (sX == 0 && sY == 0) continue;

					// Si on a trouvé une case menant plus rapidement vers le trésor, on la choisi.
					if (getInnond(cX + sX, cY + sY) < plusPetit && getInnond(cX + sX, cY + sY) != -2)
					{
						versX = cX + sX;
						versY = cY + sY;
						plusPetit = getInnond(cX + sX, cY + sY);
					}
				}
			}
			

			// On oriente vers la case menant vers le trésor.

			float xf = ((versX) * Environnement::scale);
			float yf = ((versY) * Environnement::scale);

			_direction = aim(xf, yf);
		}
		else
		{
			_direction = rand() % 360 + 1;
		}
	}
	return false;
}

//tire une boule de feu
void Gardien::fire(int angle_vertical){
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, 360 - _angle);
	return;
}


//gère le déplacement de la boule de feu
bool Gardien::process_fireball(float dx, float dy){
	Personnage* cible = _allPerso[0];

	//récupère les coordonnées de la fireball
	float a = _fb->get_x();
	float b = _fb->get_y();

	//si on touche une cible (ici le chasseur)
	if(touche_cible(a, b, cible->_x, cible->_y)){
		
		//s'il est encore vivant
		if(cible->_life > 0){
			//on lui retire des pv
			cible->_life --;
			message("PV : %d", cible->_life);
		}
		//sinon vous êtes mort
		else{
			message("vous êtes mort");
		}

		//on repermet au gardien de tirer
		switchShot();
		return false;
	}

	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		// il y a la place.
		return true;
	}

	//on inverse
	switchShot();
	return false;
}


//permet d'inverser la variable shot
void Gardien::switchShot(){
	(_shot == false) ? (_shot = true) : (_shot = false);
	return;
}

//vérifie s'il n'y a pas collision avec un ennemi
bool Gardien::check_collision_ennemi(float x, float y, float cx, float cy, float ecart){
	return (	(x >= cx && x < cx + ecart) &&
				(y >= cy && y < cy + ecart) );
	
}