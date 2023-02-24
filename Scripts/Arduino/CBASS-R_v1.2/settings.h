
// ***** Temp Program Inputs *****
double RAMP1_START_TEMP = 30.5;
double RAMP2_START_TEMP = 30.75;
double RAMP3_START_TEMP = 30.5;
double RAMP4_START_TEMP = 30.75;

#define CHILLER_OFFSET 0.20
#define TANK1_TEMP_CORRECTION 0 // Is a temperature correction for the temp sensor, the program subtracts this from the temp readout e.g. if the sensor reads low, this should be a negative number
#define TANK2_TEMP_CORRECTION 0 // Is a temperature correction for the temp sensor, the program subtracts this from the temp readout e.g. if the sensor reads low, this should be a negative number
#define TANK3_TEMP_CORRECTION 0 // Is a temperature correction for the temp sensor, the program subtracts this from the temp readout e.g. if the sensor reads low, this should be a negative number
#define TANK4_TEMP_CORRECTION 0 // Is a temperature correction for the temp sensor, the program subtracts this from the temp readout e.g. if the sensor reads low, this should be a negative number

// ***** PID TUNING CONSTANTS ****
#define KP 2000//5000//600 //IN FIELD - Chillers had higher lag, so I adjusted the TPCwindow and KP to 20 secs, kept all proportional
#define KI 10//KP/100//27417.54//240 // March 20 IN FIELD - with 1 deg steps, no momentum to take past P control, so doubled I. (10->40)
#define KD 1000//40  //

#define RELAY_ON 1
#define RELAY_OFF 0
#define T1Index 0
#define T2Index 1
#define T3Index 2
#define T4Index 3

#define T1HeaterRelay 17  // T1 Heat DB9 pin 9 black wire Arduino Digital I/O pin number 17
#define T1ChillRelay  22  // T1 Chill DB9 pin 1 brown wire Arduino Digital I/O pin number 22
#define T2HeaterRelay 15  // T2 Heat DB9 pin 8 white wire Arduino Digital I/O pin number 15
#define T2ChillRelay  24 // T2 Heat DB9 pin 2 red wire Arduino Digital I/O pin number 24
#define T3HeaterRelay 16  // T3 Heat DB9 pin 7 purple wire Arduino Digital I/O pin number 16
#define T3ChillRelay  23  // T3 Chill DB9 pin 3 orange Arduino Digital I/O pin number 23
#define T4HeaterRelay 25 // T4 Heat DB9 pin 6 blue wire Arduino Digital I/O pin number 25
#define T4ChillRelay  14  // T4 Chill DB9 pin 4 yellow wire Arduino Digital I/O pin number 14

// Other pins
#define TFT_CS   7
#define TFT_DC   6
// SD card.  Note that there is a second SD card on the back of the display, which we don't currently use.
// This is for the active one.
#define SD_DETECT  10
#define SD_CS  11

// Display constants
#define TFT_WIDTH 320
#define TFT_HEIGHT 240
#define TFT_LAND 3  // Rotation for normal text  1 = SPI pins at top, 3 = SPI at bottom.
#define TFT_PORT 2  // Rotation for y axis label, typically 1 less than TFT_LAND
#define LINEHEIGHT 19 // Pixel height of size 2 text is 14.  Add 1 or more for legibility.
#define LINEHEIGHT3 28 // Pixel height of size 3 text is ??.  Add 1 or more for legibility.


byte degree[8] = // define the degree symbol
{
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000
};
