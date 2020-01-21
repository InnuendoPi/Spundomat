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

  // Check NTP Ticker
  TickerNTP.update();

  // Check Encoder Ticker
  TickerButton.update();
  TickerEncoder.update();

  // Check DS18B20 Ticker
  TickerTemp.update();

  // Check Drucksensor Ticker
  TickerPressure.update();

  if (reflashLCD)
    showLCD();

  // Betriebsmodi
  switch (setMode)
  {
  case 0: // aus
    break;
  case 1: // CO2 Spunden
    TickerMV1.update();
    break;
  case 2: // Druck Spunden
    TickerMV1.update();
    break;
  case 3: // CO2 Karbonisieren
    TickerMV2.update();
    break;
  }
  up = false;
  down = false;
}
