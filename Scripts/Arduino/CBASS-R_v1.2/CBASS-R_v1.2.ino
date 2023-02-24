/********************************************************
   PID RelayTempOutput Example
   Same as basic example, except that this time, the TempOutput
   is going to a digital pin which (we presume) is controlling
   a relay.  The pid is designed to TempOutput an analog value,
   but the relay can only be On/Off.

     To connect them together we use "time proportioning
   control"  Tt's essentially a really slow version of PWM.
   First we decide on a window size (5000mS say.) We then
   set the pid to adjust its TempOutput between 0 and that window
   size.  Lastly, we add some logic that translates the PID
   TempOutput into "Relay On Time" with the remainder of the
   window being "Relay Off Time"
 ********************************************************/

// Library for the Adafruit TFT LCD Display
#include <Adafruit_ILI9341.h>

// SD card shield library
#include <SD.h>
#include <SPI.h>
// PID Library
#include <PID_v1.h>
//#include <PID_AutoTune_v0.h>
// So we can save and retrieve settings
#include <EEPROM.h>
// Libraries for the DS18B20 Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RTClib.h>
#include "settings.h"

// XXX delete after code is converted:
//#include <Adafruit_RGBLCDShield.h>
//Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();


// The new display uses SPI communication (not I2C), shared with other components.
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);  // The CS and DC pins.

// Colors are RGB, but 16 bits, not 24, allocated as 5, 6, and 5 bits for the 3 channels.
// For example, a light blue could be 0x1F1FFF in RGB, but 0x1F3A in 16-bit form. To convert
// typical RBG given as a,b,c, use 2048*a*31/255 + 32*b*63/255 + c*31/255
#define DARKGREEN 0x05ED  // Normal green is too pale on white. 
#define LIGHTBLUE 0x1F3A  // Normal blue is too dark on black.
// Easier names for some of the colors in the ILI9341 header file.
#define BLACK   ILI9341_BLACK
#define BLUE    ILI9341_BLUE
#define RED     ILI9341_RED
#define GREEN   ILI9341_GREEN
#define CYAN    ILI9341_CYAN
#define MAGENTA ILI9341_MAGENTA
#define YELLOW  ILI9341_YELLOW
#define WHITE   ILI9341_WHITE

// RTC is now DS3231, but the model number doesn't seem to matter.
RTC_DS1307  rtc;

DateTime  t;

// Make two files, one for logging the data and one for reading in the setpoints
File logFile;
File setpoints;
String printdate = "2022-10-20_CBASSR_v1.2Testing"; // No spaces

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//Temp Sensors pin
const int TempSensorPin = 8;


//Setup OneWire
OneWire oneWire(TempSensorPin);


//Setup temp sensors
DallasTemperature sensors(&oneWire);

DeviceAddress T1Thermometer, T2Thermometer, T3Thermometer, T4Thermometer;

//Define Variables we'll Need
//Temperature Variables
double tempT1, tempT2, tempT3, tempT4; //defining variable for handling errors
double T1SetPoint, T1TempInput, T1TempOutput, T1Correction, T1offT, T2SetPoint, T2TempInput, T2TempOutput, T2Correction, T2offT;
double T3SetPoint, T3TempInput, T3TempOutput, T3Correction, T3offT, T4SetPoint, T4TempInput, T4TempOutput, T4Correction, T4offT, ChillOffset; // With temp probe correction values
// Time Windows: Update LCD 2/sec; Serial, Ramp Status 1/sec, TPC 1/2 sec
unsigned int LCDwindow = 500, SERIALwindow = 1000, STEPwindow = 1000, i;
// misc.
//int NegIfRamping = -1, TPCwindow = KP;
int NegIfRamping = -1, TPCwindow = 10000;
unsigned int numberOfSensors = 0, SerialOutCount = 101;

