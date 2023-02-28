

#ifndef NeoStepper_h
#define NeoStepper_h

#include "pico/stdlib.h"

class NeoStepper {
public:
  // Constructor
  NeoStepper(int pinA0, int pinA1, int pinB0, int pinB1);

  void step(bool dir);

private:
  // Identification
  int _pinA0, _pinA1, _pinB0, _pinB1;
  int axis;  // X: 0; Y: 1; Z: 2;
  // State
  int _currentState = 0;
  int _states[8] = { 0b0001, 0b0101, 0b0100, 0b0110, 0b0010, 0b1010, 0b1000, 0b1001 };
};

#endif
