#include "StepperWrapper.h"

//  (dir_pin, step_pin, en_pin, home_pin, max_steps)
StepperWrapper stepper_x(0, 1, 2, 12, 12845);
StepperWrapper stepper_y(4, 5, 6, 13, 10752);
StepperWrapper stepper_z(8, 9, 10, 14, 1325);
ThreeAxisStepper steppers(&stepper_x, &stepper_y, &stepper_z);

void setup()
{
  steppers.enable();
}

void loop()
{
	delay(17);
  steppers.move(30, 13, -9);
  delay(17);
  steppers.move(-18, 3, 30);
}
