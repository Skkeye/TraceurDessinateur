
#include "TDProto.h"
#include <Arduino.h>

// function to build custom protocol from tData
String buildData(tData data)
{
  String sData = ">data>";
  sData += "angle_x:" + String(data.u8AngleX) + ";";
  sData += "angle_y:" + String(data.u8AngleY) + ";";
  sData += "bouton:" + String(data.u8ButtonValue) + ";";
  sData += ">end>";
  return sData;
}


// function to parse custom protocol
String parseData(String data)
{
  // data : >data>angle_x:0;angle_y:0;bouton:0;>end>
  // ack : >ack>>end>
  // cmd : >cmd>gyrozero>end>
  // cmd : >cmd>restart>end>
  if (data.startsWith(">data>"))
  {
    // data
  } else if (data.startsWith(">ack>")) {
    // ack
  } else if (data.startsWith(">cmd>")) {
    // cmd
  }
}