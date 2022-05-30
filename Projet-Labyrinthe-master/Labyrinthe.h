#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"
#include <queue>

class Labyrinthe : public Environnement
{
	private:
		char *tab;
		char **_data;

		// Matrice de int pour algo innondation.
		int *tabIn;
		int **_innond;

		int widthLaby;
		int heightLaby;

		int distMax;
		int seuilDefense;

		void innondationDFS(int, int, int);
		int innondationBFS();

	public:
		Labyrinthe(char *);
		int width() { return heightLaby; }	 // retourne la largeur du labyrinthe.
		int height() { return widthLaby; }   // retourne la longueur du labyrinthe.
		char data(int i, int j);
		int innond(int i, int j);

		bool set_data(int i, int j, int b);

		//provisoire
		void display_tab();

};

#endif
