#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"
#include "Personnage.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <queue>

Sound *Chasseur::_hunter_fire; // bruit de l'arme du chasseur.
Sound *Chasseur::_hunter_hit;  // cri du chasseur touché.
Sound *Chasseur::_wall_hit;	   // on a tapé un mur.


Environnement *Environnement::init(char *filename)
{
	return new Labyrinthe(filename);
}

Labyrinthe::Labyrinthe(char *filename)
{
	// std::cout << filename << std::endl;
	std::ifstream fichier;
	fichier.open(filename);

	// Si fichier introuvable.
	if(!fichier.is_open())
	{
		std::cerr << "Ouverture de fichier impossible, sortie." << std::endl;
		return;
	}

	std::string ligne;
	std::smatch matchh;

	// std::regex laby("[\+][\+\-\| \na-zA-Z]+");

	std::regex commentaire("#.*");
	std::regex texture("[a-z][ 	]+[A-z1-9]+.jpg");
	std::regex fichier_texture("[A-z1-9]+.jpg");
	std::regex debutLaby("[ ]*[+]+[-a-z ]*");

	std::map<char, std::string> images;
	std::vector<std::vector<char>> laby;
	std::vector<char> tmp;
	char tmp2;


	// Récupération des images.
	// Tant qu'on est pas sur le laby.
	while (getline(fichier, ligne) && !std::regex_search(ligne, debutLaby))
	{
		// Si c'est une texture.
		if (std::regex_search(ligne, matchh, texture))
		{
			// std::cout << "texture" << matchh.str();
			// On prend son id.
			tmp2 = matchh.str()[0];

			// On prend juste le nom du fichier de la texture.
			std::regex_search(ligne, matchh, fichier_texture);

			// On met dans le dico.
			images[tmp2] = matchh.str();
		}
		// std::cout << std::endl;
	}

	// std::cout << "a" << images['a'] << " b" << images['b'] << std::endl;


	// Récupération du laby.
	// On récupère toutes les lignes.
	widthLaby = 0;
	do
	{
		tmp.clear();
		for (int i = 0; i < (int)ligne.length(); i++)
		{
			tmp.push_back(ligne[i]);
		}
		laby.push_back(tmp);

		if((int)ligne.length() > widthLaby) widthLaby = ligne.length();

	} while (getline(fichier, ligne));

	heightLaby = laby.size();
	// std::cout << "w " << widthLaby << " " << laby[0].size() << " h" << heightLaby << " " << laby.size() << std::endl;

	// Création du tableau _data.
	tab = new char[widthLaby * heightLaby];
	_data = new char *[heightLaby];

	// Innondation.
	tabIn = new int[widthLaby * heightLaby];
	_innond = new int *[heightLaby];

	// On met des cases vide.
	for(int i = 0; i < widthLaby * heightLaby; i++)
	{
		tab[i] = EMPTY;
		tabIn[i] = -1;
	}

	for (int i = 0; i < heightLaby; i++)
	{
		_data[i] = tab + (i * widthLaby);
		_innond[i] = tabIn + (i * widthLaby);
	}


	// // Print laby
	// for(int i = 0; i < laby.size(); i++)
	// {
	// 	for(int j = 0; j < laby[i].size(); j++)
	// 	{
	// 		std::cout << laby[i][j];
	// 	}
	// 	std::cout << std::endl;
	// }

	std::regex affiche("[a-z]");
	std::regex non_mur("[A-Z =]");
	std::regex mur("[-|]");
	std::regex tp("[A-BD-FH-SU-WY-Z]");

	int debut;
	std::vector<Wall> murs;

	std::string cts;
	std::string cts_droite;
	std::string cts_gauche;
	std::string cts_bas;

	std::vector<Wall> affiches;
	Wall a;

	std::vector<Box> caisses;

	std::vector<Mover*> joueurs;

	//damien
	std::vector<Personnage*> persos;

	// Permet de savoir où un mur vertical a commencé.
	std::map<int, int> debutsVerticales;

	// Analyse laby.
	for (int i = 0; i < (int)laby.size(); i++)
	{
		debut = -1;

		for (int j = 0; j < (int)laby[i].size(); j++)
		{
			// Pour regex.
			cts = std::string(1, laby[i][j]);

			cts_droite = (j + 1 < (int)laby[i].size() ? std::string(1, laby[i][j + 1]) : " ");
			cts_gauche = (j - 1 >= 0 ? std::string(1, laby[i][j - 1]) : " ");

			cts_bas = (i + 1 <(int) laby.size() ? std::string(1, laby[i + 1][j]) : " ");



			// Début d'un mur H ou mur H destructible.
			// debut == -1 car il peut y avoir des '+' sur un mur déjà commencé (+---+---+).
			// Après, on doit avoir un mur (non non_mur) ou un mur destructible "=".
			if (laby[i][j] == '+' && debut == -1 
			&& (!std::regex_match(cts_droite, non_mur) || cts_droite == "="))
			{
				debut = j;
				// std::cout << "debut H" << debut << std::endl;
			}

			// Fin du mur H (si après, on a un non_mur ou un mur V.)
			else if (laby[i][j] == '+' && debut != -1 
			&& (std::regex_match(cts_droite, non_mur) || cts_droite == "|") 
			&& cts_gauche != "=")
			{
				murs.push_back({i, debut, i, j, 0});
				for (int k = debut; k <= j; k++)
				{
					if(_data[i][k] == 0)
					{
						_data[i][k] = 1;
						_innond[i][k] = -2;
					}
				}
				// std::cout << "Mur H" << debut << " " << j << " " << i << std::endl;
				// Si on a un mur destructible juste après, on le commence directement.
				if (cts_droite == "=")
					debut = j;
				else
					debut = -1;
			}

			// Fin du mur H destructible (si après, on a un non_mur ou un mur normal.)
			else if (laby[i][j] == '+' && debut != -1 && cts_gauche == "=")
			{
				murs.push_back({i, debut, i, j, 0});
				for (int k = debut; k <= j; k++)
				{
					if (_data[i][k] == 0)
					{
						_data[i][k] = '=';
						_innond[i][k] = -2;
					}
				}
				// Si on a un mur normal ou un second mur destructible juste après, on le commence directement.
				if (cts_droite == "-" || cts_droite == "=")
					debut = j;
				else
					debut = -1;
			}



			// Début d'un mur V.
			// Si y'a un + au-dessus et que debut n'est pas déjà utilisé.
			if (laby[i][j] == '+' 
			&& (debutsVerticales.find(j) == debutsVerticales.end() || debutsVerticales.find(j)->second == -1) 
			&& !std::regex_match(cts_bas, non_mur))
			{
				debutsVerticales[j] = i;
				// std::cout << "debut V" << debutsVerticales[j] << std::endl;
			}

			// Fin d'un mur V.
			// Si y'a un + au-dessus et que debut est utilisé.
			else if (laby[i][j] == '+' 
			&& debutsVerticales.find(j) != debutsVerticales.end() && debutsVerticales.find(j)->second != -1
			&& (std::regex_match(cts_bas, non_mur) || cts_bas == "-") )
			{
				murs.push_back({debutsVerticales.find(j)->second, j, i, j, 0});
				for (int k = debutsVerticales.find(j)->second; k <= i; k++)
				{
					if(_data[k][j] == 0)
					{
						_data[k][j] = 1;
						_innond[k][j] = -2;
					}
				}
				// std::cout << "Mur V" << debutsVerticales.find(j)->second << " " << i << " " << j << std::endl;
				debutsVerticales[j] = -1;
			}



			// Début d'un mur V destructible.
			// Si y'a un + au-dessus et que debut n'est pas déjà utilisé.
			if (laby[i][j] == '+' 
			&& (debutsVerticales.find(j) == debutsVerticales.end() || debutsVerticales.find(j)->second == -1) 
			&& cts_bas == "=")
			{
				debutsVerticales[j] = i;
				// std::cout << "debut V" << debutsVerticales[j] << std::endl;
			}

			// Fin d'un mur V destructible.
			// Si y'a un + au-dessus et que debut est utilisé.
			else if (laby[i][j] == '+' 
			&& debutsVerticales.find(j) != debutsVerticales.end() && debutsVerticales.find(j)->second != -1 
			&& (std::regex_match(cts_bas, non_mur) || cts_bas == "-"))
			{
				murs.push_back({debutsVerticales.find(j)->second, j, i, j, 0});
				for (int k = debutsVerticales.find(j)->second; k <= i; k++)
				{
					if (_data[k][j] == 0)
					{
						_data[k][j] = 1;
						_innond[k][j] = -2;
					}
				}
				// std::cout << "Mur V" << debutsVerticales.find(j)->second << " " << i << " " << j << std::endl;
				debutsVerticales[j] = -1;
			}

			// Caisses.
			if (laby[i][j] == 'X')
			{
				caisses.push_back({i, j, 0});
				_data[i][j] = 1;
				_innond[i][j] = -2;
			}

			// Chasseur.
			else if (laby[i][j] == 'C')
			{
				// On crée le chasseur.
				Chasseur *c = new Chasseur(this);
				c->_x = (float)i * 10;
				c->_y = (float)j * 10;

				// std::cout << "Chasseur" << c->_x << " " << c->_y << std::endl;

				// On le met au début de la liste.
				joueurs.emplace(joueurs.begin(), c);

				//damien
				persos.emplace(persos.begin(), c);
				int a = persos[0]->getLife();
				std::cout << "pv : " << a << std::endl;
			}

			// Gardiens.
			else if (laby[i][j] == 'G')
			{
				// On crée le chasseur.
				Gardien *g = new Gardien(this, "Blade");
				g->_x = (float)i * 10;
				g->_y = (float)j * 10;
				// std::cout << "Gardiens" << g->_x << " " << g->_y << std::endl;

				// On le met au début de la liste.
				joueurs.push_back(g);

				//damien
				persos.push_back(g);
				_data[i][j] = 0;
			}

			// Trésor.
			else if (laby[i][j] == 'T')
			{
				_treasor._x = i;
				_treasor._y = j;
				_data[i][j] = 1;
				_innond[i][j] = -1;
				// std::cout << "Trésor" << _treasor._x << " " << _treasor._y << std::endl;
			}

			// Destination.
			else if(std::regex_match(cts, tp))
			{
				_data[i][j] = laby[i][j];
			}

			// Affiches.
			else if (std::regex_match(cts, affiche))
			{
				// Mur +--a--+.
				if (debut != -1)
				{
					a = Wall({i, j, i, j + 2, 0});

					_data[i][j] = laby[i][j];
					_data[i][j + 1] = laby[i][j];
					_data[i][j + 2] = laby[i][j];

					_innond[i][j] = -2;
					_innond[i][j+1] = -2;
					_innond[i][j+2] = -2;
				}

				// Mur +||a||+.
				else
				{
					a = Wall({i, j, i + 2, j, 0});

					_data[i][j] = laby[i][j];
					_data[i + 1][j] = laby[i][j];
					_data[i + 2][j] = laby[i][j];

					_innond[i][j] = -2;
					_innond[i+1][j] = -2;
					_innond[i+2][j] = -2;
				}

				char *tmp3 = new char[128];
				sprintf(tmp3, "%s/%s", texture_dir, images.find(laby[i][j])->second.c_str());
				a._ntex = wall_texture(tmp3);

				affiches.push_back(a);
				
			}
		
			// Mur cassable.
		}
	}

	//damien
	//std::vector<Personnage*> *p = &persos;
	for (int i = 0; i < (int)persos.size(); i++)
	{
		persos[i]->_allPerso = persos;
	}
	//Personnage::_allPerso = persos;

	_nwall = murs.size();
	_walls = new Wall[murs.size()];
	for (int i = 0; i < _nwall; i++)
	{
		_walls[i] = murs[i];
	}

	_npicts = affiches.size();
	_picts = new Wall[affiches.size()];
	for (int i = 0; i < _npicts; i++)
	{
		_picts[i] = affiches[i];
	}

	_nboxes = caisses.size();
	_boxes = new Box[caisses.size()];
	for (int i = 0; i < _nboxes; i++)
	{
		_boxes[i] = caisses[i];
	}

	_nguards = joueurs.size();
	_guards = new Mover *[joueurs.size()];
	for (int i = 0; i < _nguards; i++)
	{
		_guards[i] = joueurs[i];
	}

	murs.clear();
	murs.swap(murs);

	affiches.clear();
	affiches.swap(affiches);

	caisses.clear();
	caisses.swap(caisses);

	joueurs.clear();
	joueurs.swap(joueurs);

	// Création de la matrice d'innondation.
	// -1 = Vide / -2 = Inaccessible

	// innondationDFS(_treasor._x, _treasor._y, 0);
	distMax = innondationBFS();
	seuilDefense = distMax / 2;

	// On donne les roles de défenseur.
	int potentielDefense = 0;
	for (int i = 1; i < persos.size(); i++)
	{
		potentielDefense += distMax / _innond[(int)(persos[i]->_x + Environnement::scale / 2) / Environnement::scale][(int)(persos[i]->_y + Environnement::scale / 2) / Environnement::scale];
		if(potentielDefense <= seuilDefense) {
			persos[i]->_defenseur = true;
		}
		else break;
	}

	// for (int i = 0; i < heightLaby; i++)
	// {
	// 	for (int j = 0; j < widthLaby; j++)
	// 	{
	// 		std::cout << _innond[i][j] << " ";
	// 	}
	// 	std::cout << std::endl;
	// }
}

