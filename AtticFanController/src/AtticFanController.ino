/*
 * Project Attic_Fan_Controller 
 * Description:
 *  This is a special project initiated following graduation from the IoT cohort 2
 * boot camp on Sep 11, 2020. This project focuses on repurposing the house utility
 * floor fan for its new use a whole house attic fan. This is a Utilitech Pro 20" 
 * high velocity floor fan, has 3 speeds, and has an air movement of 4500 CFM. 
 *  The project's first step involved constructing a compartment to house the fan inside the
 * existing swamp cooler air duct chamber. Construction started in late May, and finished in late 
 * September. The next step was fan testing to confirm that the house could be comfortably cooled 
 * down, strarting in the evening and extending overnight. This testing period started on Sep 24,
 * finishing on Sep 27.
 * Author: Ted Fites
 * Modifications:
 * 10/14/20  TF  Created program
 *
 */
// HEADER section *************************************************************************
#include "Adafruit_SSD1306.h"  // library to access class object for OLED display 
#include "Adafruit_BME280.h"  // library to access class object BME sensor
#define OLED_RESET D4

// *******************Declare CONSTANTS & VARIABLES  ******************************
const bool ON=1, OFF=0;
const float MIN_TEMP=79.0, MIN_HUM=99.0;
const int ORIENT_PORTRAIT=3;
const int MAN_MODE_BTN_D10=D10; // d10
const int AUTO_MODE_BTN_D11=D11;
const int FAN_ACTIVATION_D7=D7;
const unsigned long int AUTO_MODE_PRD=300000, POWER_OFF_DELAY=10000;

bool btnManualSt, btnAutoSt;  // stores DigitalRead state
bool autoFanStatus=OFF;  // indicates if fan is turned ON or OFF
bool manMode=OFF, autoMode=OFF; // indicates which operation mode is currently in effect
bool status;  // checks if BME280 was activated succesfully
float temperature, humidity;  // stores data from BME sensor
String ErrMsgManOLED = "OLED DISPLAY SWITCH ERROR--M A N U A L  MODE";
String ErrMsgAutoOLED = "OLED DISPLAY SWITCH ERROR--A U T O MODE";
String OpsMsgAutoMode = "**OPEN ACCESS DOOR & WINDOWS**";
unsigned long int startMillis, currMillis, totDelay;
// ******************* end CONSTANTS & VARIABLES  ******************************

// ******************* Declare library class objects ***************************
Adafruit_SSD1306 display(OLED_RESET); // object for OLED display methods 
Adafruit_BME280 bme;  // object for BME sensor's variables

SYSTEM_MODE(SEMI_AUTOMATIC); // Allows project flashing while Argon is disconnected from any wifi internet
void setup() 
{
  Serial.begin(9600);

// 1) Initialize the I2C addr 0x3C (for the 128x64) for the OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.display(); // show Adafruit splashscreen to OLED
  delay(1000);  // maintain display for 1 second
  
  Serial.printf("PROGRAM Attic_Fan_Controller\n");

 /*
  * 2) BME 280sensor : BME 280 .begin method to activate sensor 
  */
  
    // while(!Serial);    // time to get serial running
    delay (1000);
    Serial.println(F("BME280 test"));   // "F" in println statement: Reference to FLASH memory on SD card)
    status = bme.begin(0x76);  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),HEX);
        Serial.println("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085");
        Serial.println("   ID of 0x56-0x58 represents a BMP 280,");
        Serial.println("        ID of 0x60 represents a BME 280.");
        Serial.println("        ID of 0x61 represents a BME 680.");
        while (1);
    }
    else {
      Serial.println("BME280 Up and Running");
    }

// *************************  DECLARE PROGRAM PIN MODES ************************************
pinMode(MAN_MODE_BTN_D10,INPUT_PULLDOWN); // YELLOW button: Manual fan control 
pinMode(AUTO_MODE_BTN_D11,INPUT_PULLDOWN);  // RED button: Automatic fan control 
pinMode(FAN_ACTIVATION_D7,OUTPUT);  // Relay switch pin providing power for fan

// Display initial OLED screen in MANUAL operation display mode (with switch position OFF)
ML01_MANUAL_OPS_SCREEN(OFF);  
}

void loop() 
{
  btnManualSt=digitalRead(MAN_MODE_BTN_D10);  // YELLOW button pressed
//  btnAutoSt=digitalRead(AUTO_MODE_BTN_D11);  // RED button pressed
  Serial.printf("btnManualSt > %i\n",btnManualSt);
  if (btnManualSt)
  {
    manMode = !manMode;
    ML01_MANUAL_OPS_SCREEN(manMode);  
    if (manMode == OFF)
      {
    Serial.printf("manMode O F F > %i\n",manMode);
        digitalWrite(FAN_ACTIVATION_D7,LOW);
      }
  }
  btnManualSt=OFF;
  if (manMode == ON)
    {
        Serial.printf("manMode O N > %i\n",manMode);
        digitalWrite(FAN_ACTIVATION_D7,HIGH);
    }
  delay(1000);
}

void ML01_MANUAL_OPS_SCREEN(bool inManMode)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setRotation(ORIENT_PORTRAIT);
  display.printf("BUTTON\n");
  display.printf("      MODE\n");
  display.printf("YELLOW\n");
  switch (inManMode)
  {
    case OFF:
      display.setTextColor(WHITE);
      display.printf("      ");
      display.setTextColor(BLACK,WHITE);
      display.printf("OFF\n");
      display.setTextColor(WHITE);
      display.printf("    MANUAL\n");
      display.printf("RED   AUTO\n");
      break;
    case ON:
      display.setTextColor(WHITE);
      display.printf("      OFF\n");
      display.printf("    ");
      display.setTextColor(BLACK,WHITE);
      display.printf("MANUAL\n");
      display.setTextColor(WHITE);
      display.printf("RED   AUTO\n");
      break;
    default:
      display.println(ErrMsgManOLED);
      break;
  }
  display.display();
  return;
}

void ML04_TIME_DELAY(unsigned long int inDelay)
{
  startMillis=millis();
  currMillis=0;
  totDelay=startMillis + inDelay;
  while(currMillis < totDelay)
  {
    currMillis=millis();
  }
  return;
}