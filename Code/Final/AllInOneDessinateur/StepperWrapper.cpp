
#include "global.h"

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

  if (this->distance_from_origin >= this->max_distance_from_origin && !force) {
    if (this->direction <= this->positive_direction) {
      // delayMicroseconds(50); // to vTaskDelay
      // 50 microsecondes = 1/20 000 000 secondes = 1/20 000 millisecondes
      vTaskDelay(0.01 / portTICK_PERIOD_MS);
      return;
    }
  } else if (this->distance_from_origin == 0 && !force) {
    if (this->direction != this->positive_direction) {
      vTaskDelay(0.01 / portTICK_PERIOD_MS);
      return;
    }
  }
  // keep track of distance from origin
  if (this->direction == this->positive_direction) {
    this->distance_from_origin++;
  } else if (this->distance_from_origin > 0) {
    this->distance_from_origin--;
  }
  digitalWrite(this->step_pin, HIGH);
  vTaskDelay(0.005 / portTICK_PERIOD_MS);
  digitalWrite(this->step_pin, LOW);
  vTaskDelay(0.005 / portTICK_PERIOD_MS);
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
  (*this->stepper_x).setDir(deltaX >= 0);
  (*this->stepper_y).setDir(deltaY >= 0);
  (*this->stepper_z).setDir(deltaZ >= 0);
  vTaskDelay(0.01 / portTICK_PERIOD_MS);
  deltaX *= (deltaX < 0) ? -1 : 1;
  deltaY *= (deltaY < 0) ? -1 : 1;
  deltaZ *= (deltaZ < 0) ? -1 : 1;
  int largest_step_count = (deltaX > deltaY) ? deltaX : deltaY;
  largest_step_count =
      (largest_step_count > deltaZ) ? largest_step_count : deltaZ;
  int count_x = 1, count_y = 1, count_z = 1;
  for (int this_step = 1; this_step < largest_step_count + 1; this_step++) {
    // (*this->stepper_x).step();
    if (doesStep(this_step, count_x, deltaX, largest_step_count)) {
      (*this->stepper_x).step();
      count_x++;
    } else {
      vTaskDelay(0.01 / portTICK_PERIOD_MS);
    }
    if (doesStep(this_step, count_y, deltaY, largest_step_count)) {
      (*this->stepper_y).step();
      count_y++;
    } else {
      vTaskDelay(0.01 / portTICK_PERIOD_MS);
    }
    if (doesStep(this_step, count_z, deltaZ, largest_step_count)) {
      (*this->stepper_z).step();
      count_z++;
    } else {
      vTaskDelay(0.01 / portTICK_PERIOD_MS);
    }
  }
}

void ThreeAxisStepper::home() {
  // one at a time
  while (!(*this->stepper_x).isHome()) {
    (*this->stepper_x).setDir(false);
    (*this->stepper_x).step(true);
    vTaskDelay(0.01 / portTICK_PERIOD_MS);
  }
  while (!(*this->stepper_y).isHome()) {
    (*this->stepper_y).setDir(false);
    (*this->stepper_y).step(true);
    vTaskDelay(0.01 / portTICK_PERIOD_MS);
  }
  while (!(*this->stepper_z).isHome()) {
    (*this->stepper_z).setDir(false);
    (*this->stepper_z).step(true);
    vTaskDelay(0.01 / portTICK_PERIOD_MS);
  }

  // while (!(*this->stepper_x).isHome() && !(*this->stepper_y).isHome() &&
  //        !(*this->stepper_z).isHome()) {
  //   int home_step_x = (*this->stepper_x).isHome() - 1;
  //   int home_step_y = (*this->stepper_y).isHome() - 1;
  //   int home_step_z = (*this->stepper_z).isHome() - 1;
  //   this->move(home_step_x, home_step_y, home_step_z);
  //   delayMicroseconds(30);
  // }
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
