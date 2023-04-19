
#include "TDProto.h"
#include <Arduino.h>
#include "global.h"

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
void parseData(String data)
{
  // data : >data>angle_x:0;angle_y:0;bouton:0;>end>
  // ack : >ack>>end>
  // cmd : >cmd>gyrozero>end>
  // cmd : >cmd>restart>end>
  if (data.startsWith(">data>"))
  {
    // data
    uint8_t u8AngleX = data.substring(6, data.indexOf(";")).toInt();
    uint8_t u8AngleY = data.substring(data.indexOf(";") + 1, data.lastIndexOf(";")).toInt();
    uint8_t u8ButtonValue = data.substring(data.lastIndexOf(";") + 1, data.indexOf(">end>")).toInt();
    // Serial.println(iAngleX);
    // Serial.println(iAngleY);
    // Serial.println(iButtonValue);
  } else if (data.startsWith(">ack>")) {
    // ack
  } else if (data.startsWith(">cmd>")) {
    // cmd
    if (data.substring(5, data.indexOf(">end>")) == "gyrozero")
    {
      recalibrateIMU();
      Serial.println("Recalibration du IMU");
    } else if (data.substring(5, data.indexOf(">end>")) == "restart") {
      delay(1000);
      esp_restart();
    }
  }
}