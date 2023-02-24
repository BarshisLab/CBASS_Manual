

void SDinit()
{
  if (!SD.begin(SD_CS)) {
    Serial.println("SD initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}

boolean ReadSettings(int maxMinutesLag) // how far to count down from the actual time
{
  String cdata = "";
  boolean inDifferentNow = false;
  t = rtc.now();
  int nowHour = t.hour();
  int nowMin = t.minute();

  for (int filereadtries = 0; filereadtries < maxMinutesLag; filereadtries++) //counting down times by 1 minute increments
  {
    String timeATstring = String(nowHour) + ":" + String(nowMin);
    if (nowMin < 10)
    {
      timeATstring = String(nowHour) + ":0" + String(nowMin);
    }
 Serial.println("For Time: ");
Serial.println(timeATstring);
    cdata = read_settingsattime(timeATstring); //automatically matches the first set of characters
    if (cdata.length() > 5)
    {
      Serial.println(cdata);
      while ((cdata.indexOf('\t') > 1) && (cdata.length() > 5))
      {
        String setpointsSdata = cdata.substring(cdata.indexOf('\t') + 1);
        float T1SetPointString = setpointsSdata.substring(0, setpointsSdata.indexOf('\t')).toFloat();
        setpointsSdata = setpointsSdata.substring(setpointsSdata.indexOf('\t') + 1);
        float T2SetPointString = setpointsSdata.substring(0, setpointsSdata.indexOf('\t')).toFloat();
        setpointsSdata = setpointsSdata.substring(setpointsSdata.indexOf('\t') + 1);
        float T3SetPointString = setpointsSdata.substring(0, setpointsSdata.indexOf('\t')).toFloat();
        setpointsSdata = setpointsSdata.substring(setpointsSdata.indexOf('\t') + 1);
        float T4SetPointString = setpointsSdata.substring(setpointsSdata.indexOf('\t') + 1).toFloat();

        if ((T1SetPointString > -100) && (T1SetPointString < 1000) && (T2SetPointString > -100) && (T2SetPointString < 1000) && (T3SetPointString > -100) && (T3SetPointString < 1000) && (T4SetPointString > -100) && (T4SetPointString < 1000))
        {
          RAMP1_START_TEMP = T1SetPointString;
          RAMP2_START_TEMP = T2SetPointString;
          RAMP3_START_TEMP = T3SetPointString;
          RAMP4_START_TEMP = T4SetPointString;
          inDifferentNow = true;
Serial.print(RAMP1_START_TEMP);Serial.print("\t");
Serial.print(RAMP2_START_TEMP);Serial.print("\t");Serial.print(RAMP3_START_TEMP); Serial.print("\t");Serial.println(RAMP4_START_TEMP);             
Serial.println("Settings read: OK");
    return inDifferentNow;
        }
      }
      break;
    }
    else
    {
      nowMin--;
      if (nowMin < 0)
      {
        nowMin = 59;
        nowHour--;
        if (nowHour < 0)
        {
          nowHour = 23;
        }
      }
    }
  }
  return inDifferentNow;

}


String read_settingsattime(String timeAt)
{
  String rdata = "";
  File printFile = SD.open("Settings.ini");
  String buffer;
  if (!printFile) {
    Serial.print("The text file cannot be opened");
    return "0";
  }
  while (printFile.available())
  {
    buffer = printFile.readStringUntil('\n');
    //Serial.println(buffer); //Printing for debugging purpose
//    if (buffer.indexOf(timeAt) >= 0)
      if (buffer.indexOf(timeAt) == 0)
    {
      Serial.println("Found INI");
      printFile.close();
      return buffer;
    }
    //do some action here
  }
  printFile.close();
  return "0";

}


String getValue(String p1)
{
  if ( p1.indexOf('=') > 1  )
  {
    return p1.substring((p1.indexOf('=') + 1));
  }
  else
  {
    Serial.println("Settings.ini Error");
    Serial.println(p1);
    return "";
  }
}
