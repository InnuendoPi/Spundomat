void loop()
{
  // Webserver (80)
  server.handleClient();

  // Prüfe WiFi status
  if (WiFi.status() != WL_CONNECTED)
  {
    if (wlanState)
    {
      DEBUG_MSG("%s", "*** SYSINFO: WLAN loop nicht verbunden\n");
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);
      wlanState = false;
      TickerWLAN.start();
    }
    else
      TickerWLAN.update();
  }

  // Check mDNS
  if (startMDNS)
    MDNS.update();

  // Check NTP
  timeClient.update();

  // Check DS18B20 Ticker
  TickerTemp.update();

  // Check Encoder
  button.tick();
  readEncoder();

  // Check CO2 Sensor
  if (startCO2)
    TickerCO2.update();

  // InfluxDB Ticker
  if (startDB && startVis)
    TickerInfluxDB.update();

  if (reflashLCD || TickerDisplay.getUp())
    TickerDisplay.updateNow();

  // Betriebsmodi
  switch (setMode)
  {
  case AUS: // aus
    TickerPressure.update();
    break;
  case SPUNDEN_CO2: // CO2 Spunden
    updateMV1();
    break;
  case SPUNDEN_DRUCK: // Druck Spunden
    updateMV1();
    break;
  case KARBONISIEREN_CO2: // CO2 Karbonisieren
    updateMV2();
    break;
  case KARBONISIEREN_DRUCK: // CO2 Karbonisieren
    updateMV2();
    break;
  case SPUNDOMAT: // CO2 Spunden & Karbonisieren
    updateSpundomat();
    break;
  case PLAN1:
  case PLAN2:
  case PLAN3:
    startPlan();
    break;
  case DICHTHEIT:
    checkDichtheit();
    break;
  }
  up = false;
  down = false;
}
