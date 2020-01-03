void loop()
{
  // Webserver (80)
  server.handleClient();

  // Check WiFi status
  if (WiFi.status() != WL_CONNECTED)
  {
    DBG_PRINT("*** SYSINFO: WLAN reconnect status ");
    DBG_PRINTLN(WiFi.status());
    DBG_PRINT("*** SYSINFO: WLAN IP ");
    DBG_PRINTLN(WiFi.localIP().toString());
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  }

  // Telnet
  if (startTEL)
    checkTELNET();
  // mDNS
  if (startMDNS)
     MDNS.update();

  button.tick();
  readEncoder();

  if (TickPressureOccured)
  {
    readPressure();
    TickPressureOccured = false;
  }

  if (TickTempOccured)
  {
    readTemparature();
    TickTempOccured = false;
  }

  if (reflashLCD)
  {
    showLCD();
  }

  // Betriebsmodi
  switch (setMode)
  {
  case 0: // aus
    break;
  case 1: // CO2 Spunden
    if (pressure > calcPressure(setCarbonation, temperature))
    {
      if (startMV1)
        releasePressure();
      readPressure();
    }
    break;
  case 2: // Druck Spunden
    if (pressure > setPressure)
    {
      if (startMV1)
        releasePressure();
      readPressure();
    }
    break;
  case 3: // CO2 Karbonisieren
    if (pressure < calcPressure(setCarbonation, temperature))
    {
      if (startMV2)
        buildPressure();
      readPressure();
    }
    break;
  }
  up = false;
  down = false;
}
