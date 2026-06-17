#include <SPI.h>
#include <mcp2515.h>
#include <Adafruit_MAX31865.h>


MCP2515 mcp2515(17);                                                                           //CAN Controller CS Pin festlegen
Adafruit_MAX31865 Motor1 = Adafruit_MAX31865(5);                                              //Motor 1 Sensor CS Pin festlegen
Adafruit_MAX31865 Motor2 = Adafruit_MAX31865(6);                                             //Motor 2 Sensor CS Pin festlegen
Adafruit_MAX31865 Zinnbad = Adafruit_MAX31865(7);                                            //Zinnbad Sensor CS Pin festlegen

struct can_frame canMsg1;                                                                     //CAN Nachrichten Frame Erstellen
struct can_frame canMsg2;                                                                     //CAN Nachrichten Frame Erstellen

float Motor1Temp;                                                                         //Variable für Motor 1 Temperatur
float Motor2Temp;                                                                         //Variable für Motor 2 Temperatur
float ZinnbadTemp;                                                                        //Variable für Zinnbad Temperatur

unsigned long lastSentMessage = millis();

#define RREF_Motor1      425.0                                                                //Nominalwert vom Referenzwiderstand Motor 1  (430 bei PT100, 4300 bei PT1000)
#define RREF_Motor2      425.0                                                                //Nominalwert vom Referenzwiderstand Motor 2  (430 bei PT100, 4300 bei PT1000)
#define RREF_Zinnbad     425.0                                                                //Nominalwert vom Referenzwiderstand Zinnbad  (430 bei PT100, 4300 bei PT1000)

#define RNOMINAL_Motor1   100                                                                 //Nominalwert Sensor Motor 1  (100 bei PT100, 1000 bei PT1000)
#define RNOMINAL_Motor2   100                                                                 //Nominalwert Sensor Motor 2  (100 bei PT100, 1000 bei PT1000)
#define RNOMINAL_Zinnbad  100                                                                 //Nominalwert Sensor Zinnbad  (100 bei PT100, 1000 bei PT1000)


bool TempErrorCheck(Adafruit_MAX31865 &sensor, const char* name) {
  uint8_t fault = sensor.readFault();
  if (fault) {
    Serial.print(name); Serial.print(" Fehler 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH)  Serial.println("RTD High Threshold");
    if (fault & MAX31865_FAULT_LOWTHRESH)   Serial.println("RTD Low Threshold");
    if (fault & MAX31865_FAULT_REFINLOW)    Serial.println("REFIN- > 0.85 x Bias");
    if (fault & MAX31865_FAULT_REFINHIGH)   Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    if (fault & MAX31865_FAULT_RTDINLOW)    Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    if (fault & MAX31865_FAULT_OVUV)        Serial.println("Under/Over voltage");
    sensor.clearFault();
    return true;  // Fehler vorhanden
  }
  return false;
}

void setup() {

  Serial.begin(921600);

  Motor1.begin(MAX31865_3WIRE);
  Motor2.begin(MAX31865_3WIRE);
  Zinnbad.begin(MAX31865_3WIRE);

  //CAN Controller initiieren
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  //CAN Nachrichten Struktur festlegen
  canMsg1.can_id  = 0x101;
  canMsg1.can_dlc = 4;
  
  canMsg2.can_id  = 0x102;
  canMsg2.can_dlc = 2;
}


void loop() {

  if((millis() - lastSentMessage) > 245){
    lastSentMessage = millis();

    Motor1Temp = Motor1.temperature(RNOMINAL_Motor1, RREF_Motor1);
    Motor2Temp = Motor2.temperature(RNOMINAL_Motor2, RREF_Motor2);
    ZinnbadTemp = Zinnbad.temperature(RNOMINAL_Zinnbad, RREF_Zinnbad);

    uint16_t ScaledMotor1Temp;
    if(TempErrorCheck(Motor1, "Motor 1")) {
        ScaledMotor1Temp = 0;
    } else {
        ScaledMotor1Temp = (uint16_t)((Motor1Temp + 200.0) * 100.0);
    }

    uint16_t ScaledMotor2Temp;
    if(TempErrorCheck(Motor2, "Motor 2")) {
        ScaledMotor2Temp = 0;
    } else {
        ScaledMotor2Temp = (uint16_t)((Motor2Temp + 200.0) * 100.0);
    }
    
    uint16_t ScaledZinnbadTemp;
    if(TempErrorCheck(Zinnbad, "Zinnbad")) {
        ScaledZinnbadTemp = 0;
    } else {
        ScaledZinnbadTemp = (uint16_t)((ZinnbadTemp + 200.0) * 100.0);
    }


    canMsg1.data[0] = (ScaledMotor1Temp >> 8) & 0xFF;  // High byte
    canMsg1.data[1] = ScaledMotor1Temp & 0xFF;         // Low byte

    canMsg1.data[2] = (ScaledMotor2Temp >> 8) & 0xFF;  // High byte
    canMsg1.data[3] = ScaledMotor2Temp & 0xFF;         // Low byte
    mcp2515.sendMessage(&canMsg1);
    
    canMsg2.data[0] = (ScaledZinnbadTemp >> 8) & 0xFF;  // High byte
    canMsg2.data[1] = ScaledZinnbadTemp & 0xFF;         // Low byte
    mcp2515.sendMessage(&canMsg2);

  }

}
