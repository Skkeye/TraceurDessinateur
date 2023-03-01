
#include "notes.h"


void initAxis(Axis_t axis, int stepsX, int stepsY, int stepsZ) {
  int iMax = (stepsX >= stepsY) ? stepsX : stepsY;
  iMax = (iMax >= stepsZ) ? iMax : stepsZ;

  axis.x.ratio = (float)((float)iMax / ((float)stepsX + 1));
  axis.y.ratio = (float)((float)iMax / ((float)stepsY + 1));
  axis.z.ratio = (float)((float)iMax / ((float)stepsZ + 1));

  axis.x.count = 1;
  axis.y.count = 1;
  axis.z.count = 1;

  axis.x.steps = stepsX;
  axis.y.steps = stepsY;
  axis.z.steps = stepsZ;
}

bool doesStep(SingleAxis_t singleAxis, int thisStep) {
  thisStep++;  // 0 based
  int num = (int)ceilf(singleAxis.count * singleAxis.ratio);
  if (num == thisStep && singleAxis.count <= singleAxis.steps) {
    singleAxis.count++;
    return true;
  }
  return false;
}