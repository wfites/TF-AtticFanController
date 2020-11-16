/*
 * Project AtticFanController_FINAL
 * Description:
 *  This is a post-IoT Bootcamp project following the Cohort 2 graduation on Sep 11, 2020.
 *  This project repurposes a hi-power 3-speed floor fan for its new role as a whole house attic 
 * fan. Power for the fan is provided by constructing an active-mode PNP circuit connected to a 
 * relay switch, powered by an Argon Miccrocontroller. The fan circulates air throughout the house,
 * concentrating into a custom-built ceiling compartment that houses the fan, which fans air out 
 * into the attic space, and then exited via open garage windows.
 * 	The Argon is programmed with manual and automatic operating modes for fan control. Manual 
 * mode operates just like an on-off switch. Automatic mode continuously checks for interior 
 * temperature or humidity exceeding their programmed thresholds beforing powering on the fan. 
 * Separate OLED screens show programmed menu options to operate buttons for selecting the active 
 * operation mode.
 *  	The controller, active-mode PNP circuit and relay switch components can be adapted for 
 * controlling a variety of household A/C appliances, such as ceiling fans and electric heaters, 
 * whenever the relay switch circuit can be spliced into the device's hot wire of the power cord. 

 *
 * Author: Ted Fites
 * 
 * Modifications:
 * 10/29/20   TF Completed FINAL version. Includes final modifications for OLED screens button 
 *              operation screen messages and up-to-date source code file documentation.
 * 10/26/20   TF Completed v#3. Includes tested functionality for wrong button press during operation.
 * 10/25/20   TF Completed v#2. Includes tested functionality for controller AUTO mode complimentary
 *              to MANUAL mode. Also transferred circuit from Argon breadboard to small breakout board
 *              (2"x6") for added wiring space & to eliminate short circuits on Argon board.
 * 10/14/20   TF Created program. Includes tested intial functionality for controller MANUAL mode.
 *
 */
//************************************* HEADER section ***************************************
//********************* 1) Declare program header library files ******************************
#include "Adafruit_SSD1306.h"  // library to access class object for OLED display 
#include "Adafruit_BME280.h"  // library to access class object BME sensor
#define OLED_RESET D4

// ******************* 2) Declare global CONSTANTS & VARIABLES  ******************************
const bool ON=1, OFF=0;  // button switch variables clarifying main loop conditional checks logic
const float AUTO_MIN_TEMP=79.0, AUTO_MIN_HUM=99.0;  // initial AUTO mode threshold temperature/humidity values
const int ORIENT_PORTRAIT=3; // sets OLED button operation screen orientation to portrait
const int MAN_MODE_BTN_D10=D10;  // digital input pin# for YELLOW button (MANUAL mode) 
const int AUTO_MODE_BTN_D11=D11;  // digital input pin# for RED button (auto mode) 
const int FAN_ACTIVATION_D7=D7;  // digital output pin# supplying DC to activate (close) relay switch circuit
const unsigned long int POWER_OFF_DELAY=10000; // delay program 10 seconds following fan power-off
const unsigned long int BUTTON_PRESS_DELAY=1000; // delay main loop execution 1 second to recognize button press

bool btnManualSt, btnAutoSt;  // stores DigitalRead states from manual and auto button presses
bool autoFanStatus=OFF;  // intial setting used in AUTO mode checking if fan currently turned ON or OFF
bool manMode=OFF, autoMode=OFF; // initial settings checking which controller operation mode is currently in effect
bool status;  // activation status check for BME280 sensor
float temperature, humidity;  // variables storing data from BME analog sensor reads
// Next 2 variables display "catch-all" console error messages used during testing button operation (should never see)
String ErrMsgManOLED = "OLED DISPLAY SWITCH CASE ERROR--M A N U A L  MODE";
String ErrMsgAutoOLED = "OLED DISPLAY SWITCH CASE ERROR--A U T O MODE";
// Next 3 variables display on OLED screen during button operation
String OpsMsgAutoMode = "OPEN WIDE ATTIC DOOR& WINDOWS"; // fan prep reminder message for AUTO button press
String OpsMsgFrame = "!!!!!!!!!!";
String WrongButtonMsgFrame = "__________";
unsigned long int startMillis, currMillis, totDelay;  // for calculating program execution delays
enum SwPressErr {WRONG_YELLOW_PRESS,WRONG_RED_PRESS};  // User data type clarifying logic for invalid mode switch press
  SwPressErr en_SwPressErr;

