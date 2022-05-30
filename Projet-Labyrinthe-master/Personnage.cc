#include <iostream>
#include <math.h>
#include <unistd.h>
#include "Personnage.h"
#include "Mover.h"

Personnage::Personnage(Labyrinthe *l, const char *modele, int life) : Mover(120, 80, l, modele){
    _life = life;
}