void setup()
{
  Serial.begin(115200);
#ifdef DEBUG_ESP_PORT
  Serial.setDebugOutput(true);
#endif
  snprintf(esp_clientid, maxHostSign, "ESP8266-%08X", ESP.getChipId());
  Serial.printf("\n*** SYSINFO: starte Spundomat V %s Wemos D1 mini ID %s\n", Version, esp_clientid);

  // Verbinde WLAN
  wifiManager.setDebugOutput(false);
  wifiManager.setMinimumSignalQuality(10);
  wifiManager.setConfigPortalTimeout(300);
  wifiManager.setAPCallback(configModeCallback);
  WiFiManagerParameter p_hint("<small>*Sobald der Spundomat im WLAN eingebunden ist, öffne im Browser http://spundomat.local </small>");
  wifiManager.addParameter(&p_hint);
  wifiManager.autoConnect(nameMDNS);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  WiFi.persistent(true);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  // Starte I2C
  Wire.begin();
  Wire.beginTransmission(0x27);
  lcd.begin(20, 4);

  // Load filesystem
  if (LittleFS.begin())
  {
    Serial.printf("*** SYSINFO: Setup LittleFS, free heap mem: %d\n", ESP.getFreeHeap());
    // Prüfe WebUpdate
    if (LittleFS.exists(UPDATESYS))
      updateSys();
    if (LittleFS.exists(UPDATETOOLS))
      updateTools();

    if (LittleFS.exists(CONFIG)) // Load configuration
    {
      loadConfig();
    }
    else
      Serial.println("*** SYSINFO: Konfigurationsdatei config.txt nicht vorhanden. Setze Standardwerte ...");
  }
  else
    Serial.println("*** SYSINFO: Fehler - Dateisystem LittleFS konnte nicht eingebunden werden!");

  // Erstelle Ticker
  setTicker();

  // Starte Webserver
  setupServer();

  // Starte mDNS Dienst
  if (startMDNS)
    setMDNS();
  else
    Serial.printf("*** SYSINFO: ESP8266 IP Addresse: %s Time: %s RSSI: %d\n", WiFi.localIP().toString().c_str(), zeit, WiFi.RSSI());

  // Starte Temperatursensor
  sensors.begin();
  readTemparature();
  TickerTemp.start();

  // Check WLAN
  TickerWLAN.start();
  TickerWLAN.pause();

  // Pin Definitionen
  if (startMV1)
  {
    pinMode(PIN_MV1, OUTPUT); // D8
    digitalWrite(PIN_MV1, LOW);
  }
  if (startMV2)
  {
    pinMode(PIN_MV2, OUTPUT); // D0
    digitalWrite(PIN_MV2, LOW);
  }
  if (buzzer)
  {
    pinMode(PIN_BUZZER, OUTPUT); // D7
    digitalWrite(PIN_BUZZER, LOW);
  }

  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), tick, CHANGE); // D0
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_B), tick, CHANGE); // D5

  button.attachClick(click);

  // Starte Drucksensor
  TickerPressure.start();
  // Starte Display
  TickerDisplay.start();
  TickerDisplayTimer.start();
  // LCD
  startLCD();

  // Influx Datenbank
  if (startDB > 0)
    setInfluxDB();

  if (startDB > 0 && startVis)
    TickerInfluxDB.start();

  // Check WebUpdate logs
  checkLog();
}

// Webserver
void setupServer()
{
  server.on("/", handleRoot);
  server.on("/index.htm", handleRoot);
  server.on("/index", handleRoot);
  server.on("/index.html", handleRoot);
  server.on("/ablaufplan", HTTP_GET, handleAblauf);
  server.on("/ablaufplan.html", HTTP_GET, handleAblauf);
  server.on("/ablaufplan.htm", HTTP_GET, handleAblauf);
  server.on("/splan", HTTP_GET, handleFermenter);
  server.on("/splan.html", HTTP_GET, handleFermenter);
  server.on("/splan.htm", HTTP_GET, handleFermenter);
  server.on("/reboot", rebootDevice);         // Spundomat reboot
  server.on("/kalibrieren", kalibrieren);     // Spundomat Kalibrierung
  server.on("/visualisieren", visualisieren); // Spundomat Visualisierung
  server.on("/reqFirm", handleRequestFirm);   // System Infos für WebConfig
  server.on("/reqMisc", handleRequestMisc);   // System Infos für WebConfig
  server.on("/setMisc", handleSetMisc);       // Einstellungen ändern
  server.on("/setMisc2", handleSetMisc2);       // Einstellungen ändern
  server.on("/reqMiscSet", handleRequestMiscSet);
  server.on("/reqSud", handleRequestSudID);
  server.on("/reqMode", handlereqMode);       // WebIf Abfrage Modus
  server.on("/reqVisual", handlereqVisual);       // WebIf Abfrage Modus
  server.on("/reqEinheit", handlereqEinheit); // WebIf Abfrage Einheit Zeiteingabe
  server.on("/reqPlan1", handleRequestPlan1);
  server.on("/reqPlan2", handleRequestPlan2);
  server.on("/reqPlan3", handleRequestPlan3);
  server.on("/setPlan", handleSetPlan);
  // server.on("/setPlan1", handleSetPlan1);
  // server.on("/setPlan2", handleSetPlan2);
  // server.on("/setPlan3", handleSetPlan3);
  server.on("/reqOG", readSteuerOG);
  server.on("/reqUG", readSteuerUG);
  server.on("/setFerm", handleSetFerm);
  server.on("/reqName", handleRequestName);
  server.on("/startHTTPUpdate", startHTTPUpdate); // Firmware WebUpdate
  // FSBrowser initialisieren
  server.on("/edit", HTTP_GET, handleGetEdit);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/edit", HTTP_PUT, handleFileCreate);
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  server.on(
      "/edit", HTTP_POST, []()
      { server.send(200, "text/plain", ""); loadConfig(); },
      handleFileUpload);
  server.on(
      "/restore", HTTP_POST, []()
      { server.send(200, "text/plain", ""); },
      handleRestore);
  server.onNotFound(handleWebRequests);
  httpUpdate.setup(&server);  // DateiUpdate

  server.on("/spundomat.min.css", handleWebRequests);
  server.on("/spundomat.min.js", handleWebRequests);
  server.on("/spundomat.ttf", handleWebRequests);
  server.on("/config.txt", handleWebRequests);
  server.on("/favicon.ico", handleWebRequests);
  server.serveStatic("/spundomat.min.css", LittleFS, "/spundomat.min.css", "no-store, must-revalidate");
  server.serveStatic("/spundomat.min.js", LittleFS, "/spundomat.min.js", "no-store, must-revalidate");
  server.serveStatic("/spundomat.ttf", LittleFS, "/spundomat.ttf", "no-cache, no-store, must-revalidate");
  server.serveStatic("/config.txt", LittleFS, "/config.txt", "no-cache, no-store, must-revalidate");

  server.begin();
}
