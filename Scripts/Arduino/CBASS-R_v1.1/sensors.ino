
void sensorsInit()
{
    // ***** INPUT *****
  // Start up the TempSensor library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement

  // method 1: by index
  if (!sensors.getAddress(T1Thermometer, 0)) Serial.println("Unable to find address for Tank 1");
  if (!sensors.getAddress(T2Thermometer, 1)) Serial.println("Unable to find address for Tank 2");
  if (!sensors.getAddress(T3Thermometer, 2)) Serial.println("Unable to find address for Tank 3");
  if (!sensors.getAddress(T4Thermometer, 3)) Serial.println("Unable to find address for Tank 4");
  // show the addresses we found on the bus
  Serial.print("Tank 1 Address: ");
  printAddress(T1Thermometer);
  Serial.println();

  Serial.print("Tank 2 Address: ");
  printAddress(T2Thermometer);
  Serial.println();

  Serial.print("Tank 3 Address: ");
  printAddress(T3Thermometer);
  Serial.println();

  Serial.print("Tank 4 Address: ");
  printAddress(T4Thermometer);
  Serial.println();

  // set the resolution to 12 bit
  sensors.setResolution(T1Thermometer, 12);
  sensors.setResolution(T2Thermometer, 12);
  sensors.setResolution(T3Thermometer, 12);
  sensors.setResolution(T4Thermometer, 12);
}










// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}
