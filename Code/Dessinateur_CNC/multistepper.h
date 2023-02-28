
#include "pico/stdlib.h"

const uint8_t puicStepMasks[4] = { 0b00000101, 0b00000110, 0b00001010, 0b00001001 };

typedef uint32_t *maskList_t;

int8_t iCurrentStepStateX = 0;
int8_t iCurrentStepStateY = 0;
int8_t iCurrentStepStateZ = 0;

// Fonction qui calcule les intervalles pour les steps
int iStepInterval(int n, int max, int reqSteps);

// Fonction qui retourne le plusGrand des 3 int
int iBiggestVal(int a, int b, int c);

// Fonction qui genere une liste de masques pour les
// gpio pour faire tourner les steppers
maskList_t computeStepMasks(int stepsX, int stepsY, int stepsZ);
