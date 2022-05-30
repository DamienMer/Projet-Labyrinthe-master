# Projet POA

## Changelog

## [Build 200507.1] - 2020-05-07

### TODO

- Téléportation.
- Changement de niveau.

### Added

- (A) Ajout des positions d'arrivée pour les téléporteurs.

### Fixed

- (A) Correction de la lecture laby (certains murs n'étaient pas pris en compte).
- (A) Correction de la lecture des affiches.
- (A) Mode défense normalement correct.


## [Build 200416.1] - 2020-04-21

### TODO

- Correction Labyrinthe (certains murs ne sont pas pris en compte)
- Mode défense.
- Téléportation.
- Changement de niveau.

### Added

- (D) Class Personnage. Permet de modifier plus simplement le labyrinthe et les personnages
- (D) Système de PV. Les Personnages peuvent être tués s'ils n'ont plus de PV
- (D) Les gardiens perdent en précision en fonction de leurs PV restants
- (D) Un Gadien tue instantannément le Chasseur en le touchant
- (D) Collisions. Les Gardiens et le Chasseur ne rentre plus en collision
- (D) Objectif de Victoire. S'il n'y a plus de Gardiens ou si le Chasseur touche le trésor, la partie 		est gagné (un message s'affiche)


## [Build 200415.1] - 2020-04-15

### TODO

- Mode attaque (déplacement vers le joueur en lui tirant dessus).
- Mode défense.
- Téléportation.
- Changement de niveau.

### Added

- (A/D) Ajout initial du mode attaque (suit le Chasseur en tirant, prise en compte des obstacles).
- (D) Ajout traque du gardien. Quand il aggresse le chasseur, s'il le perd de vue il ira à la dernière position connu du chasseur
- (D) Ajout Rotation/Alerte. Lorsque le chasseur entre dans le champs de vision du gardien, celui-ci passe en "alerte" et tourne jusqu'à être face au chasseur avant de
	passer en mode "aggressif".
- (D) Ajout Bloquage de vue. La vue du gardien est obstruée par les murs et les caisses.
- (A) Ajout de la distance max du laby dans Laby.cc et .h.
- (A) Ajout initial du mode défense.

### Fixed

- (D) Correction sur les angles qui allaient de -180 à 180. Ils sont désormais entre 0 et 360


## [Build 200414.1] - 2020-04-14

### TODO

- Mode attaque (déplacement vers le joueur en lui tirant dessus).
- Mode défense.
- Téléportation.
- Changement de niveau.

### Added

- (A) Ajout de l'algo d'innondation (parcours en longueur pour l'instant).
- (A) Ajout de l'algo innondation en BFS.
- (D) Gardien tire sur le Chasseur.
- (D) La boule explose sur le joueur.

### Changed

- (A) Suppression des vectors après lecture du laby.

### Fixed

- (A) Correction de la position des Gardiens. Les Gardiens ne rentre plus dans les objets.


## [Build 200410.1] - 2020-04-10

### Added

- (D) Déplacement des Gardiens aléatoire (Mode patrouille).

### Fixed

- (A) Correction au niveau de la lecture du laby qui sortait une représentation mirroir.


## [Build 200405.1] - 2020-04-05

### Added

- (A/D) Ajout d'une version initial d'un lecteur de carte.

### Fixed

- (A) Multiplication des positions des persos par 10 (pour avoir les bonnes positions).
- (A) Remplissage des tableaux finaux manuellement car c'est peut-être la source de plantages (A voir).
- (A) Correction de la taille de la grille (erreur lors de la création de _data).
- (A) Dans char data(), inversion des i et j (car mon tableau est _data[height][width]).

### Added

- Marche aléatoire du gardien
- Ajout display_tab, afficher le contenu de _data
- Ajout des hitbox qui suivent le gardien et le chasseur

