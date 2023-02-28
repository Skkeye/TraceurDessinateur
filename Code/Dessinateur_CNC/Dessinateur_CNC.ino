#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"

// Definitions des broches
#define PIN_LIMIT_X 12
#define PIN_LIMIT_Y 13
#define PIN_LIMIT_Z 14
#define PIN_MOSI 19
#define PIN_MISO 16
#define PIN_CS 17
#define PIN_SCK 18

// Struct pour stocker une coordonee 3d
struct uiVector3 {
  uint x = 0;
  uint y = 0;
  uint z = 0;
};

// Struct contenant les informations necessaires pour un deplacement
struct uiInstruction {
  uiVector3 delta;
  uiVector3 speed;
};

// Purement pour controller les Steppers
void core1_entry() {
}

void setup() {
}

void loop() {
}
