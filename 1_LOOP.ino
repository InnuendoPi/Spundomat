unsigned long nextResetTime=0;
void loop()
{
  // Webserver (80)
  server.handleClient();

  // Prüfe WiFi status
  if ((WiFi.status() != WL_CONNECTED) ||  (!WiFi.localIP().isSet()))
  {
    DEBUG_MSG("*** SYSINFO: WLAN nicht verbunden: %s\n", WiFi.status());
    DEBUG_MSG("*** SYSINFO: WLAN IP %s\n", WiFi.localIP().toString().c_str());
    if (nextResetTime==0) nextResetTime=millis()+30*60*1000;
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  } else {
    nextResetTime=0;
  }
  if ((millis() > nextResetTime) && (nextResetTime>0)) {
    SPIFFS.end(); // unmount SPIFFS
    ESP.restart();
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
