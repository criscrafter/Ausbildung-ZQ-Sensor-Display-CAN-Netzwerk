#include <Arduino.h>
#include <SPI.h>
#include <mcp2515.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_I2C_ADDRESS 0x3C ///< See datasheet for Address;

unsigned long lastDisplayUpdate = 0;
unsigned long lastReceivedMessage = 0;

MCP2515 mcp2515(17);

Adafruit_SSD1306 OLED(128, 64, &Wire, OLED_RESET);

struct can_frame canMsg;

float Motor1Temp = 0;
float Motor2Temp = 0;
float ZinnbadTemp = 0;

 
void setup() { 

  Serial.begin(921600);

  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  for(uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if(Wire.endTransmission() == 0)
        Serial.printf("I2C device at 0x%02X\n", addr);
  }

  if(!OLED.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }


  //CAN Controller initiieren
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  OLED.clearDisplay();
  OLED.setTextSize(1);                         // Schriftgröße auf 1 setzen
  OLED.setTextColor(SSD1306_WHITE);            // Schriftfarbe auf Weiß setzen
  OLED.setCursor(0,0);                         // Cursor zu Positiion 0,0 bewegen
  OLED.println(F("ZQ - Programmierung"));
  OLED.println("Datenübertragung");
  OLED.println("über CAN Bus");
  OLED.println("- Friedrich Rosenthal");
  OLED.display();

  delay(3000);
}

void loop() {

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {

    if (canMsg.can_id == 0x101 && canMsg.can_dlc >= 4) {
      uint16_t ScaledMotor1Temp = (canMsg.data[0] << 8) | canMsg.data[1];
      Motor1Temp = (ScaledMotor1Temp / 100.0) - 200.0;

      uint16_t ScaledMotor2Temp = (canMsg.data[2] << 8) | canMsg.data[3];
      Motor2Temp = (ScaledMotor2Temp / 100.0) - 200.0;
    }

    if (canMsg.can_id == 0x102 && canMsg.can_dlc >= 2) {
      uint16_t ScaledZinnbadTemp = (canMsg.data[0] << 8) | canMsg.data[1];
      ZinnbadTemp = (ScaledZinnbadTemp / 100.0) - 200.0;
    }

    lastReceivedMessage = millis();

  }

  if((millis() - lastDisplayUpdate) > 100){
    lastDisplayUpdate = millis();

    OLED.clearDisplay();

    if(millis() - lastReceivedMessage > 1000) {
      OLED.setCursor(0,0);
      OLED.print("KEIN SIGNAL");
    } else {
    
      OLED.setCursor(0,0);
      OLED.print("Motor 1:");

      if(Motor1Temp == -200){
        OLED.print(" ERROR");
      } else {
        OLED.print(Motor1Temp, 1);
        OLED.print("C");
      }


      OLED.setCursor(0,10);
      OLED.print("Motor 2:");

      if(Motor2Temp == -200){
        OLED.print(" ERROR");
      } else {
        OLED.print(Motor2Temp, 1);
        OLED.print("C");
      }


      OLED.setCursor(0,20);
      OLED.print("Zinnbad:");

      if(ZinnbadTemp == -200){
        OLED.print(" ERROR");
      } else {
        OLED.print(ZinnbadTemp, 1);
        OLED.print("C");
      }

    }

    OLED.display();
  }

}#include <SPI.h>
#include <mcp2515.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_I2C_ADDRESS 0x3C ///< See datasheet for Address;

unsigned long lastDisplayUpdate = 0;
unsigned long lastReceivedMessage = 0;

MCP2515 mcp2515(8);

Adafruit_SSD1306 OLED(128, 64, &Wire, OLED_RESET);

struct can_frame canMsg;

float Motor1Temp = 0;
float Motor2Temp = 0;
float ZinnbadTemp = 0;


void setup() { 

  Serial.begin(921600);

  if(!OLED.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //CAN Controller initiieren
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  OLED.clearDisplay();
  OLED.setTextSize(1);                         // Schriftgröße auf 1 setzen
  OLED.setTextColor(SSD1306_WHITE);            // Schriftfarbe auf Weiß setzen
  OLED.setCursor(0,0);                         // Cursor zu Positiion 0,0 bewegen
  OLED.println(F("ZQ - Programmierung"));
  OLED.println("Datenübertragung");
  OLED.println("über CAN Bus");
  OLED.println("- Friedrich Rosenthal");
  OLED.display();

  delay(1000);
}

void loop() {

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {

    if (canMsg.can_id == 0x101 && canMsg.can_dlc >= 4) {
      uint16_t ScaledMotor1Temp = (canMsg.data[0] << 8) | canMsg.data[1];
      Motor1Temp = (ScaledMotor1Temp / 100.0) - 200.0;

      uint16_t ScaledMotor2Temp = (canMsg.data[2] << 8) | canMsg.data[3];
      Motor2Temp = (ScaledMotor2Temp / 100.0) - 200.0;
    }

    if (canMsg.can_id == 0x102 && canMsg.can_dlc >= 2) {
      uint16_t ScaledZinnbadTemp = (canMsg.data[0] << 8) | canMsg.data[1];
      ZinnbadTemp = (ScaledZinnbadTemp / 100.0) - 200.0;
    }

    lastReceivedMessage = millis();

  }

  if((millis() - lastDisplayUpdate) > 100){
    lastDisplayUpdate = millis();

    OLED.clearDisplay();

    if(millis() - lastReceivedMessage > 1000) {
      OLED.setCursor(0,0);
      OLED.print("KEIN SIGNAL");
    } else {
    
      OLED.setCursor(0,0);
      OLED.print("Motor 1:");

      if(Motor1Temp == -200){
        OLED.print(" ERROR");
      } else {
        OLED.print(Motor1Temp, 1);
        OLED.print("C");
      }


      OLED.setCursor(0,10);
      OLED.print("Motor 2:");

      if(Motor2Temp == -200){
        OLED.print(" ERROR");
      } else {
        OLED.print(Motor2Temp, 1);
        OLED.print("C");
      }


      OLED.setCursor(0,20);
      OLED.print("Zinnbad:");

      if(ZinnbadTemp == -200){
        OLED.print(" ERROR");
      } else {
        OLED.print(ZinnbadTemp, 1);
        OLED.print("C");
      }

    }

    OLED.display();
  }

}
