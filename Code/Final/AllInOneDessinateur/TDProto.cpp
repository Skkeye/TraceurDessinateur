

#include "TDProto.h"
#include "global.h"

// function to build custom protocol from Data_t
// String buildData(Data_t data)
// {
//   String sData = ">data>";
//   sData += "angle_x:" + String(data.u8AngleX) + ";";
//   sData += "angle_y:" + String(data.u8AngleY) + ";";
//   sData += "bouton:" + String(data.u8ButtonValue) + ";";
//   sData += ">end>";
//   return sData;
// }


// function to parse custom protocol
void parseData(String data, Data_t &xData, bool &paused, ThreeAxisStepper *steppers)
{
  // data : >data>angle_x:0;angle_y:0;bouton:0;>end>
  // ack : >ack>>end>
  // cmd : >cmd>gyrozero>end>
  // cmd : >cmd>restart>end>
  if (data.startsWith(">data>"))
  {
    // data
    
    xData.u8AngleX = data.substring(6, data.indexOf(";")).toInt();
    xData.u8AngleY = data.substring(data.indexOf(";") + 1, data.lastIndexOf(";")).toInt();
    xData.u8ButtonValue = data.substring(data.lastIndexOf(";") + 1, data.indexOf(">end>")).toInt();

    // Serial.println(iAngleX);
    // Serial.println(iAngleY);
    // Serial.println(iButtonValue);
  } else if (data.startsWith(">ack>")) {
    // ack
  } else if (data.startsWith(">cmd>")) {
    // cmd
    if (data.substring(5, data.indexOf(">end>")) == "restart") {
      Serial.println("Redémarrage du module");
      vTaskDelay(pdMS_TO_TICKS(1000));
      rp2040.reboot();
    } else if (data.substring(5, data.indexOf(">end>")) == "home") {
      Serial.println("Home");
      paused = true;
      steppers->home();
      paused = false;
    } else if (data.substring(5, data.indexOf(">end>")) == "start") {
      Serial.println("Start");
      steppers->enable();
    } else if (data.substring(5, data.indexOf(">end>")) == "stop") {
      Serial.println("Stop");
      steppers->disable();
    } else if (data.substring(5, data.indexOf(">end>")) == "pause") {
      Serial.println("Pause");
      paused = true;
    } else if (data.substring(5, data.indexOf(">end>")) == "resume") {
      Serial.println("Resume");
      paused = false;
    } else {
      Serial.println("Commande inconnue");
    }
  }
}