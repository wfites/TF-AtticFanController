/*
 * Project AtticFanController_v3
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
 *  This program includes finished and tested funcionality for both MANUAL & AUTO modes.
 * Author: Ted Fites
 * 
 * Modifications:
 * 10/29/20   TF Completed FINAL version. Includes final modifications for OLED screens button operation 
 *              messages and source code file documentation.
 * 10/26/20   TF Completed v#3. Includes tested functionality for wrong button press during operation.
 * 10/25/20   TF Completed v#2. Includes tested functionality for controller AUTO mode complimentary
 *              to MANUAL mode. Also trnasferred cricuit from Argon breadboard to small breakout board
 *              (2"x6") for added wiring space & to eliminate short circuits on Argon board.
 * 10/14/20   TF Created program. Includes tested functionality for controller MANUAL mode.
 *
 */
//************************************* HEADER section ************************************
// 1) Declare program header library files
#include "Adafruit_SSD1306.h"  // library to access class object for OLED display 
#include "Adafruit_BME280.h"  // library to access class object BME sensor
#define OLED_RESET D4

// ******************* 2) Declare global CONSTANTS & VARIABLES  ******************************
const bool ON=1, OFF=0;
const float AUTO_MIN_TEMP=79.0, AUTO_MIN_HUM=99.0;  // MIN_TEMP=79
const int ORIENT_PORTRAIT=3;
const int MAN_MODE_BTN_D10=D10; 
const int AUTO_MODE_BTN_D11=D11;
const int FAN_ACTIVATION_D7=D7;
const unsigned long int AUTO_MODE_DELAY=300000; // delay 5 minutes between AUTO mode reactivation
const unsigned long int POWER_OFF_DELAY=10000; // delay 10 seconds following fan power-off
const unsigned long int BUTTON_PRESS_DELAY=1000; // delay main loop execution 1 second to recognize button press

bool btnManualSt, btnAutoSt;  // stores DigitalRead states for manual and auto button presses
bool autoFanStatus=OFF;  // indicates if fan is currently turned ON or OFF
bool manMode=OFF, autoMode=OFF; // indicates which controller operation mode is currently in effect
bool status;  // checks if BME280 was activated succesfully
float temperature, humidity;  // stores data from BME sensor
String ErrMsgManOLED = "OLED DISPLAY SWITCH CASE ERROR--M A N U A L  MODE";
String ErrMsgAutoOLED = "OLED DISPLAY SWITCH CASE ERROR--A U T O MODE";
String OpsMsgAutoMode = "OPEN WIDE ATTIC DOOR& WINDOWS";
String OpsMsgFrame = "*********";
String WrongButtonMsgFrame = "_________";
unsigned long int startMillis, currMillis, totDelay;
enum modeType {YELLOW_FOR_MANUAL,RED_FOR_AUTO}; 
  modeType en_ModeType;

// ******************* end CONSTANTS & VARIABLES  ******************************

// ******************* 3) Declare library class objects ************************
Adafruit_SSD1306 display(OLED_RESET); // object for OLED display methods 
Adafruit_BME280 bme;  // object for BME sensor's variables
//********************* end  HEADER section ************************************

SYSTEM_MODE(SEMI_AUTOMATIC); // Allows project flashing while Argon is disconnected from any wifi internet
void setup() 
{
  Serial.begin(9600);

// 1) Initialize the I2C address 0x3C to activate the OLED display (128x64 pixels)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.display(); // show Adafruit splashscreen to OLED
  delay(1000);  // maintain display for 1 second
  Serial.printf("PROGRAM AtticFanController_v3\n");

 /*
  * 2) BME 280sensor : Execute .begin method to activate sensor 
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
/*****************************************************************************************
 *************************************  M A I N  L O O P  ********************************
 *****************************************************************************************/
void loop() 
{
  btnManualSt=digitalRead(MAN_MODE_BTN_D10);  // YELLOW button pressed
  btnAutoSt=digitalRead(AUTO_MODE_BTN_D11);  // RED button pressed
//  Serial.printf("btnManualSt > %i\n",btnManualSt);
  if (btnManualSt)
  {
    if (autoMode == ON)  // Check for RED button pressed out of turn (AUTO mode) when already in manual mode
    {  
      autoMode=OFF;
      en_ModeType=RED_FOR_AUTO;
      ML05_WRONG_BUTTON_DISPLAY_SCREEN();
      goto MainLoopBottom;
    }  
    else
    {
      manMode = !manMode;
      ML01_MANUAL_OPS_SCREEN(manMode);  
    }

    if (manMode == OFF)
    {
//      Serial.printf("manMode O F F > %i\n",manMode);
      digitalWrite(FAN_ACTIVATION_D7,LOW);
    }
  }  // end if (btnManualSt)

  if (manMode == ON)
  {
//    Serial.printf("manMode O N > %i\n",manMode);
    digitalWrite(FAN_ACTIVATION_D7,HIGH);
  }
  
  if (btnAutoSt)
  {
    if (manMode == ON)  // Check for RED button pressed out of turn (AUTO mode) when already in manual mode
    {
      manMode=OFF;
      en_ModeType=YELLOW_FOR_MANUAL;
      ML05_WRONG_BUTTON_DISPLAY_SCREEN();
      goto MainLoopBottom;
    }      
    else
    {
      autoMode = !autoMode;
      ML02_AUTO_OPS_SCREEN(autoMode);
      Serial.printf("AUTOMode  > %i\n",autoMode);
    }

    if (autoMode == OFF)
    {
//      ML02_AUTO_OPS_SCREEN(autoMode);
      if (autoFanStatus == ON)
      {
        Serial.printf("MAIN LOOP:AUTOMode is O F F > %i\n",autoMode);
        autoFanStatus = OFF;
        digitalWrite(FAN_ACTIVATION_D7,LOW);
        ML04_TIME_DELAY(POWER_OFF_DELAY);     
      }
    }
  }  // end if (btnAutoSt)

  if (autoMode == ON)
  {
    Serial.printf("AUTOMode is O N > %i execute ML03_CHECK_AUTO_THRESHOLDS()\n",autoMode);
    ML03_CHECK_AUTO_THRESHOLDS();
    ML02_AUTO_OPS_SCREEN(autoMode);
  }
MainLoopBottom:
  ML04_TIME_DELAY(BUTTON_PRESS_DELAY);     
}  //****************************    end  M A I N  L O O P  **********************************

void ML01_MANUAL_OPS_SCREEN(bool inManMode)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setRotation(ORIENT_PORTRAIT);
  display.printf("<BUTTON>\n");
  display.printf("      MODE\n");
  display.printf("<YELLOW>\n");
  switch (inManMode)
  {
    case OFF:
      display.setTextColor(WHITE);
      display.printf("      ");
      display.setTextColor(BLACK,WHITE);
      display.printf("OFF\n");
      display.setTextColor(WHITE);
      display.printf("    MANUAL\n");
      display.printf("<RED> AUTO\n");
      break;
    case ON:
      display.setTextColor(WHITE);
      display.printf("      OFF\n");
      display.printf("    ");
      display.setTextColor(BLACK,WHITE);
      display.printf("MANUAL\n");
      display.setTextColor(WHITE);
      display.printf("<RED> AUTO\n");
      break;
    default:
      display.println(ErrMsgManOLED);
      break;
  }
  display.display();
  return;
}