// Display Conversion Strings
char T1SetPointStr[5], T2SetPointStr[5], T1TempInputStr[5], T2TempInputStr[5], T1TempOutputStr[4], T2TempOutputStr[4], ErrStr[4];
char T3SetPointStr[5], T4SetPointStr[5], T3TempInputStr[5], T4TempInputStr[5], T3TempOutputStr[4], T4TempOutputStr[4];
char hrsStr[3] = "0", minsStr[3] = "0", secsStr[3] = "0";
char T1RelayStateStr[4] = "OFF", T2RelayStateStr[4] = "OFF", T3RelayStateStr[4] = "OFF", T4RelayStateStr[4] = "OFF";


//Specify the links and initial tuning parameters
double kp = KP, ki = KI, kd = KD; //kp=350,ki= 300,kd=50;

// EEPROM addresses for persisted data
const int SpAddress = 0;
const int KpAddress = 8;
const int KiAddress = 16;
const int KdAddress = 24;

// PID Controllers
PID T1_PID(&T1TempInput, &T1TempOutput, &T1SetPoint, kp, ki, kd, DIRECT);
PID T2_PID(&T2TempInput, &T2TempOutput, &T2SetPoint, kp, ki, kd, DIRECT);
PID T3_PID(&T3TempInput, &T3TempOutput, &T3SetPoint, kp, ki, kd, DIRECT);
PID T4_PID(&T4TempInput, &T4TempOutput, &T4SetPoint, kp, ki, kd, DIRECT);

// 10 second Time Proportional Output window
int WindowSize = 10000;
unsigned long windowStartTime;

//TimeKeepers
unsigned long now_ms = millis(), hrs = 0, mins = 0, secs = 0, SERIALt, LCDt, TPCt, STEP1t, STEP2t, HOLD1t, HOLD2t;

// ************************************************
// States for state machine
// ************************************************
// 2 lines not used, so commented.
//enum operatingState { OFF = 0, SETP, RUN, TUNE_P, TUNE_I, TUNE_D, AUTO};
//operatingState opState = OFF;


