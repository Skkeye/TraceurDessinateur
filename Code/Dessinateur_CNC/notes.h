
#include "pico/stdlib.h"
#include <math.h>

typedef struct {
  float count;
  float ratio;
  int steps;
} SingleAxis_t;

typedef struct {
  SingleAxis_t x;
  SingleAxis_t y;
  SingleAxis_t z;
} Axis_t;

Axis_t xAxis;

void initAxis(Axis_t axis, int stepsX, int stepsY, int stepsZ);

bool doesStep(SingleAxis_t singleAxis, int thisStep);