// ******************* end CONSTANTS & VARIABLES  ******************************
// ******************* 3) Declare library class objects ************************
Adafruit_SSD1306 display(OLED_RESET); // object for OLED display methods 
Adafruit_BME280 bme;  // object for BME sensor's variables
//********************* end  HEADER section ************************************

SYSTEM_MODE(SEMI_AUTOMATIC); // Argon mode allows project flashing while disconnected from any wifi internet
void setup()  //***********************  S E T U P *****************************
{
  Serial.begin(9600);

// 1) Initialize the I2C address to 0x3C to activate the OLED display (128x64 pixels screen)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.display(); // show Adafruit splashscreen to OLED
  delay(1000);  // maintain display for 1 second
  Serial.printf("PROGRAM AtticFanController_FINAL\n");

 // 2) BME 280sensor : Execute .begin method to activate sensor  
  delay (1000);
  Serial.println(F("BME280 test"));   
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

// 3) Declare program PIN MODES 
pinMode(MAN_MODE_BTN_D10,INPUT_PULLDOWN); // YELLOW button pin: Manual fan control 
pinMode(AUTO_MODE_BTN_D11,INPUT_PULLDOWN);  // RED button pin: Automatic fan control 
pinMode(FAN_ACTIVATION_D7,OUTPUT);  // Digital pin to activate Relay switch to power attic fan

// 4) Display initial OLED screen in MANUAL operation display mode (manual switch set OFF)
ML01_MANUAL_OPS_SCREEN(OFF);  
}  // end SETUP

/*****************************************************************************************
 *************************************  M A I N  L O O P  ********************************
 *****************************************************************************************
 * B U T T O N  P R O C E S S I N G  L O G I C  O V E R V I E W ********************************
 * When pressing either button, YELLOW (for manual mode), or RED (for auto mode) use this same 
 * basic processing logic:
 * 1) Display the active OLED button operation screen for the operation mode the user selects. 
 * When the program starts, the MANUAL operation mode OLED screen displays.The active switch 
 * setting appears highlighted on the OLED display. For manual mode, switch settings are labeled 
 * "<MANUAL>" (for ON) and "<OFF>". For auto mode, switch settings are labeled "<RED>" (for ON) 
 * and "<OFF>". Also for auto mode, current temperature and humidity alues are highlighted on the 
 * display.
 * 2) Check for a YELLOW or RED button press (for manual or auto mode respectively). The initial
 * button press switches ON the selected operation mode.
 * 3) Check for a mistaken button SWITCH press. This occurs when the user has already switched ON
 * (activated) a selected operation mode (manual or auto). Instead of switching OFF the same 
 * selected mode by correctly repressing the same button, the user presses the opposite button. 
 * When this occurs, display an OLED message to press the correct button, then return the user 
 * back to the original screen and original switch setting (before the mistaken button press). 
 * 4) Upon correct button press, set the active mode's switch opposite to its current setting, in 
 * effect activating the switch (ON to OFF, OFF to ON).
 * 5) Redisplay the OLED button operation screen to display the highlighted new switch setting 
 * (changed in the previous step).
 * 6) Check for active mode as switched OFF. If it is switched OFF, then deactivate 
 * current flow sent through the digital output pin D7 (via digitalWrite set to LOW/NO voltage). 
 * In effect, this deactivates current flow to the relay switch, which opens the relay switch 
 * circuit to the spliced extension cord hot (black) wire, thereby shutting off power to the attic 
 * fan. This deactivation occurs and lasts only at the instant of the button press.
 * 7) Check for active mode as switched ON. If it is switched ON, then activate  
 * current flow sent through the digital output pin D7 (via digitalWrite set to HIGH 
 * voltage). In effect, after the button is pressed to turn the switch ON, this maintains activation 
 * of current flow to the relay switch, which keeps the relay switch circuit closed to the spliced 
 * extension cord hot (black) wire, thereby maintaining power to the attic fan.
 *  This logic sequence is repeated for each button press while the program runs.
 ***************************************************************************************************/ 
