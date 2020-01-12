void loop()
{
  // Webserver (80)
  server.handleClient();
  // Prüfe WiFi status
  if (WiFi.status() != WL_CONNECTED)
  {
    DEBUG_MSG("*** SYSINFO: WLAN nicht verbunden: %s\n", WiFi.status());
    DEBUG_MSG("*** SYSINFO: WLAN IP %s\n", WiFi.localIP().toString().c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  }

  // mDNS
  if (startMDNS)
     MDNS.update();

  button.tick();
  readEncoder();
  timeClient.update();
  
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
      releasePressure();
      readPressure();
    }
    break;
  case 2: // Druck Spunden
    if (pressure > setPressure)
    {
      releasePressure();
      readPressure();
    }
    break;
  case 3: // CO2 Karbonisieren
    if (pressure < calcPressure(setCarbonation, temperature))
    {
      buildPressure();
      readPressure();
    }
    break;
  }
  up = false;
  down = false;
}
