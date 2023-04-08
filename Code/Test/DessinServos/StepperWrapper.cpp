
#include "StepperWrapper.h"
#include <Arduino.h>


void StepperWrapper::initPins()
{
  Serial.println("Init pins");
  pinMode(this->dir_pin, OUTPUT);
  pinMode(this->step_pin, OUTPUT);
  pinMode(this->en_pin, OUTPUT);
  pinMode(this->home_pin, INPUT);
  digitalWrite(this->dir_pin, LOW);
  digitalWrite(this->step_pin, LOW);
  digitalWrite(this->en_pin, HIGH);   // HIGH = disabled
}

StepperWrapper::StepperWrapper(int dir, int step, int en, int home, int max_distance, bool flip_dir)
{
  Serial.println("Constructor wrapper");
  this->dir_pin = dir;
  this->step_pin = step;
  this->en_pin = en;
  this->home_pin = home;
  this->distance_from_origin = 0;
  this->max_distance_from_origin = max_distance;
  this->positive_direction = !flip_dir;
  this->initPins();
}

StepperWrapper::~StepperWrapper()
{
}

void StepperWrapper::step()
{
  Serial.println("Step");
  digitalWrite(this->step_pin, HIGH);
	delayMicroseconds(10);
	digitalWrite(this->step_pin, LOW);
	delayMicroseconds(10);
}

void StepperWrapper::setDir(bool dir)
{
  Serial.print("Dir set: ");
  Serial.println(dir);
  digitalWrite(this->dir_pin, this->positive_direction == dir);
}

void StepperWrapper::setEnabled(bool en)
{
  Serial.print("En set: ");
  Serial.println(en);
  digitalWrite(this->en_pin, !en);
}

bool StepperWrapper::isHome()
{
  Serial.println("Is home");
  return digitalRead(this->home_pin);
}


ThreeAxisStepper::ThreeAxisStepper(StepperWrapper *stepperX, StepperWrapper *stepperY, StepperWrapper *stepperZ)
{
  Serial.println("TripleStepper constructor");
  this->stepper_x = stepperX;
  this->stepper_y = stepperY;
  this->stepper_z = stepperZ;
}

ThreeAxisStepper::~ThreeAxisStepper()
{
}

bool ThreeAxisStepper::doesStep(int this_step, int count, int steps, int largest_step_count)
{
  int num = (int)ceilf(count * (float)((float)largest_step_count / (float)(steps + 1)));
  Serial.print("Does step");
  Serial.println((num == this_step && count <= this_step));
  return (num == this_step && count <= this_step);
}

void ThreeAxisStepper::move(int deltaX, int deltaY, int deltaZ)
{
  Serial.println("Move");
  (*this->stepper_x).setDir(deltaX >= 0);
  (*this->stepper_y).setDir(deltaY >= 0);
  (*this->stepper_z).setDir(deltaZ >= 0);
  deltaX *= -(deltaX < 0);
  deltaY *= -(deltaY < 0);
  deltaZ *= -(deltaZ < 0);
  int largest_step_count = (deltaX > deltaY) ? deltaX : deltaY;
  largest_step_count = (largest_step_count > deltaZ) ? largest_step_count : deltaZ;
  int count_x, count_y, count_z = 1;
  for(int this_step = 1; this_step < largest_step_count + 1; this_step++) {
    if(doesStep(this_step, count_x, deltaX, largest_step_count)) {
      Serial.println("StepperX");
      (*this->stepper_x).step();
      count_x++;
    }
    if(doesStep(this_step, count_y, deltaY, largest_step_count)) {
      Serial.println("StepperY");
      (*this->stepper_y).step();
      count_y++;
    }
    if(doesStep(this_step, count_z, deltaZ, largest_step_count)) {
      Serial.println("StepperZ");
      (*this->stepper_z).step();
      count_z++;
    }
    delay(1); // TODO: find minimum delay
  }
}

void ThreeAxisStepper::home()
{
  Serial.println("Big home");
  while(!(*this->stepper_x).isHome() && !(*this->stepper_y).isHome() && !(*this->stepper_z).isHome()) {
    int home_step_x = (*this->stepper_x).isHome() - 1;
    int home_step_y = (*this->stepper_y).isHome() - 1;
    int home_step_z = (*this->stepper_z).isHome() - 1;
    this->move(home_step_x, home_step_y, home_step_z);
    delay(2);
  }
}

void ThreeAxisStepper::disable()
{
  Serial.println("All disabled");
  (*this->stepper_x).setEnabled(false);
  (*this->stepper_y).setEnabled(false);
  (*this->stepper_z).setEnabled(false);
}

void ThreeAxisStepper::enable()
{
  Serial.println("All enabled");
  (*this->stepper_x).setEnabled(true);
  (*this->stepper_y).setEnabled(true);
  (*this->stepper_z).setEnabled(true);
}