void setup()
{

  RelaysInit();
  delay(2000); //Check that all relays are inactive at Reset

  // ***** INITALIZE OUTPUT *****`
  startDisplay();
  Serial.begin(9600);          //  setup serial for sensor

  clockInit();

  PIDinit();

  sensorsInit();

  checkTime();
  SDinit();
  if (ReadSettings(1440))
  {
    RampSet();
    ShowRampInfo();
  }
  else
  {
    Serial.println(F("No settings for this time period"));
  }

  tft.fillScreen(BLACK);
  tft.setTextSize(3);
  tft.setCursor(0, 0);  // remember that the original LCD counts characters.  This counts pixels.
  tft.print("PrintDate is:");
  tft.setCursor(0, LINEHEIGHT3);
  tft.print(printdate);
  Serial.println("PrintDate is:");
  Serial.println(printdate);
  delay(3000);
  tft.setCursor(0, LINEHEIGHT3*3);
  tft.print("1. 5s Pause...");
  Serial.println();
  Serial.println();
  Serial.print("Initialization sequence.");
  Serial.println();
  Serial.print("1. 5s Pause...");
  Serial.println();
  delay(5000);
  Serial.println();

  // Relay Tests
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  //Test Relay 1 "T1 Heater"
  tft.print("1. T1Heatr");
  Serial.print("1. T1Heatr");
  Serial.println();
  digitalWrite(T1HeaterRelay, RELAY_ON);
  delay(5000);
  digitalWrite(T1HeaterRelay, RELAY_OFF);
  //Test Relay 2 "T1 Chiller"
  //tft.fillScreen(BLACK);
  tft.setCursor(0, LINEHEIGHT3);
  tft.print("2. T1Chillr");
  Serial.print("2. T1Chillr");
  Serial.println();
  digitalWrite(T1ChillRelay, RELAY_ON);
  delay(5000);
  digitalWrite(T1ChillRelay, RELAY_OFF);
  //Test Relay 3 "T2 Heater"
  //tft.fillScreen(BLACK);
  tft.setCursor(0, LINEHEIGHT3*2);
  tft.print("3. T2Heatr");
  Serial.print("3. T2Heatr");
  Serial.println();
  digitalWrite(T2HeaterRelay, RELAY_ON);
  delay(5000);
  digitalWrite(T2HeaterRelay, RELAY_OFF);
  //Test Relay 4 "T2 Chiller"
  //tft.fillScreen(BLACK);
  tft.setCursor(0, LINEHEIGHT3*3);
  tft.print("4. T2Chillr");
  Serial.print("4. T2Chillr");
  Serial.println();
  digitalWrite(T2ChillRelay, RELAY_ON);
  delay(5000);
  digitalWrite(T2ChillRelay, RELAY_OFF);
  Serial.println();
  //tft.fillScreen(BLACK);  // Blank and start from top.
  tft.setCursor(0, LINEHEIGHT3*4);
  //Test Relay 5 "T3 Heater"
  tft.print("5. T3Heatr");
  Serial.print("5. T3Heatr");
  Serial.println();
  digitalWrite(T3HeaterRelay, RELAY_ON);
  delay(5000);
  digitalWrite(T3HeaterRelay, RELAY_OFF);
  //Test Relay 6 "T3 Chiller"
  //tft.fillScreen(BLACK);
  tft.setCursor(0, LINEHEIGHT3*5);
  tft.print("6. T3Chillr");
  Serial.print("6. T3Chillr");
  Serial.println();
  digitalWrite(T3ChillRelay, RELAY_ON);
  delay(5000);
  digitalWrite(T3ChillRelay, RELAY_OFF);
  //Test Relay 7 "T4 Heater"
  //tft.fillScreen(BLACK);
  tft.setCursor(0, LINEHEIGHT3*6);
  tft.print("7. T4Heatr");
  Serial.print("7. T4Heatr");
  Serial.println();
  digitalWrite(T4HeaterRelay, RELAY_ON);
  delay(5000);
  digitalWrite(T4HeaterRelay, RELAY_OFF);
  //Test Relay 8 "T4 Chiller"
  //tft.fillScreen(BLACK);
  tft.setCursor(0, LINEHEIGHT3*7);
  tft.print("8. T4Chillr");
  Serial.print("8. T4Chillr");
  Serial.println();
  digitalWrite(T4ChillRelay, RELAY_ON);
  delay(5000);
  digitalWrite(T4ChillRelay, RELAY_OFF);
  Serial.println();
  Serial.print("PrintDate,Date,N_ms,Th,Tm,Ts,");
  Serial.print("T1SP,T1inT,T1RelayState,");
  Serial.print("T2SP,T2inT,T2RelayState,");
  Serial.print("T3SP,T3inT,T3RelayState,");
  Serial.print("T4SP,T4inT,T4RelayState,");
  Serial.println();

  // Clear the screen before loop() because we may not fully clear it in the loop.
  tft.fillScreen(BLACK);
}


unsigned long timer24h = 0;

