#include <RTClib.h>
#include <Adafruit_RGBLCDShield.h>

RTC_DS1307 rtc;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

int uploadLag = 6; // About how long it takes to upload the sketch and actually set the time.

void showTime() {
  DateTime now = rtc.now();
  
  lcd.setCursor(0, 1);
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second());
  lcd.print("   ");

  lcd.setCursor(0, 0);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(", ");
  lcd.print(now.day());
  lcd.print('/');
  lcd.print(now.month());
  lcd.print('/');
  lcd.print(now.year());
}


void setup() {
  Serial.begin(9600);
  Serial.println("ClockSetter");
  lcd.begin(16, 2);              // start the library
  lcd.setCursor(0, 0);
  lcd.print("ClockSetter");
  delay(1000);
  if (!rtc.begin()) {
    // This code may not be what I thought.  It returns true if there is
    // no RTC!  The isrunning call will catch that case.
    Serial.println("Couldn't start RTC");
    while (1);
  }
  if (!rtc.isrunning()) {
    Serial.println("Failed to communicate with RTC.  Is one connected?");
    while (1);
  }
  Serial.println("Found RTC"); // New 2/8/19
  // following line sets the RTC to the date & time this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)) + TimeSpan(0, 0, 0, uploadLag));
  Serial.println("Set RTC. Install another sketch now.");
}

void loop() {
  showTime();
  delay(500);
}
