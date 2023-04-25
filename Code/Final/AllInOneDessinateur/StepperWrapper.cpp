/**
 * @file StepperWrapper.cpp
 * @brief Le code pour manipuler des moteurs steppers
 *
 * Olivier David Laplante -- 8 Avril 2023 : Creation
 */

#include "StepperWrapper.h"
#include <Arduino.h>

void StepperWrapper::initPins() {
  pinMode(this->dir_pin, OUTPUT);
  pinMode(this->step_pin, OUTPUT);
  pinMode(this->en_pin, OUTPUT);
  pinMode(this->home_pin, INPUT);
  digitalWrite(this->dir_pin, LOW);
  digitalWrite(this->step_pin, LOW);
  digitalWrite(this->en_pin, HIGH); // HIGH = disabled
}

StepperWrapper::StepperWrapper(int dir, int step, int en, int home,
                               int max_distance, bool flip_dir) {
  this->dir_pin = dir;
  this->step_pin = step;
  this->en_pin = en;
  this->home_pin = home;
  this->distance_from_origin = 0;
  this->max_distance_from_origin = max_distance;
  this->positive_direction = !flip_dir;
  this->direction = false;
  this->initPins();
}

void StepperWrapper::step(bool force = false) {
  // check if we are at the limit

  // if (this->distance_from_origin >= this->max_distance_from_origin && !force) {
  //   if (this->direction == this->positive_direction) {
  //     delayMicroseconds(50);
  //     return;
  //   }
  // } else if (this->distance_from_origin <= 0 && !force) {
  //   if (this->direction != this->positive_direction) {
  //     delayMicroseconds(50);
  //     return;
  //   }
  // }
  // // keep track of distance from origin
  // if (this->direction) {
  //   this->distance_from_origin++;
  // } else {
  //   this->distance_from_origin--;
  // }
  digitalWrite(this->step_pin, HIGH);
  delayMicroseconds(25);
  digitalWrite(this->step_pin, LOW);
  delayMicroseconds(25);
}

void StepperWrapper::setDir(bool dir) {
  this->direction = dir;
  digitalWrite(this->dir_pin, this->positive_direction == dir);
}

void StepperWrapper::setEnabled(bool en) {
  digitalWrite(this->en_pin, !en);
}

bool StepperWrapper::isHome() {
  return digitalRead(this->home_pin);
}

ThreeAxisStepper::ThreeAxisStepper(StepperWrapper *stepperX,
                                   StepperWrapper *stepperY,
                                   StepperWrapper *stepperZ) {
  this->stepper_x = stepperX;
  this->stepper_y = stepperY;
  this->stepper_z = stepperZ;
}

bool ThreeAxisStepper::doesStep(int this_step, int count, int steps, int largest_step_count) {
  int num = (int)ceilf(count * (float)((float)largest_step_count / (float)(steps + 1)));
  return (num == this_step && count <= steps);
}

void ThreeAxisStepper::move(int deltaX, int deltaY, int deltaZ) {
  // On met les directions des moteurs  
  (*this->stepper_x).setDir(deltaX >= 0);
  (*this->stepper_y).setDir(deltaY >= 0);
  (*this->stepper_z).setDir(deltaZ >= 0);
  
  // Attendre pour que les moteurs soient prêts
  delayMicroseconds(10);
  
  // Normaliser les pas
  deltaX *= (deltaX < 0) ? -1 : 1;
  deltaY *= (deltaY < 0) ? -1 : 1;
  deltaZ *= (deltaZ < 0) ? -1 : 1;

  // Trouver le plus grand nombre de pas à faire
  int largest_step_count = (deltaX > deltaY) ? deltaX : deltaY;
  largest_step_count = (largest_step_count > deltaZ) ? largest_step_count : deltaZ;
  // Initialiser les compteurs de pas pour chaque moteur
  int count_x = 1, count_y = 1, count_z = 1;

  // Faire les pas un à un
  for (int this_step = 1; this_step < largest_step_count + 1; this_step++) {
    if (doesStep(this_step, count_x, deltaX, largest_step_count)) { // Si on doit faire un pas pour ce moteur
      (*this->stepper_x).step(); // Faire le pas
      count_x++; // Incrémenter le compteur de pas pour ce moteur (pour savoir quand on doit faire un pas)
    } else { // Sinon, attendre pour compenser les pas manqués
      delayMicroseconds(50); 
    }
    if (doesStep(this_step, count_y, deltaY, largest_step_count)) { // Si on doit faire un pas pour ce moteur
      (*this->stepper_y).step(); // Faire le pas
      count_y++; // Incrémenter le compteur de pas pour ce moteur (pour savoir quand on doit faire un pas)
    } else { // Sinon, attendre pour compenser les pas manqués
      delayMicroseconds(50);
    }
    if (doesStep(this_step, count_z, deltaZ, largest_step_count)) { // Si on doit faire un pas pour ce moteur
      (*this->stepper_z).step(); // Faire le pas
      count_z++; // Incrémenter le compteur de pas pour ce moteur (pour savoir quand on doit faire un pas)
    } else { // Sinon, attendre pour compenser les pas manqués
      delayMicroseconds(50);
    }
  }
}

void ThreeAxisStepper::home() {
  
  // un moteur à la fois
  while (!(*this->stepper_x).isHome()) {
    (*this->stepper_x).setDir(true);
    (*this->stepper_x).step(true);
    delayMicroseconds(90);
  }
  while (!(*this->stepper_y).isHome()) {
    (*this->stepper_y).setDir(false);
    (*this->stepper_y).step(true);
    delayMicroseconds(90);
  }
  while (!(*this->stepper_z).isHome()) {
    (*this->stepper_z).setDir(false);
    (*this->stepper_z).step(true);
    delayMicroseconds(90);
  }

  // remettre les distances à 0
  (*this->stepper_x).distance_from_origin = 0;
  (*this->stepper_y).distance_from_origin = 0;
  (*this->stepper_z).distance_from_origin = 0;
}

void ThreeAxisStepper::disable() {
  (*this->stepper_x).setEnabled(false);
  (*this->stepper_y).setEnabled(false);
  (*this->stepper_z).setEnabled(false);
}

void ThreeAxisStepper::enable() {
  (*this->stepper_x).setEnabled(true);
  (*this->stepper_y).setEnabled(true);
  (*this->stepper_z).setEnabled(true);
}
