

#ifndef TDProto_h
#define TDProto_h

#include "global.h"




struct Data_t {
  uint8_t u8AngleX;
  uint8_t u8AngleY;
  uint8_t u8ButtonValue;
};

// data : >data>angle_x:0;angle_y:0;bouton:0;>end>
// ack : >ack>>end>
// cmd : >cmd>gyrozero>end>
// cmd : >cmd>restart>end>

// function definition to build custom protocol from tData
String buildData(Data_t data);

// function definition to parse custom protocol
void parseData(String data, Data_t &xData, bool &paused, ThreeAxisStepper *steppers);


#endif