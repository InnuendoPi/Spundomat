void tickerTempCallback() // Timer Objekt Temperatur
{
  readTemparature();
}

void tickerInfluxDBCallback() // Timer Objekt Influx Datenbank
{
  if (wlanState)
    sendDBData();
  else
    DEBUG_MSG("%s", "*** SYSINFO: sending Influx data skipped: WLAN not connected\n");
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

void tickerCO2Callback()
{
  leseCO2();
  if (wertCO2 < 1000)
  {
    // grün
    DEBUG_MSG("*** SYSINFO: CO2-Wert grün %d\n", wertCO2);
    if (setGPIO == 2)
    {
      DEBUG_MSG("%s", "*** SYSINFO: D7 Low\n");
      digitalWrite(PIN_BUZZER, LOW);
    }
  }
  else if (wertCO2 < 2000)
  {
    // gelb
    DEBUG_MSG("*** SYSINFO: CO2-Wert gelb %d\n", wertCO2);
    if (setGPIO == 1)
    {
      DEBUG_MSG("%s", "*** SYSINFO: Warning\n");
      sendAlarm(ALARM_WARNING);
    }
    else if (setGPIO == 2)
    {
      DEBUG_MSG("%s", "*** SYSINFO: D7 High\n");
      digitalWrite(PIN_BUZZER, HIGH);
    }
  }
  else
  {
    // rot
    DEBUG_MSG("*** SYSINFO: CO2-Wert rot %d\n", wertCO2);
    if (setGPIO == 1)
    {
      DEBUG_MSG("%s", "*** SYSINFO: Alarm\n");
      sendAlarm(ALARM_ERROR);
    }
    else if (setGPIO == 2)
    {
      DEBUG_MSG("%s", "*** SYSINFO: D7 High\n");
      digitalWrite(PIN_BUZZER, HIGH);
    }
  }
}
