#ifndef UTIPE_CONSTANTS_H
#define UTIPE_CONSTANTS_H

//window
#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT ((WINDOW_WIDTH * 9) / 16)
#define FPS           60

//Corpus
#define number_of_objects 1
#define RADIUS_OBJECT (WINDOW_HEIGHT / 85)

//Planet
#define number_of_planets 3
#define RADIUS_PLANET (WINDOW_WIDTH / 70)

//Physic
#define G  100000 //gravitational constant
#define FRICTION_COEFF 0.1

//Simulation
#define SUB_STEPS 1000 //nombre de calculs avant affichage //4000

//Optimisation
#define SOFTENING_SQ 200 // Paramètre epsilon au carré (800 initialement, diminuée pour une série de tests)

#define CAPTURATION_TOLERANCE_KINETIC_ENERGY 150 //Joule d'énergie cinétique pour considérer que l'objet est capturé par une planète
#define CAPTURATION_TOLERANCE_POSITION_DIFFERENCE 30 //tolérance de nombres de pixels de différence entre l'objet et les planètes pour considérer que l'objet est capturé par une planète

//Simulation par frame
#define SIM_STEPS_PER_FRAME 5000 //nombre d'itérations de physique par frame de rendu

//Texture
#define TEXTURE_UPDATE_STEPS WINDOW_WIDTH //nombre de pixels avant mise à jour de la texture GPU

#endif //UTIPE_CONSTANTS_H