void loop()
{
  // ***** Time Keeping *****
  now_ms = millis();
  // Make Milliseconds into sane time
  hrs = now_ms / 3600000;
  mins = (now_ms - (hrs * 3600000)) / 60000;
  secs = (now_ms - (hrs * 3600000 + mins * 60000)) / 1000;

  // ***** INPUT FROM TEMPERATURE SENSORS *****
  sensors.requestTemperatures();
  
  tempT1 = sensors.getTempCByIndex(0) - T1Correction;
  if (0.0 < tempT1 && tempT1 < 80.0)  T1TempInput = tempT1;

  tempT2 = sensors.getTempCByIndex(1) - T2Correction;
  if (0.0 < tempT2 && tempT2 < 80.0)  T2TempInput = tempT2;

  tempT3 = sensors.getTempCByIndex(2) - T3Correction;
  if (0.0 < tempT3 && tempT3 < 80.0)  T3TempInput = tempT3; // Above plus probe correction value

  tempT4 = sensors.getTempCByIndex(3) - T4Correction;
  if (0.0 < tempT4 && tempT4 < 50.0)  T4TempInput = tempT4; // Above plus probe correction value

  if ((now_ms - timer24h) > 60000)
  {
    checkTime();
    timer24h = now_ms;
    Serial.println("Checking settings");
    if (ReadSettings(5))
    {
      RampSet();
      ShowRampInfo();
      Serial.println("Settings loaded");
    }
    else
    {
      Serial.println("No new Settings found");
    }
  }

  // ***** UPDATE PIDs *****
  T1_PID.Compute();
  T2_PID.Compute();
  T3_PID.Compute();
  T4_PID.Compute();

  //***** UPDATE TIME WINDOW for TIME PROPORTIONAL CONTROL *****
  if (now_ms - TPCt > TPCwindow) {
    //    Serial.println("UpdatedTPCt");
    //    Serial.println(TPCt);
    TPCt += TPCwindow;   //time to shift the Relay Window
  }

  //***** UPDATE RELAY STATE for TIME PROPORTIONAL CONTROL *****
  // Tank1
  if (T1TempOutput < 0) { //Chilling
    if (T1TempInput > T1SetPoint - ChillOffset) {
      digitalWrite(T1ChillRelay, RELAY_ON);
      digitalWrite(T1HeaterRelay, RELAY_OFF);
      strcpy(T1RelayStateStr, "CHL");
    }
    else {
      digitalWrite(T1ChillRelay, RELAY_OFF);
      digitalWrite(T1HeaterRelay, RELAY_OFF);
      strcpy(T1RelayStateStr, "OFF");
    }
  } else { //Heating
    if (T1TempOutput > 0) {
      if (T1TempInput > T1SetPoint - ChillOffset) {
        digitalWrite(T1HeaterRelay, RELAY_ON);
        digitalWrite(T1ChillRelay, RELAY_ON);
        strcpy(T1RelayStateStr, "HTR");
      }
      else {
        digitalWrite(T1HeaterRelay, RELAY_ON);
        digitalWrite(T1ChillRelay, RELAY_OFF);
        strcpy(T1RelayStateStr, "HTR");
      }
    }
    else {
      if (T1TempInput > T1SetPoint - ChillOffset) {
        digitalWrite(T1HeaterRelay, RELAY_ON);
        digitalWrite(T1ChillRelay, RELAY_ON);
        strcpy(T1RelayStateStr, "HTR");
      }
      else {
        digitalWrite(T1HeaterRelay, RELAY_OFF);
        digitalWrite(T1ChillRelay, RELAY_ON);
        strcpy(T1RelayStateStr, "OFF");
      }
    }
  }
  // Tank2
  if (T2TempOutput < 0) { //Chilling
    if (T2TempInput > T2SetPoint - ChillOffset) {
      digitalWrite(T2ChillRelay, RELAY_ON);
      digitalWrite(T2HeaterRelay, RELAY_OFF);
      strcpy(T2RelayStateStr, "CHL");
    }
    else {
      digitalWrite(T2ChillRelay, RELAY_OFF);
      digitalWrite(T2HeaterRelay, RELAY_OFF);
      strcpy(T2RelayStateStr, "OFF");
    }
  } else { //Heating
    if (T2TempOutput > 0) {
      if (T2TempInput > T2SetPoint - ChillOffset) {
        digitalWrite(T2HeaterRelay, RELAY_ON);
        digitalWrite(T2ChillRelay, RELAY_ON);
        strcpy(T2RelayStateStr, "HTR");
      }
      else {
        digitalWrite(T2HeaterRelay, RELAY_ON);
        digitalWrite(T2ChillRelay, RELAY_OFF);
        strcpy(T2RelayStateStr, "HTR");
      }
    }
    else {
      if (T2TempInput > T2SetPoint - ChillOffset) {
        digitalWrite(T2HeaterRelay, RELAY_ON);
        digitalWrite(T2ChillRelay, RELAY_ON);
        strcpy(T2RelayStateStr, "HTR");
      }
      else {
        digitalWrite(T2HeaterRelay, RELAY_OFF);
        digitalWrite(T2ChillRelay, RELAY_ON);
        strcpy(T2RelayStateStr, "OFF");
      }
    }
  }
  // Tank3
  if (T3TempOutput < 0) { //Chilling
    if (T3TempInput > T3SetPoint - ChillOffset) {
      digitalWrite(T3ChillRelay, RELAY_ON);
      digitalWrite(T3HeaterRelay, RELAY_OFF);
      strcpy(T3RelayStateStr, "CHL");
    }
    else {
      digitalWrite(T3ChillRelay, RELAY_OFF);
      digitalWrite(T3HeaterRelay, RELAY_OFF);
      strcpy(T3RelayStateStr, "OFF");
    }
  } else { //Heating
    if (T3TempOutput > 0) {
      if (T3TempInput > T3SetPoint - ChillOffset) {
        digitalWrite(T3HeaterRelay, RELAY_ON);
        digitalWrite(T3ChillRelay, RELAY_ON);
        strcpy(T3RelayStateStr, "HTR");
      }
      else {
        digitalWrite(T3HeaterRelay, RELAY_ON);
        digitalWrite(T3ChillRelay, RELAY_OFF);
        strcpy(T3RelayStateStr, "HTR");
      }
    }
    else {
      if (T3TempInput > T3SetPoint - ChillOffset) {
        digitalWrite(T3HeaterRelay, RELAY_ON);
        digitalWrite(T3ChillRelay, RELAY_ON);
        strcpy(T3RelayStateStr, "HTR");
      }
      else {
        digitalWrite(T3HeaterRelay, RELAY_OFF);
        digitalWrite(T3ChillRelay, RELAY_ON);
        strcpy(T3RelayStateStr, "OFF");
      }
    }
  }
  // Tank4
  if (T4TempOutput < 0) { //Chilling
    if (T4TempInput > T4SetPoint - ChillOffset) {
      digitalWrite(T4ChillRelay, RELAY_ON);
      digitalWrite(T4HeaterRelay, RELAY_OFF);
      strcpy(T4RelayStateStr, "CHL");
    }
    else {
      digitalWrite(T4ChillRelay, RELAY_OFF);
      digitalWrite(T4HeaterRelay, RELAY_OFF);
      strcpy(T4RelayStateStr, "OFF");
    }
  } else { //Heating
    if (T4TempOutput > 0) {
      if (T4TempInput > T4SetPoint - ChillOffset) {
        digitalWrite(T4HeaterRelay, RELAY_ON);
        digitalWrite(T4ChillRelay, RELAY_ON);
        strcpy(T4RelayStateStr, "HTR");
      }
      else {
        digitalWrite(T4HeaterRelay, RELAY_ON);
        digitalWrite(T4ChillRelay, RELAY_OFF);
        strcpy(T4RelayStateStr, "HTR");
      }
    }
    else {
      if (T4TempInput > T4SetPoint - ChillOffset) {
        digitalWrite(T4HeaterRelay, RELAY_ON);
        digitalWrite(T4ChillRelay, RELAY_ON);
        strcpy(T4RelayStateStr, "HTR");
      }
      else {
        digitalWrite(T4HeaterRelay, RELAY_OFF);
        digitalWrite(T4ChillRelay, RELAY_ON);
        strcpy(T4RelayStateStr, "OFF");
      }
    }
  }



  //***** UPDATE SERIAL *****
  if (now_ms - SERIALt > SERIALwindow) {
    SerialReceive();
    SerialSend();
    SERIALt += SERIALwindow;
  }

  //***** UPDATE LCD *****
  if ((now_ms - LCDt) > LCDwindow)
  {
    // Two styles - a matter of taste.  This is also where a version with graphing could be called.
    // In that case a more compact text portion would be desireable.
    //displayTemperatureStatus();
    displayTemperatureStatusBold();
    // No need to delay.  LCDwindow should regulate things.
    // delay(2000);
    LCDt += LCDwindow;
  }
}

