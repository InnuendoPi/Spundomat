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

void tickerSteuerungCallback()
{
  steuerung();
}

void tickerConCallback()
{
  if (counterCon < maxCon - 1)
  {
    counterCon++;
    checkTemp = false;
    //DEBUG_MSG("Callback: TickerCon Update zieltemp %f timer %d counter %d\n", targetTemp, structCon[counterCon].timer, counterCon);
    startCon();
  }
}

void tickerAlarmierungCallback()
{
  float diff = targetTemp - temperature;
  if (abs(diff) > 0.5) // Differenz zur Zieltemperatur
  {
    float diff2 = alarmTemperature - temperature;
    if (abs(diff2) < 0.02) // Differenz Änderung zur Temperatur vor <ZUSATZALARM>ms
    {
      if (setGPIO == 1)
        sendAlarm(ALARM_ERROR);
      DEBUG_MSG("*** SYSINFO: Temperaturdifferenz sehr gering %f\n", diff2);
    }
  }
  else
  {
    DEBUG_MSG("*** SYSINFO: Temperatur Alarm %f Temp %f\n", alarmTemperature, temperature);
  }
  alarmTemperature = temperature;
}