void ML02_AUTO_OPS_SCREEN(bool inAutoMode)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setRotation(ORIENT_PORTRAIT);
  display.println(OpsMsgFrame);
  display.printf("  MANUAL \n");     
  display.println(OpsMsgFrame);
  display.printf("<BUTTON>\n");
  display.printf("      MODE\n");
  switch (inAutoMode)
  {
    case OFF:
      display.printf("<RED>");
      display.setTextColor(BLACK,WHITE);
      display.printf(" OFF\n");
      display.setTextColor(WHITE);
      display.printf("<YELLOW>\n");     
      ML02_2_AUTO_OPS_SCREEN();
      break;
    case ON:
      display.setTextColor(BLACK,WHITE);
      display.printf("<RED>");
      display.setTextColor(WHITE);
      display.printf(" OFF\n");
      display.printf("<YELLOW>\n");     
      ML02_2_AUTO_OPS_SCREEN();
      break;
    default:
      display.println(ErrMsgAutoOLED);
      break;
  }      
  display.display();
  return;
}

void ML02_2_AUTO_OPS_SCREEN()
{
  display.setTextColor(WHITE);
  display.println();
  display.printf(" TEMP HUM\n");
  display.printf(" SETTINGS\n");
  display.printf(" %2.0f   %2.0f\n",AUTO_MIN_TEMP,AUTO_MIN_HUM);
  display.printf(" SENSOR\n");
  display.setTextColor(BLACK,WHITE);
  display.printf(" %2.0f",temperature);
  display.setTextColor(WHITE);
  display.printf("   ");
  display.setTextColor(BLACK,WHITE);
  display.printf("%2.0f\n",humidity);
  display.setTextColor(WHITE);
  display.println(OpsMsgFrame);
  display.println(OpsMsgAutoMode);
  display.println(OpsMsgFrame);
  return;
}
void ML03_CHECK_AUTO_THRESHOLDS()
{
  temperature=bme.readTemperature() * (9.0/5.0) + 32.0;
  humidity=bme.readHumidity();
  Serial.printf("AUTOMode TEMPERATURE > %2.0f\n",temperature);
  Serial.printf("AUTOMode HUMIDITY > %2.0f\n",humidity);
  if ((temperature >= AUTO_MIN_TEMP) || (humidity >= AUTO_MIN_HUM))
  {
    autoFanStatus=ON;
    digitalWrite(FAN_ACTIVATION_D7,HIGH);
  }
  else
  {
    autoFanStatus=OFF;
    digitalWrite(FAN_ACTIVATION_D7,LOW);
  }
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

void ML05_WRONG_BUTTON_DISPLAY_SCREEN()
{
  char strBtnYellow[8] = " YELLOW";
  char strBtnRed[8] =    "  RED  ";
  char strOpModeMan[8] = " MANUAL";
  char strOpModeAuto[8] ="  AUTO ";

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setRotation(ORIENT_PORTRAIT);
  display.println(WrongButtonMsgFrame);
  display.println("| PRESS |");
  display.printf("|");
  display.setTextColor(BLACK,WHITE);
  switch (en_ModeType)
  {
    case YELLOW_FOR_MANUAL: 
      display.printf("%s",strBtnYellow);
      break;
    case RED_FOR_AUTO:  
      display.printf("%s",strBtnRed);
      break;
  }
  display.setTextColor(WHITE);
  display.println("|");
  display.println("| BUTTON|");
  display.println("|   TO  |");
  display.println("| RETURN|");
  display.println("|   TO  |");
  display.printf("|");
  display.setTextColor(BLACK,WHITE);
  switch (en_ModeType)
  {
    case YELLOW_FOR_MANUAL: 
      display.printf("%s",strOpModeMan);
      break;
    case RED_FOR_AUTO:  
      display.printf("%s",strOpModeAuto);
      break;
  }
  display.setTextColor(WHITE);
  display.println("|");
  display.println("|  MODE |");
  display.println(WrongButtonMsgFrame);
  display.display();
  return;
}
