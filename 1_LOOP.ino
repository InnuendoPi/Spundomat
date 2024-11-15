void loop()
{
  // Webserver (80)
  server.handleClient();

  // Prüfe WiFi status
  if (WiFi.status() == WL_CONNECTED)
  {
    if (startMDNS) // mDNS handle
    {
      MDNS.update();
    }
    TickerTime.update(); // Zeitserver
  }
  else
  {
    EM_WLAN(); // WLAN reconnect
  }

  // Check DS18B20 Ticker
  TickerTemp.update();

  // Check Encoder
  button.tick();
  readEncoder();

  // InfluxDB Ticker
  if (startDB > 0 && startVis)
  {
    TickerInfluxDB.update();
  }

  if (useBacklightTimer)
    TickerDisplayTimer.update();
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
