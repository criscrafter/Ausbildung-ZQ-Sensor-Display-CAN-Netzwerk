#include <SPI.h>
#include <mcp2515.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_I2C_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

unsigned long lastDisplayUpdate = millis();


MCP2515 mcp2515(8);

Adafruit_SSD1306 OLED(128, 64, &Wire, OLED_RESET);


struct can_frame canMsg;


float Motor1Temp = 0;
float Motor2Temp = 0;
float ZinnbadTemp = 0;


void setup() { 

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
  OLED.println(F("CAN Data Display"));
  OLED.println("- Friedrich Rosenthal");
  OLED.display();

  delay(1000);
}

void loop() {

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {

    if (canMsg.can_id == 0x000 && canMsg.can_dlc >= 2) {
      int16_t scaledEngOil = (canMsg.data[0] << 8) | canMsg.data[1];
      engOilTemp = (scaledEngOil / 100.0) - 60.0;
    }

    if (canMsg.can_id == 0x002 && canMsg.can_dlc >= 2) {
      int16_t scaledPressure = (canMsg.data[0] << 8) | canMsg.data[1];
      engOilPressure = (scaledPressure / 1000.0);
    }

  }

  if((millis() - lastDisplayUpdate) > 100){
    lastDisplayUpdate = millis();
    OLED.clearDisplay();

    OLED.setCursor(0,0);
    OLED.print("Wassertemp:");
    OLED.print(engCoolantTemp, 2);
    OLED.print("C");

    OLED.setCursor(0,10);
    OLED.print("Motoroel.:");
    OLED.print(engOilTemp, 2);
    OLED.print("C");

    OLED.setCursor(0,20);
    OLED.print("Transoel.:");
    OLED.print(transOilTemp, 2);
    OLED.print("C");

    OLED.setCursor(0,30);
    OLED.print("Oeldruck:");
    OLED.print(engOilPressure, 3);
    OLED.print("Bar");

    OLED.setCursor(0,40);
    OLED.print("Luefter PWM:");
    OLED.print(fanPWM);    

    OLED.display();
  }
}
