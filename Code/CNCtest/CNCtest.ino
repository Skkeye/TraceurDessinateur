#include "pico/stdlib.h"
#include "hardware/gpio.h"


#define PIN_STOP_X 12
#define PIN_STEPPER_A0 0
#define PIN_STEPPER_A1 1
#define PIN_STEPPER_B0 2
#define PIN_STEPPER_B1 3

const int stepTable[8] = { 0b00001110, 0b00001010, 0b00001011, 0b00001001, 0b00001101, 0b00000101, 0b00000111, 0b00000110 };
int currentStep = 0;
SerialUSB xUSBSerial = SerialUSB();

void step(bool dir) {
  currentStep += (dir) ? 1 : -1;
  currentStep = (currentStep == 8) ? 0 : currentStep;
  currentStep = (currentStep == -1) ? 7 : currentStep;
  digitalWrite(PIN_STEPPER_A0, (stepTable[currentStep] & 1) != 0);
  digitalWrite(PIN_STEPPER_A1, (stepTable[currentStep] & 2) != 0);
  digitalWrite(PIN_STEPPER_B0, (stepTable[currentStep] & 4) != 0);
  digitalWrite(PIN_STEPPER_B1, (stepTable[currentStep] & 8) != 0);
}

void sendDigit(int digit) {
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = digit; i > 0; i--) {
    digitalWrite(LED_BUILTIN, 1);
    delay(200);
    digitalWrite(LED_BUILTIN, 0);
    delay(200);
  }
  digitalWrite(LED_BUILTIN, 1);
  delay(700);
  digitalWrite(LED_BUILTIN, 0);
  delay(700);
}

void sendNum(int num) {
  int filter = (int)(num / 10);
  sendDigit(num - (10 * filter));
  if (filter == 0) return;

  sendNum(filter);
}

void home() {
  int iCount = 0;
  while (digitalRead(PIN_STOP_X) == 0) {
    step(true);
    iCount++;
    delay(2);
  }

  sendNum(iCount);
}

void setup() {
  xUSBSerial.begin();
  pinMode(PIN_STEPPER_A0, OUTPUT);
  pinMode(PIN_STEPPER_A1, OUTPUT);
  pinMode(PIN_STEPPER_B0, OUTPUT);
  pinMode(PIN_STEPPER_B1, OUTPUT);
  pinMode(PIN_STOP_X, INPUT);

  home();
  for (int i = 0; i < 12845; i++) {
    step(false);
    delay(2);
  }
  // delay(1000);
}

void loop() {
  // for (int i = 0; i < 12845; i++) {
  //   step(true);
  //   delay(2);
  // }
  // delay(1000);
  // for (int i = 0; i < 1000; i++) {
  //   step(false);
  //   delay(2);
 
