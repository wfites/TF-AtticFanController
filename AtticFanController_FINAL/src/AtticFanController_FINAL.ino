/*
 * Project AtticFanController_FINAL
 * Description:
 *  This is a post-IoT Bootcamp project following the Cohort 2 graduation on Sep 11, 2020.
 * This project focuses on repurposing the house utility floor fan for its new role as a whole 
 * house attic fan. Specifically, the purpose of the project is to construct a circuit using the 
 * Argon microcontroller to operate a Utilitech Pro 20" high velocity floor fan (model #SFD1-500B) 
 * for the purpose of distributing cool air throughout the entire house for the spring and fall 
 * seasons. The fan has 3 speeds, with a maximum air movement of 4500 CFM. 
 
 * Author: Ted Fites
 * 
 * Modifications:
 * 10/29/20   TF Completed FINAL version. Includes final modifications for OLED screens button 
 *              operation screen messages and up-to-date source code file documentation.
 * 10/26/20   TF Completed v#3. Includes tested functionality for wrong button press during operation.
 * 10/25/20   TF Completed v#2. Includes tested functionality for controller AUTO mode complimentary
 *              to MANUAL mode. Also transferred circuit from Argon breadboard to small breakout board
 *              (2"x6") for added wiring space & to eliminate short circuits on Argon board.
 * 10/14/20   TF Created program. Includes tested functionality for controller MANUAL mode.
 *
 * PROJECT DEVELOPMENT & TIMEFRAME
 *    Development actually began prior to the start of the bootcamp on July 6. The project's first 
 * step involved constructing a plywood compartment to house the fan inside the existing swamp 
 * cooler air duct chamber. Compartment construction started in late May, finishing in late 
 * September, where the fan was successfully installed into the finished compartment.  
 * Having finished the compartment, fan cooling testing began for confirming  that the house could
 * be comfortably cooled down in the evenings, overnight and through morning. Fan testing 
 * extended from Sep 24-27. Results were successful, often reducing the temperature 9-10 degrees 
 * overnight through approx 10-11A next morning, when windows were shut and regular A/C was turned 
 * on.
 *    Next, circuit design and circuit programming began the last week of September, starting 
 * with lab book circuit diagram, finishing with first draft Fritzing circuit diagram, and 
 * followed by initial lab book OLED display layouts. Breadboard circuit wiring began at the end 
 * of September after completing the first draft Fritzing diagram. 
 *    During the first week of October, more hands on construction and testing took place for 
 * building the 15 ft long extension cord proving power from the nearby furnace closet outlet to 
 * the fan compartment, located nearby on the hallway ceiling. During the second week, 
 * first-configuration breadboard circuit was completed, followed by first draft programming logic 
 * revisions. C++ program logic revisions, debugging, testing and documentation continued through
 * weeks three and four (see above, section "Modifications") to ensure the circuit and button 
 * programming worked according to specs. During week three, I created the Github repository 
 * (AT-AtticFanController) to push completed separate versions of the C++ driver program, 
 * AtticFanController.
 *    Once completing a working breadboard circuit, project development was redirected to
 * splicing the extension cord into the tested breadboard circuit. [ TO BE COMPLETED]
 * 
 * ESSENTIAL PROJECT CIRCUIT COMPONENTS
 *  These are the components for the programmed circuit:
 * 1) Particle Argon Microcontroller
 *    This is the processing unit controlling all its connected devices listed below that
 * construct the project circuit. The 32-bit main processor runs at 64 MHz, offering
 * WI-FI, Bluetooth & cellular network connections. Has its own OS, is used world-wide, and allows 
 * easy prototype-to-production source code options. Also has an expansive pin configuration for 
 * communicating with other devices, including up to 20 digital pins, 6 analog-to-digital 
 * conversion pins, and up to 13 analog pins. It's powered on 3.3 volts, with optional 5 volt 
 * capacity (a feature critical to this project).
 * 
 * 2) Yellow & Red buttons
 *  When pressed, these buttons complete a digital input circuit with the Argon to communicate
 * the user's selected operation mode, and when it's currently activated and deactivated.
 
 * 3) SSD 1306 OLED display (Adafruit)
 *    This interface device, complete with its own library of programmable subroutines,  
 * communicates all essential information to users. It features menu-driven screens for selecting 
 * the operating mode (manual or auto), current BME sensor readings for temperature and humidity  
 * (and their programmed thresholds automatically triggering fan operation), and error button 
 * press messages during operation. This display screen, approximately 3/4"x1", uses an I2C serial 
 * communication protocol (being slaved off of the BME sensor). In this project, the display is 
 * programmed in portrait mode with a maximum 16 lines, at 10 characters per line, totaling 160 
 * displayable characters. 
 * 
 * 4) BME 280 sensor (Adafruit)
 *    This tiny device (approx 1/2" square) also features a complete libary of programmable 
 * routines, has sensing capabilities for temperature (in C), humidity and air pressure. For this
 * project, its I2C serial communication data and clock pins are slaved directly from the Argon's
 * corresponding SDA (data) and SCL (clock) pins. It features rapid, reliable sensing capability.
 * 
 * 5) Relay Switch (Songle)
 *    This switch is used to safely control an electrical device, such as the attic fan requiring 
 * higher current or higher voltage, from a mocrocontroller. The switch contains an electro-
 * magnetic control coil, coming in contact with a switch powered from a digital pin on the
 * Argon, that closes the circuit from the Argon. 
 * 
 * 6) PNP Transistor 2N3906
 *    This bipolar junction transistor consists of a single N-type semi-conductor material (having 
 * surplus negative-charged carrier electons) sandwiched in between two P-type semiconductor 
 * material (having surplus positive-charged particles), with leads (legs) attached to each 
 * section.  The middle lead is the base, with outer leads known as collector and emitter. This 
 * transistor has two PN junctions: base-collector, and base-emitter. Transistors often function 
 * as switches for integrated circuits, but our project circuit uses it as a current amplifier to 
 * supply current to the relay switch.
 * 
 * HOW THE PROJECT CIRCUIT WORKS 
 *    Activating the attic fan is largely dependent on the current generated within the input circuit 
 * through the transistor and through the output circuit and on to the relay switch. The circuit's 
 * base-emitter junction is critical for generating the current. In this project circuit, a manual 
 * button press or automated activation sends output current from digital pin (D7). This current 
 * is pushed by a smaller amount of voltage (metered just under 3V) along its circuit to the 
 * transistor's base lead, at which point this very small current then combines with higher 
 * current pushed from by the Argon's VUSB pin(metered at 5V) along the transistor's base-emitter 
 * junction, to the emitter lead, and finally to the IN connector at the relay switch.  
 *    From there, the circuit joins up with the furnace closet extension cord that provides A/C
 * power to the attic fan. The relay switch is wired into the extension cord's hot (black) wire from 
 * the opposite side of the relay switch, into the COM (Common ground) and NO (Normally On)
 * connectors. When the Argon circuit is switched on, current travels from the relay switch and 
 * through these connectors to the two spliced ends of the hot wire, traveling to the fan along 
 * this wire, then exiting through the cord's white wire back to the outlet, to fully complete 
 * the circuit and power the fan. Then a reversal takes place: Current flows through the white 
 * wire into the fan, then exits out through the black wire (via the relay switch connection) as 
 * it exits back to the outlet. This A/C cycle is repeated 60 times per second. 
 *    The project's circuit operates in ACTIVE mode, where the transistor acts to amplify the 
 * current at the base-emitter junction through to the relay switch. Active mode can occur when 
 * voltage at the base-emitter junction reaches .7V (metered as .749V), and with only a very small 
 * base current relative to the emitter current (metered at 180 mV at the base-emitter junction). 
 * The relationship between emitter versus base current is known as DC current gain. Typical 
 * ratios can vary between 50-200, where the project circuit is calculated at 138 mA.
 *  
 * 
 * BASIC BUTTTON OPERATION
 *    The major programming focus is simple, consistent operation, where the two major modes, 
 * manual and auto, work identically. Manual mode simply means pressing the yellow button to 
 * instantly switch ON the fan; pressing the same button again switches OFF the fan. Auto mode 
 * means pressing the red button to instantly activate continuous threshold level checks for 
 * temperature and humidity. At anytime when the sensor detects either temperature OR humidity 
 * readings at/above threshold, the fan is switched ON. At anytime when readings fall below both 
 * thresholds, the fan is switched OFF. Pressing the red button again instantly deactivates 
 * threshold checks. Threshold checks are presently set within the driver program (an advanced 
 * revision would allow threshold adjustments by button press, like on a thermostat). 
 * 
 * OLED DISPLAY SCREENS
 *    There are three separate OLED display screens, and are programmed for activation based on
 * user button presses. A pair of YELLOW button presses (for switching on and off the fan) displays 
 * the same "Manual" OLED screen. A pair of RED button presses displays the "Auto" OLED screen. 
 * Either mode screens allows simple navigation to the alternate mode. However, a second button 
 * press for the button opposite from the first button press displays the Error press OLED screen. 
 * The layout for each operation mode screen is described below:
 * I) MANUAL mode OLED screen
 *    This screen appears when the driver program starts. From top to bottom lines, it consists 
 * of three major components: 
 *  -Operation mode ID line, 
 *  -Mode navigation header lines, 
 *  -Mode navigation option selection lines (with manual mode selections appearing first). 
 * Here is a detailed breakdown for each line of the three components:
 * 1) Header screen ID line, appearing as:  
 *   " * MANUAL * "
 * 2) Button color (left-justified) and operation mode header labels (right-justified):
 *   " <BUTTON> 
 *           MODE "
 *  This layout style employs placing headers, normally adjacent on a single line, to be staggered 
 * over two lines, compensating for the OLED display's space restrictions. The button color header 
 * is delimited by angle brackets, <>, visually distinguishing it from the op mode header.
 * 3) Button color name and operation mode switch names associated for MANUAL mode:
 *   " <YELLOW>
 *           OFF
 *         MANUAL "
 *  This layout inherits multiple features from the staggered header lines, including names 
 * justified and lining up under their associated headers, angle brackets for the button color
 * name, and both switch names right-justified on separate lines for clear association. Valid
 * selections are "OFF" or "MANUAL", while highlighting the active selection "MANUAL" upon button 
 * press. 
 * 4) Button color name and operation mode switch names associated for AUTO mode:
 *   " <RED> AUTO "
 *  Layout features here also inherit those from the header line. These are also both valid 
 * selections, one of which to be activated by pressing the RED button. 
 *    
 * II) AUTOMATIC mode OLED screen
 * This mode uses the entire 16 lines to display mode-related info. From top to bottom lines, it 
 * consists of five major components: 
 *  -Operation mode ID line, 
 *  -Mode navigation header lines, 
 *  -Mode navigation option selection lines (with auto mode selections appearing first)
 *  -BME temperature & humidity sensor data: separate lines for the two threshold settings, 
 *    followed by the two sensor readings
 *  -Prep-related reminder instructions for opening the attic access door & house windows for 
 * proper air circulation
 * 1) Header screen ID line, appearing as:  
 *   " *  AUTO  * "
 * 2) Button color (left-justified) and operation mode header labels (right-justified):
 *   " <BUTTON> 
 *           MODE "
 * 3) Button color name and the opposing operation mode switch associated for AUTO mode:
 *   " <RED> OFF "
 *  The active, highlighted selection upon button press is <RED>.
 * 4) Button color name and operation mode switch names associated for MANUAL mode:
 *   " <YELLOW> "
 * 5) Two lines for header labels displaying programmed temperature & humidity settings, 
 *  followed by a third line displaying their values:
 *   "  TEMP HUM "
 *   "  SETTINGS "
 *   "  99    99 " 
 * 6) One line for a summary header label for displaying sensor readings of temperature & 
 * humidity settings, followed by a second line displaying actual readings:
 *    " SENSOR " 
 *   "  99   99 " 
 * 7) Four lines for drawing attention to and displaying reminder instructions for necessary
 * prep-related duties:
 *   " !!!!!!!!!! " 
 *   " OPEN WIDE "
 *   " ATTIC DOOR " 
 *   " & WINDOWS "
 *  These esential prep steps can be overlooked in auto mode because the fan may not be 
 * immediately activated (and which serves as the definitive reminder).
 * 
 * III) ERROR Press OLED screen
 *  This screen appears when the button colors from the two consecutive button presses do not 
 * match; in effect, the user does not correctly toggle OFF the intial operating mode after 
 * toggling it ON. One of two separate error screens appear, depending on the press sequence:
 * 1) If the YELLOW button is first pressed         2) If the RED button is first pressed 
 * initating MANUAL mode, followed by a RED button  initating AUTO mode, followed by a YELLOW 
 * press (AUTO mode), this screen appears:          button press (MANUAL mode), this screen appears:
 *  "  PRESS  "                                       "  PRESS  "
 *  "  YELLOW "                                       "   RED "
*  "  BUTTON "                                        "  BUTTON " 
 *     " TO "                                            " TO "
 *  "  RETURN "                                       "  RETURN "
 *     " TO "                                            " TO "
 *  "  MANUAL "                                        "  AUTO "
 *    " MODE "                                          " MODE "
 *  In either case, following the instruction returns the user back to the previous op mode and
 * switch position.
 * 
 * PROGRAMMING THE CIRCUIT   
 *  See comments embedded within the main loop logic 
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