void SerialSend()
{
  if (SerialOutCount > 100) {
    Serial.print("PrintDate,Date,N_ms,Th,Tm,Ts,");
    Serial.print("T1SP,T1inT,TempT1,T1RelayState,");
    Serial.print("T2SP,T2inT,TempT2,T2RelayState,");
    Serial.print("T3SP,T3inT,TempT3,T3RelayState,");
    Serial.print("T4SP,T4inT,TempT4,T4RelayState,");
    Serial.println();
    logFile = SD.open("LOG.txt", FILE_WRITE);
    if (logFile) {
      logFile.print("PrintDate,Date,N_ms,Th,Tm,Ts,");
      logFile.print("T1SP,T1inT,TempT1,T1RelayState,");
      logFile.print("T2SP,T2inT,TempT2,T2RelayState,");
      logFile.print("T3SP,T3inT,TempT3,T3RelayState,");
      logFile.print("T4SP,T4inT,TempT4,T4RelayState,");
      logFile.println();
    }
    logFile.close();
    SerialOutCount = 0;
  }
  Serial.print(printdate), Serial.print(","), Serial.print(getdate()), Serial.print(","), Serial.print(now_ms), Serial.print(","), Serial.print(t.hour(), DEC), Serial.print(","), Serial.print(t.minute(), DEC), Serial.print(","), Serial.print(t.second(), DEC), Serial.print(",");
  Serial.print(T1SetPoint), Serial.print(","), Serial.print(T1TempInput), Serial.print(","), Serial.print(tempT1), Serial.print(","), Serial.print(T1TempOutput), Serial.print(","), Serial.print(T1RelayStateStr), Serial.print(",");
  Serial.print(T2SetPoint), Serial.print(","), Serial.print(T2TempInput), Serial.print(","), Serial.print(tempT2), Serial.print(","), Serial.print(T2TempOutput), Serial.print(","), Serial.print(T2RelayStateStr), Serial.print(",");
  Serial.print(T3SetPoint), Serial.print(","), Serial.print(T3TempInput), Serial.print(","), Serial.print(tempT3), Serial.print(","), Serial.print(T3TempOutput), Serial.print(","), Serial.print(T3RelayStateStr), Serial.print(",");
  Serial.print(T4SetPoint), Serial.print(","), Serial.print(T4TempInput), Serial.print(","), Serial.print(tempT4), Serial.print(","), Serial.print(T4TempOutput), Serial.print(","), Serial.print(T4RelayStateStr), Serial.print(",");
  Serial.println();
  logFile = SD.open("log.txt", FILE_WRITE);
  if (logFile) {
    logFile.print(printdate), logFile.print(","), logFile.print(getdate()), logFile.print(","), logFile.print(now_ms), logFile.print(","), logFile.print(t.hour(), DEC), logFile.print(","), logFile.print(t.minute(), DEC), logFile.print(","), logFile.print(t.second(), DEC), logFile.print(",");
    logFile.print(T1SetPoint), logFile.print(","), logFile.print(T1TempInput), logFile.print(","), logFile.print(tempT1), logFile.print(","), logFile.print(T1RelayStateStr), logFile.print(",");
    logFile.print(T2SetPoint), logFile.print(","), logFile.print(T2TempInput), logFile.print(","), logFile.print(tempT2), logFile.print(","), logFile.print(T2RelayStateStr), logFile.print(",");
    logFile.print(T3SetPoint), logFile.print(","), logFile.print(T3TempInput), logFile.print(","), logFile.print(tempT3), logFile.print(","), logFile.print(T3RelayStateStr), logFile.print(",");
    logFile.print(T4SetPoint), logFile.print(","), logFile.print(T4TempInput), logFile.print(","), logFile.print(tempT4), logFile.print(","), logFile.print(T4RelayStateStr), logFile.print(",");
    logFile.println();

  }
  logFile.close();
  SerialOutCount += 1;
}

void SerialReceive()
{
  if (Serial.available())
  {
    char b = Serial.read();
    Serial.flush();
  }
}