void loop() 
{
  btnManualSt=digitalRead(MAN_MODE_BTN_D10);  // Check: YELLOW button press
  btnAutoSt=digitalRead(AUTO_MODE_BTN_D11);  // Check: RED button press

  if (btnManualSt)  // **** M A N U A L  M O D E  SELECTED  **********************************
/************************* Y E L L O W  BUTTON PRESS PROCESSING   ****************************
 * This processing occurs upon the instantaneous YELLOW button press                         *
 * *******************************************************************************************/
  {
    if (autoMode == ON)  
    { // Check for RED button pressed out of turn (AUTO mode) when already in manual mode
      autoMode=OFF; // switch the wrong mode setting (AUTO) to OFF
      en_SwPressErr=WRONG_RED_PRESS;
      ML05_WRONG_BUTTON_DISPLAY_SCREEN(); // display OLED screen explaining error & correction instruction
      goto MainLoopBottom;  // exit to loop bottom, leaving on error screen, waiting for next button press
    }  
    else
    { // For a valid press, switch MANUAL mode opposite to its current setting, then redisplay 
      // the OLED screen showing the new switch setting.
      manMode = !manMode;
      ML01_MANUAL_OPS_SCREEN(manMode);  
    }

    if (manMode == OFF)
    { // Check for MANUAL mode switched OFF. If so, deactivate current flow to the relay switch
//      Serial.printf("manMode O F F > %i\n",manMode);
      digitalWrite(FAN_ACTIVATION_D7,LOW);
    }
  }  
/************************* e n d Y E L L O W  BUTTON PRESS PROCESSING   ********************/

/* Now perform a continuous loop check for MANUAL mode switched ON. If so, activate current flow 
* to the relay switch. Maintain this current flow to the relay while the switch stays ON. 
*/
  if (manMode == ON)
  { 
//    Serial.printf("manMode O N > %i\n",manMode);
    digitalWrite(FAN_ACTIVATION_D7,HIGH);
  }

  if (btnAutoSt)  // ***** A U T O  M O D E  SELECTED  ***************************************
/************************* R E D   BUTTON PRESS PROCESSING   *********************************
 * This processing occurs upon the instantaneous RED button press                            *
 * *******************************************************************************************/
  {
    if (manMode == ON)  
    {  // Check for YELLOW button pressed out of turn (MANUAL mode) when already in auto mode
      manMode=OFF;
      en_SwPressErr=WRONG_YELLOW_PRESS;
      ML05_WRONG_BUTTON_DISPLAY_SCREEN();
      goto MainLoopBottom;
    }      
    else
    {
      autoMode = !autoMode;
      ML02_AUTO_OPS_SCREEN(autoMode);
      Serial.printf("AUTOMode  > %i\n",autoMode);
    }
  /* Check if AUTO mode is switched off. Next, check if the fan is switched ON based  on either 
  * the temperature or humidity reaching their programmed thresholds. If so, then deactivate the 
  * fan, followed by a 10 second fan blade cessation period.
  */
    if (autoMode == OFF)
    {
      if (autoFanStatus == ON)
      {
        Serial.printf("MAIN LOOP:AUTOMode is O F F > %i\n",autoMode);
        autoFanStatus = OFF;
        digitalWrite(FAN_ACTIVATION_D7,LOW);
        ML04_TIME_DELAY(POWER_OFF_DELAY);     
      }
    }
  }  
/************************* e n d  R E D  BUTTON PRESS PROCESSING   ********************/

/* Now perform a continuous loop check for AUTO mode switched ON. While the switch is ON,
* perform a continuous, automatic check on the temperature and humidity thresholds to see
* if either threshold will trigger on the relay switch. Following the treshold checks, 
* redisplay the auto OLED screen to display continuously updated changes in temp or humidity.
*/
  if (autoMode == ON)
  {
    Serial.printf("AUTOMode is O N > %i execute ML03_CHECK_AUTO_THRESHOLDS()\n",autoMode);
    ML03_CHECK_AUTO_THRESHOLDS();
    ML02_AUTO_OPS_SCREEN(autoMode);
  }
MainLoopBottom:
  ML04_TIME_DELAY(BUTTON_PRESS_DELAY);     
}  
/****************************    end  M A I N  L O O P  ************************************
 *******************************************************************************************/ 

