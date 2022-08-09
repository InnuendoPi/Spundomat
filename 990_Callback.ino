void tickerTempCallback() // Timer Objekt Temperatur
{
  readTemparature();
}

void tickerInfluxDBCallback() // Timer Objekt Influx Datenbank
{
  if (checkDBConnect()) // if checkDBConnect is successful it sets visState to 0
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
  WiFi.reconnect();
  if (WiFi.status() == WL_CONNECTED)
  {
    DEBUG_MSG("*** SYSINFO: WLAN reconnect IP %s\n", WiFi.localIP().toString().c_str());
    wlanState = true;
    TickerWLAN.stop();
  }
  else
  {
    wlanState = false;
    DEBUG_MSG("%s", "*** SYSINFO: WLAN not connected\n");
    sendAlarm(ALARM_ERROR);
  }
}
