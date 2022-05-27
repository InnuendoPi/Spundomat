// Verkabelung CO2 Sensors MH-Z19b an Wemos D1 mini
// MH-Z19b       Wemos D1 mini
// 3 TX sw       D2 SDA
// 2 RX gelb     D1 SDL
// 6 VIN grün    5V
// 7 GND blau    GND

/*

Deaktiviert

void initCO2()
{
  DEBUG_MSG("%s", "Starte CO2 Sensor\n");
  co2Serial.begin(9600);
  myMHZ19.begin(co2Serial);
  myMHZ19.setRange(2000);
  // myMHZ19.calibrateZero();
  // myMHZ19.setSpan(2000);
  myMHZ19.zeroSpan(2000);
  myMHZ19.autoCalibration(true, 24);
  if (myMHZ19.getABC())
  {
    DEBUG_MSG("ABC Status: %s\n", "ON");
  }
  else
  {
    DEBUG_MSG("ABC Status: %s\n", "OFF");
  }

  // Auto Calibration: 
  // The MH-Z19 is a sensor that is designed to be powered on and rarely turned off. 
  // The sensor calibrates over time (if autocalibration is on) tuning the Zero at the end of each "ABC" (auto calibration) 
  // period (0 - 24hrs currently). After 3 weeks, a value is stored to represent it's accuracy, 
  // this can be requested using getAccuracy(). ABC must be disabled by sending the appropriate command 
  // before the end of the ABC period to ensure it remains off - this is handled by the library.

  // myMHZ19.getABC() ? Serial.println("ON") : Serial.println("OFF");
  // myMHZ19.printCommunication(true, true);

  char myVersion[4];
  myMHZ19.getVersion(myVersion);
  
  String firm = "";
  for (byte i = 0; i < 4; i++)
  {
    firm += myVersion[i];
    if (i == 1)
      firm += ".";
  }
  DEBUG_MSG("CO2 Sensor Firmware Version: %s\n", firm.c_str());
  DEBUG_MSG("CO2 Sensor Range: %d\n", myMHZ19.getRange());
  DEBUG_MSG("CO2 Sensor Background CO2: %d\n", myMHZ19.getBackgroundCO2());

  myMHZ19.setFilter(true, true);

  wertCO2 = myMHZ19.getCO2();
  DEBUG_MSG("CO2 %d ppm\n", wertCO2);
  // DEBUG_MSG("Temp %f °C\n", myMHZ19.getTemperature());
}

void leseCO2()
{
  co2Serial.begin(9600);
  myMHZ19.begin(co2Serial);
  // myMHZ19.setRange(2000);
  int CO2 = myMHZ19.getCO2();
  if (CO2 != 0)
    wertCO2 = CO2;
  else
  {
    byte thisCode = myMHZ19.errorCode;
    if (thisCode != RESULT_OK)
    {
      if (thisCode == RESULT_FILTER) // was it the filter ?
      {
        DEBUG_MSG("%s", "*** Filter was triggered ***\n");
        DEBUG_MSG("Offending Value: %d\n", CO2);
      }
      else
      {
        DEBUG_MSG("Communication Error Found. Error Code: %d\n", thisCode);
      }
    }
  }
}
*/
