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

  // Check mDNS
  if (startMDNS)
    MDNS.update();

  // Check NTP
  timeClient.update();

  // Check Encoder Ticker
  TickerButton.update();
  TickerEncoder.update();

  // Check DS18B20 Ticker
  TickerTemp.update();

  // Check Drucksensor Ticker
  TickerPressure.update();

  // Check Piezzo Ticker
  // if (startBuzzer)
  //   TickerPiezzo.update();

  if (reflashLCD)
    showLCD();

  // Betriebsmodi
  switch (setMode)
  {
  case AUS: // aus
    break;
  case SPUNDEN_CO2: // CO2 Spunden
    updateMV1();
    break;
  case SPUNDEN_DRUCK: // Druck Spunden
    updateMV1();
    break;
  case KARBONISIEREN: // CO2 Karbonisieren
    updateMV2();
    break;
  }
  up = false;
  down = false;
}
