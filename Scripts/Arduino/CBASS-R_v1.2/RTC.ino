const char monthsOfTheYear[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
String getdate()
{
   t = rtc.now();
   String dateStr =  String(t.year(), DEC) + "_" + String(monthsOfTheYear[t.month()-1])  + "_" + String(t.day(), DEC);
   return dateStr;
}


void checkTime()
{
  t = rtc.now();
  Serial.print("Time: ");
  Serial.print(t.hour(), DEC);
  Serial.print(":");
  Serial.println(t.minute(), DEC);
}

void clockInit() {
  while (!rtc.begin()) {
    Serial.println("Couldn't start RTC");
    delay(2000);
  }
  Serial.println("Started RTC");
}
