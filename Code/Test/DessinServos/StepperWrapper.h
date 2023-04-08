



#ifndef STEPPER_WRAPPER_H
#define STEPPER_WRAPPER_H

class StepperWrapper {
private:
  int dir_pin, step_pin, en_pin, home_pin;
  int distance_from_origin;
  int max_distance_from_origin;
  bool positive_direction;

  void initPins();

public:
  StepperWrapper(int dir, int step, int en, int home, int max_distance, bool flip_dir = false);
  ~StepperWrapper();

  void step();

  void setDir(bool dir);

  void setEnabled(bool en);

  bool isHome();
};


class ThreeAxisStepper {
private:
  StepperWrapper *stepper_x, *stepper_y, *stepper_z;

  bool doesStep(int this_step, int count, int steps_moving, int largest_step_count);
public:
  ThreeAxisStepper(StepperWrapper *stepperX, StepperWrapper *stepperY, StepperWrapper *stepperZ);
  ~ThreeAxisStepper();

  void move(int deltaX, int deltaY, int deltaZ);

  void home();

  void disable();
  void enable();
};

#endif