// Algo type DFS (parcours en longueur).
void Labyrinthe::innondationDFS(int x, int y, int distTresor)
{
	// Si sortie du tableau, on return.
	if (x < 0 || x >= heightLaby || y < 0 || y >= widthLaby || _innond[x][y] == -2) return;

	// Si la case à déjà une distance ou que cette distance 
	// est plus petit que celle étudié, on return.
	if (_innond[x][y] == -1 || _innond[x][y] > distTresor)
	{
		_innond[x][y] = distTresor;
		innondationDFS(x, y - 1, distTresor + 1);
		innondationDFS(x, y + 1, distTresor + 1);
		innondationDFS(x + 1, y, distTresor + 1);
		innondationDFS(x - 1, y, distTresor + 1);

		innondationDFS(x + 1, y + 1, distTresor + 1);
		innondationDFS(x + 1, y - 1, distTresor + 1);
		innondationDFS(x - 1, y - 1, distTresor + 1);
		innondationDFS(x - 1, y + 1, distTresor + 1);
	}
}

// Algo type BFS (parcours en largeur).
int Labyrinthe::innondationBFS()
{
	// Création de deux files.
	std::queue<int> queueX;
	std::queue<int> queueY;

	// On récupère la position du trésor comme position initiale.
	int x = _treasor._x;
	int y = _treasor._y;

	// Permet de sauvegarder les voisins de x, y.
	int voisinX, voisinY;

	// On initialise la distance au trésor à 0.
	int distTresor = 0;

	// On dit que le trésor est à distance 0 de lui-même.
	// On incrémente de 1 pour les voisins du trésor.
	_innond[x][y] = distTresor++;

	// On met la position du trésor dans la file.
	queueX.push(x);
	queueY.push(y);

	// Tant que toutes les cases n'ont pas été étudié.
	while(!queueX.empty())
	{

		// On récupère le premier élément de la file.
		x = queueX.front();
		y = queueY.front();

		// On l'enlève de la file.
		queueX.pop();
		queueY.pop();

		// Si l'élément étudié est à une distance supérieur, c'est qu'on étudie un 
		// niveau inferieur de "l'arbre" donc on incrémente de 1.
		if(_innond[x][y] == distTresor) distTresor++;

		// Pour tous les voisins.
		for(int sX = -1; sX <= 1; sX++)
		{
			for (int sY = -1; sY <= 1; sY++)
			{
				// On ne veut pas étudier l'élément actuel, donc lorsqu'on est dessus, on saute.
				if (sX == 0 && sY == 0) continue;

				// On récupère la position du voisin de l'élément. 
				voisinX = x + sX;
				voisinY = y + sY;

				// Si ce voisin sort du tableau, on le saute.
				if (voisinX < 0 || voisinX >= heightLaby || voisinY < 0 || voisinY >= widthLaby) continue;

				// Si ce voisin n'a pas été étudié.
				if (_innond[voisinX][voisinY] == -1) // Bleu
				{
					// On le met dans la file.
					queueX.push(voisinX);
					queueY.push(voisinY);

					// On lui met une position.
					_innond[voisinX][voisinY] = distTresor; // Blanc
				}
			}
		}
	}
	return distTresor;
}

bool Labyrinthe::set_data(int i, int j, int b){

	if((int)_data[i][j] == 1){
		return false;
		std::cout << "error" << std::endl;
	}
	
	else{
		_data[i][j] = b;
	}
	return true;
}

// retourne la case (i, j).
char Labyrinthe::data(int i, int j)
{
	if(i < 0 || i > heightLaby || j < 0 || j > widthLaby)
	{
		std::cout << "error data" << std::endl;
		return 1;
	}

	return _data[i][j];
}

// retourne la case (i, j).
int Labyrinthe::innond(int i, int j)
{
	if (i < 0 || i > heightLaby || j < 0 || j > widthLaby)
	{
		std::cout << "error data" << std::endl;
		return -2;
	}

	return _innond[i][j];
}

//provisoire
void Labyrinthe::display_tab(){
	int a;
	for(int i = 0; i < width(); i++){
		for(int j = 0; j < height(); j++){
			a = data(i, j);
			std::cout << a;
		}
		std::cout << std::endl;
	}
	std::cout << "val : "<< (int)data(10, 13) <<std::endl;
}

