// ============================================================
// Example:     LCDML_serialmonitor
// ============================================================
// Description:
// This example includes the complete functionality over some
// tabs. All Tabs which are started with "LCDML_display_.."
// generates an output on the display / console / ....
// This example is for the author to test the complete functionality
// ============================================================

// *********************************************************************
// includes
// *********************************************************************
  #include <LiquidCrystal.h>
  #include <LCDMenuLib2.h>
  #include <Wire.h>
#include "RTClib.h" // from https://github.com/adafruit/RTClib
#include <TimeLord.h> // from http://swfltek.com/arduino/timelord.html. When adding it to your IDE, rename the file, removing the "-depreciated"
#include <EEPROM.h>
#include <BME280.h>

// *********************************************************************
// LCDML display settings
// *********************************************************************
  // settings for LCD
  #define _LCDML_DISP_cols  20
  #define _LCDML_DISP_rows  4

  #define _LCDML_DISP_cfg_cursor                     0x7E   // cursor Symbol
  #define _LCDML_DISP_cfg_scrollbar                  1      // enable a scrollbar
  
  // LCD object
  // liquid crystal needs (rs, e, dat4, dat5, dat6, dat7)
LiquidCrystal lcd(12, 13, 7, 6, 5, 4); // 
  const uint8_t scroll_bar[5][8] = {
    {B10001, B10001, B10001, B10001, B10001, B10001, B10001, B10001}, // scrollbar top
    {B11111, B11111, B10001, B10001, B10001, B10001, B10001, B10001}, // scroll state 1
    {B10001, B10001, B11111, B11111, B10001, B10001, B10001, B10001}, // scroll state 2
    {B10001, B10001, B10001, B10001, B11111, B11111, B10001, B10001}, // scroll state 3
    {B10001, B10001, B10001, B10001, B10001, B10001, B11111, B11111}  // scrollbar bottom
  };
BME280 bme;


RTC_DS1307 RTC;
// *********************************************************************
// Prototypes
// *********************************************************************
  void lcdml_menu_display();
  void lcdml_menu_clear();
  void lcdml_menu_control();

// *********************************************************************
// Global variables
// *********************************************************************


// *********************************************************************
// Objects
// *********************************************************************
  LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
  LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);

// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
  // LCDML_0        => layer 0
  // LCDML_0_X      => layer 1
  // LCDML_0_X_X    => layer 2
  // LCDML_0_X_X_X  => layer 3
  // LCDML_0_...      => layer ...

  // For beginners
  // LCDML_add(id, prev_layer, new_num, lang_char_array, callback_function)
  LCDML_add         (0  , LCDML_0         , 1  , "Information"      , mFunc_information);       // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (1  , LCDML_0         , 2  , "Time info"        , mFunc_timer_info);        // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (2  , LCDML_0         , 3  , "Program"          , NULL);                    // NULL = no menu function
  LCDML_add         (3  , LCDML_0_3       , 1  , "Program 1"        , NULL);                    // NULL = no menu function
  LCDML_add         (4  , LCDML_0_3_1     , 1  , "P1 dummy"         , NULL);                    // NULL = no menu function
  LCDML_add         (5  , LCDML_0_3_1     , 2  , "P1 Settings"      , NULL);                    // NULL = no menu function
  LCDML_add         (6  , LCDML_0_3_1_2   , 1  , "Warm"             , NULL);                    // NULL = no menu function
  LCDML_add         (7  , LCDML_0_3_1_2   , 2  , "Cold"             , NULL);                    // NULL = no menu function
  LCDML_add         (8  , LCDML_0_3_1_2   , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (9  , LCDML_0_3_1     , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (10 , LCDML_0_3       , 2  , "Program 2"        , mFunc_p2);                // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (11 , LCDML_0_3       , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (12 , LCDML_0         , 4  , "Special"          , NULL);                    // NULL = no menu function
  LCDML_add         (13 , LCDML_0_4       , 1  , "Go to Root"       , mFunc_goToRootMenu);      // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (14 , LCDML_0_4       , 2  , "Jump to Time info", mFunc_jumpTo_timer_info); // this menu function can be found on "LCDML_display_menuFunction" tab
  LCDML_add         (15 , LCDML_0_4       , 3  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab


  // Advanced menu (for profit) part with more settings
  // Example for one function and different parameters
  // It is recommend to use parameters for switching settings like, (small drink, medium drink, big drink) or (200ml, 400ml, 600ml, 800ml) ...
  // the parameter change can also be released with dynParams on the next example
  // LCDMenuLib_add(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  LCDML_addAdvanced (16 , LCDML_0         , 5  , NULL,          "Parameter Grow Box"      , NULL,                0,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (17 , LCDML_0_5       , 1  , NULL,          "Humidity %"      , mFunc_para,       10,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (18 , LCDML_0_5       , 2  , NULL,          "Temperature Day"      , temperatura,       20,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (19 , LCDML_0_5       , 3  , NULL,          "Temperature Night"      , mFunc_para,       30,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (20 , LCDML_0_5       , 4  , NULL,          "Hysteresis Hum."      , mFunc_para,       40,            _LCDML_TYPE_default);                    // NULL = no menu function
  LCDML_addAdvanced (21 , LCDML_0_5       , 5  , NULL,          "Hysteresis Temp."      , mFunc_para,       50,            _LCDML_TYPE_default);                    // NULL = no menu function

  LCDML_add         (22 , LCDML_0_5       , 6  , "Back"             , mFunc_back);              // this menu function can be found on "LCDML_display_menuFunction" tab


  // Example for dynamic content
  // 1. set the string to ""
  // 2. use type  _LCDML_TYPE_dynParam   instead of    _LCDML_TYPE_default
  // this function type can not be used in combination with different parameters
  // LCDMenuLib_add(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )

  // Example for conditions (for example for a screensaver)
  // 1. define a condition as a function of a boolean type -> return false = not displayed, return true = displayed
  // 2. set the function name as callback (remove the braces '()' it gives bad errors)
  // LCDMenuLib_add(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  LCDML_addAdvanced (23 , LCDML_0         , 7  , COND_hide,  "Exit"        , mFunc_screensaver,        0,   _LCDML_TYPE_default);       // this menu function can be found on "LCDML_display_menuFunction" tab

  // ***TIP*** Try to update _LCDML_DISP_cnt when you add a menu element.

  // menu element count - last element id
  // this value must be the same as the last menu element
  #define _LCDML_DISP_cnt    23

  // create menu
  LCDML_createMenu(_LCDML_DISP_cnt);

// *********************************************************************
// SETUP
// *********************************************************************

//nuovo

//the variables provide the holding values for the set clock routine

int setsecs = 0;
int maxday;        // maximum number of days in the given month
int TimeMins;      // number of seconds since midnight
byte TimerMode; //mode 0=crescita 1=autofiorente
int TimeOut = 10;
int TimeOutCounter;

// variabili isteresi
byte setistetemp;
byte setisteum = 1;
byte setpointum = 2;
byte setpointdaytemp;
byte setpointumnight = 4;
byte setpointtempnight = 5;

const int TIMEZONE = 2;                             //UTC
const float LATITUDE = 40.54, LONGITUDE = 17.14;    // set YOUR position here  //COORDINATE MONOPOLI
int Sunrise, Sunset;                                //sunrise and sunset expressed as minute of day (0-1439)
TimeLord myLord;                                    // TimeLord Object, Global variable
byte sunTime[] = {0, 0, 0, 1, 1, 13};               // 17 Oct 2013
int SunriseHour, SunriseMin, SunsetHour, SunsetMin; //Variables used to make a decent display of our sunset and sunrise time.
int umidificatorestate = 0;
unsigned long iMillis = 0;
unsigned long tempoON = 10000;
byte gocciaChar[8] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110
};

byte temperaturaChar[8] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};

byte gradiChar[8] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte ledChar[8] = {
  B10101,
  B01110,
  B10001,
  B10001,
  B01110,
  B01010,
  B01010,
  B01110
};

byte offChar[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000,
  B00000
};

byte onChar[8] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000
};
  void setup()
  {
    // serial init; only be needed if serial control is used


    // LCD Begin
    lcd.begin(_LCDML_DISP_cols,_LCDML_DISP_rows);
    // set special chars for scrollbar
    lcd.createChar(0, (uint8_t*)scroll_bar[0]);
    lcd.createChar(1, (uint8_t*)scroll_bar[1]);
    lcd.createChar(2, (uint8_t*)scroll_bar[2]);
    lcd.createChar(3, (uint8_t*)scroll_bar[3]);
    lcd.createChar(4, (uint8_t*)scroll_bar[4]);
    lcd.createChar(5, gocciaChar);
    lcd.createChar(6, temperaturaChar);
    lcd.createChar(7, ledChar);

    // LCDMenuLib Setup
    LCDML_setup(_LCDML_DISP_cnt);

    // Some settings which can be used

    // Enable Menu Rollover
    LCDML.MENU_enRollover();

    // Enable Screensaver (screensaver menu function, time to activate in ms)
   // LCDML.SCREEN_enable(mFunc_screensaver, 10000); // set to 10 seconds
    LCDML.SCREEN_disable();

    // Some needful methods

    // You can jump to a menu function from anywhere with
    //LCDML.OTHER_jumpToFunc(mFunc_p2); // the parameter is the function name
    //nuovo
      Wire.begin();     //start I2C interface
  RTC.begin();      //start RTC interface
  
  pinMode(A3, OUTPUT);    //Relay connected to A3 rele luci
  digitalWrite(A3, HIGH); //sets relay off (default condition)
  pinMode(A2, OUTPUT);    //Relay connected to A2 riscaldamento
  digitalWrite(A2, HIGH); //sets relay off (default condition)
  pinMode(A1, OUTPUT);    //Relay connected to A1 umidificatore
  digitalWrite(A1, HIGH); //sets relay off (default condition)
  setistetemp = EEPROM.read(1);

  setisteum = EEPROM.read(2);
  
  setpointum = EEPROM.read(3);
 

  setpointtempnight = EEPROM.read(5);
 
  TimerMode = EEPROM.read(6);
  //Checks to see if the RTC is runnning, and if not, sets the time to the time this sketch was compiled.



  bme.begin();

 


  //Timelord initialisation
  myLord.TimeZone(TIMEZONE * 60);
  myLord.Position(LATITUDE, LONGITUDE);
  CalcSun();
  }
//
// *********************************************************************
// LOOP
// *********************************************************************
  void loop()
  {
    
    LCDML.loop();

    
  }
  

// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_DISP_rows > _LCDML_DISP_cfg_max_rows)
# error change value of _LCDML_DISP_cfg_max_rows in LCDMenuLib2.h
# endif


int CalcSun()
{ //Calculates the Sunrise and Sunset times
  DateTime now = RTC.now();
  sunTime[3] = now.day(); // Give Timelord the current date
  sunTime[4] = now.month();
  sunTime[5] = now.year();
  myLord.SunRise(sunTime);                // Computes Sun Rise.
  Sunrise = sunTime[2] * 60 + sunTime[1]; // Sunrise returned in minutes past midnight
  SunriseHour = sunTime[2];
  SunriseMin = sunTime[1];
  sunTime[3] = now.day(); // Uses the Time library to give Timelord the current date
  sunTime[4] = now.month();
  sunTime[5] = now.year();
  myLord.SunSet(sunTime);                // Computes Sun Set.
  Sunset = sunTime[2] * 60 + sunTime[1]; // Sunset returned in minutes past midnight
  SunsetHour = sunTime[2];
  SunsetMin = sunTime[1];
}
