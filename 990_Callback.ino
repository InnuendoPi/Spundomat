void tickerTempCallback() // Timer Objekt Temperatur
{
  readTemparature();
}

void tickerInfluxDBCallback() // Timer Objekt Influx Datenbank
{
  sendDBData();
}

void tickerPressureCallback() // Timer Objekt Druck
{
  readPressure();
}

void tickerDisplayCallback() // Timer Objekt Druck
{
  showLCD();
}

void tickerWLANCallback() // Timer Objekt Druck
{
  WiFi.mode(WIFI_OFF);
  DEBUG_MSG("%s", "*** SYSINFO: debug WLAN disconnected\n");
}
