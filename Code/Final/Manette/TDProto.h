#include <Arduino.h>



typedef struct {
  uint8_t u8AngleX;
  uint8_t u8AngleY;
  uint8_t u8ButtonValue;
} tData;

// data : >data>angle_x:0;angle_y:0;bouton:0;>end>
// ack : >ack>>end>
// cmd : >cmd>gyrozero>end>
// cmd : >cmd>restart>end>

// function definition to build custom protocol from tData
String buildData(tData data);

// function definition to parse custom protocol
void parseData(String data);