#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include "Sound.h"
#include "Chasseur.h"

class Labyrinthe;

class Gardien : public Personnage
{
	private : 
		bool _aggressif = false;
		bool _alerte = false;
		float _direction;
		int distanceVision;
		float potentielDefense;
		//int _life = 3;
		//_shot = true uniquement si la fireball du gardien n'a pas encore explosé
		bool _shot = false;

	public:
		

		Sound *_hunter_fire; // bruit de l'arme du chasseur.
		Sound *_hunter_hit;	 // cri du chasseur touch�.
		Sound *_wall_hit;	 // on a tap� un mur.

		Gardien(Labyrinthe *l, const char *modele);
		// : Mover(120, 80, l, modele) {}

		// mon gardien pense tr�s mal!
		void update(void);
		// et ne bouge pas!
		bool move(double dx, double dy);
		// ne sait pas tirer sur un ennemi.
		void fire(int angle_vertical);
		// quand a faire bouger la boule de feu...
		bool process_fireball(float dx, float dy);

		int aim(float, float);

		bool attaque();

		bool vision();

		bool touche_cible(float x, float y, float cx, float cy);

		bool case_convert(float x, float y, int a);

		void ajusteAngle();

		bool ciblage(int angleCible);

		void setLife(int l);

		int getLife();

		// inverse _shot
		void switchShot();

		void action();

		bool check_collision_ennemi(float x, float y, float cx, float cy, float ecart);

};

#endif
