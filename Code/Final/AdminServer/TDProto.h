#include <Arduino.h>


enum eCommand {
  cmdGyroZero,
  cmdHome,
  cmdStop,
  cmdStart,
  cmdPause,
  cmdResume,
  cmdRestart
};

typedef struct {
  uint8_t u8AngleX;
  uint8_t u8AngleY;
  uint8_t u8ButtonValue;
} tData;

// data : >data>angle_x:0;angle_y:0;bouton:0;>end>
// ack : >ack>>end>
// req : >req>data>end>
// req : >req>cmd>end>
// cmd : >cmd>0>end>
// cmd : >cmd>1>end>

// function definition to build custom protocol from tData
String buildData(tData data);

// function definition to parse custom protocol
String parseData(String data);