#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <iostream>
#include <vector>
#include <math.h>
#include <unistd.h>

#include "Mover.h"
#include "Labyrinthe.h"

class Labyrinthe;
class Personnage;

class Personnage : public Mover{
    private:
    public:
        //vie
        int _life;
        //vector qui contient tous les perso
        std::vector<Personnage*> _allPerso;
        //le labyrinthe
        Labyrinthe* _myLaby;
        //si le personnage est défenseur ou pas
        bool _defenseur;


        Personnage(Labyrinthe *l, const char *modele, int life) : Mover(120, 80, l, modele){
            _life = life;
            _myLaby = l;
        }
        
        int getLife(){
            return _life;
        }

        void setLife(int i){
            _life = i;
        }

        Labyrinthe* getLabyrinthe(){
            return _myLaby;
        }

        int getInnond(int i, int j){
            return _myLaby->innond(i, j);
        }

};

#endif