/********************   OLED DISPLAY: M A N U A L   O P  M O D E  **************************/
void ML01_MANUAL_OPS_SCREEN(bool inManMode)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setRotation(ORIENT_PORTRAIT);
  display.print("* MANUAL *");
  display.println();
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

/********************   OLED DISPLAY--UPPER SCREEN: A U T O  O P  M O D E  ************/
void ML02_AUTO_OPS_SCREEN(bool inAutoMode)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setRotation(ORIENT_PORTRAIT);
  display.print("*  AUTO  *");
  display.printf("<BUTTON>\n");
  display.printf("      MODE\n");
/* BUTTON OPERATION INSTRUCTIONS "STYLE" TO HIGHLIGHT ACTIVE AUTO MODE SETTING:
* Use .setTextColor "inverted" text settings to highlight either the target mode button ("RED"), 
* indicating the active operation mode (and implying it's also switched ON), OR the auto 
* mode's deactivate switch setting ("OFF"). This mode selection style differs from both highlighted 
* manual operation mode selection settings, demonstrating a hyper-concise operation selection 
* style adaptable to very small display screens.
*/
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

/********************   OLED DISPLAY--LOWER SCREEN: A U T O  O P  M O D E  ************
* This function is called from its parent function (ML02_AUTO_OPS_SCREEN()) to finish displaying
* information to the auto mode display screen, and called from two locations, specific to which 
* is the active auto switch setting in effect. 
*/
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
  display.print(OpsMsgFrame);
  display.println(OpsMsgAutoMode);
//  display.println(OpsMsgFrame);
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

/********************   OLED DISPLAY:  W R O N G   B U T T O N   P R E S S   ************
* This display screen appears whenever the user doesn't use the selected button mode (manual or
* auto) as a proper toggle switch; that is, to press tha same button once to switch ON the operation
* mode, followed by a second press to switch OFF the mode. 
*/
void ML05_WRONG_BUTTON_DISPLAY_SCREEN()
{
  char strBtnYellow[10] = " YELLOW ";
  char strBtnRed[10] =    "  RED   ";
  char strOpModeMan[10] = " MANUAL ";
  char strOpModeAuto[10] ="  AUTO  ";

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setRotation(ORIENT_PORTRAIT);
  display.print(WrongButtonMsgFrame);
  display.print("| PRESS  |");
  display.printf("|");
  display.setTextColor(BLACK,WHITE);
  switch (en_SwPressErr)
  {
    case WRONG_YELLOW_PRESS: 
      display.printf("%s",strBtnYellow);
      break;
    case WRONG_RED_PRESS:  
      display.printf("%s",strBtnRed);
      break;
  }
  display.setTextColor(WHITE);
  display.print("|");
  display.print("| BUTTON |");
  display.print("|   TO   |");
  display.print("| RETURN |");
  display.print("|   TO   |");
  display.print("|");
  display.setTextColor(BLACK,WHITE);
  switch (en_SwPressErr)
  {
    case WRONG_YELLOW_PRESS: 
      display.printf("%s",strOpModeMan);
      break;
    case WRONG_RED_PRESS:  
      display.printf("%s",strOpModeAuto);
      break;
  }
  display.setTextColor(WHITE);
  display.print("|");
  display.print("|  MODE  |");
  display.print(WrongButtonMsgFrame);
  display.display();
  return;
}