
#include <stdlib.h>
#include <math.h>
#include "multistepper.h"

int iStepInterval(int n, int max, int reqSteps) {
  n = (n <= reqSteps) ? n : -1;
  return (int)ceilf(n * ((float)max / ((float)reqSteps + 1)));
}

int iBiggestVal(int a, int b, int c) {
  int iBig = a;
  iBig = (iBig < b) ? b : iBig;
  iBig = (iBig < c) ? c : iBig;
  return iBig;
}

maskList_t computeStepMasks(int stepsX, int stepsY, int stepsZ) {
  int absStepsX = abs(stepsX);
  int absStepsY = abs(stepsY);
  int absStepsZ = abs(stepsZ);
  int stepCountX = 1;
  int stepCountY = 1;
  int stepCountZ = 1;
  uint32_t maskList[1000];  // TODO: check if it would be better to pass this via an argument (pointer)
  int largestStepCount = iBiggestVal(absStepsX, absStepsY, absStepsZ);

  if (absStepsX != 0) {
    int isStep = 0;
    for (int index = 0; index < largestStepCount; index++) {
      isStep = (int)(iStepInterval(stepCountX, largestStepCount, absStepsX) == (index + 1));
      stepCountX += isStep;
      iCurrentStepStateX += isStep * (stepsX / absStepsX);
      iCurrentStepStateX = (iCurrentStepStateX < 0) ? 3 : iCurrentStepStateX;
      iCurrentStepStateX = (iCurrentStepStateX > 3) ? 0 : iCurrentStepStateX;
      maskList[index] = 0x00000000;
      maskList[index] |= puicStepMasks[iCurrentStepStateX];
    }
  } else {
    for (int index = 0; index < largestStepCount; index++) {
      maskList[index] = 0x00000000;
      maskList[index] |= puicStepMasks[iCurrentStepStateX];
    }
  }

  if (absStepsY != 0) {
    int isStep = 0;
    for (int index = 0; index < largestStepCount; index++) {
      isStep = (int)(iStepInterval(stepCountY, largestStepCount, absStepsY) == (index + 1));
      stepCountY += isStep;
      iCurrentStepStateY += isStep * (stepsY / absStepsY);
      iCurrentStepStateY = (iCurrentStepStateY < 0) ? 3 : iCurrentStepStateY;
      iCurrentStepStateY = (iCurrentStepStateY > 3) ? 0 : iCurrentStepStateY;
      maskList[index] |= (puicStepMasks[iCurrentStepStateY] << 4);
    }
  } else {
    for (int index = 0; index < largestStepCount; index++) {
      maskList[index] |= (puicStepMasks[iCurrentStepStateY] << 4);
    }
  }

  if (absStepsZ != 0) {
    int isStep = 0;
    for (int index = 0; index < largestStepCount; index++) {
      isStep = (int)(iStepInterval(stepCountZ, largestStepCount, absStepsZ) == (index + 1));
      stepCountZ += isStep;
      iCurrentStepStateZ += isStep * (stepsZ / absStepsZ);
      iCurrentStepStateZ = (iCurrentStepStateZ < 0) ? 3 : iCurrentStepStateZ;
      iCurrentStepStateZ = (iCurrentStepStateZ > 3) ? 0 : iCurrentStepStateZ;
      maskList[index] |= (puicStepMasks[iCurrentStepStateZ] << 8);
    }
  } else {
    for (int index = 0; index < largestStepCount; index++) {
      maskList[index] |= (puicStepMasks[iCurrentStepStateY] << 8);
    }
  }

  return maskList;  // TODO: (followup) I think I need to pass this via pointer and created/malloc outside of scope
}
