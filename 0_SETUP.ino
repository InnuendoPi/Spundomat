void setup()
{
  Serial.begin(115200);
#ifdef DEBUG_ESP_PORT
  Serial.setDebugOutput(true);
#endif
  Serial.println("");
  Serial.println("");
  Serial.println("*** SYSINFO: Starte Setup Spundomat");

  // Verbinde WLAN
  wifiManager.setDebugOutput(false);
  wifiManager.setMinimumSignalQuality(10);
  wifiManager.setConfigPortalTimeout(300);
  wifiManager.setAPCallback(configModeCallback);
  WiFiManagerParameter p_hint("<small>*Sobald der Spundomat im WLAN eingebunden ist, öffne im Browser http://spundomat (mDNS)</small>");
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
    updateSys();                        // WebUpdate
    EEPROM.begin(512);                  // EEProm Zugriff
    if (LittleFS.exists("/config.txt")) // Load configuration
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

  // Zeitserver via NTP
  timeClient.begin();
  timeClient.forceUpdate();
  checkSummerTime();

  // Starte Webserver
  setupServer();

  // Starte mDNS Dienst
  if (startMDNS)
    setMDNS();
  else
  {
    Serial.printf("*** SYSINFO: ESP8266 IP Addresse: %s Time: %s RSSI: %d\n", WiFi.localIP().toString().c_str(), timeClient.getFormattedTime().c_str(), WiFi.RSSI());
  }

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
  if (setGPIO > 0) // 0 = Aus 1 = Buzzer 2 = Ventilator
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
  // LCD
  startLCD();

  // Influx Datenbank
  if (startDB)
    setInfluxDB();

  if (startDB && startVis)
    TickerInfluxDB.start();

  // Check Update logs
  checkLog();
}

// Webserver
void setupServer()
{
  server.on("/", handleRoot);
  server.on("/index.htm", handleRoot);
  server.on("/reboot", rebootDevice);         // Spundomat reboot
  server.on("/kalibrieren", kalibrieren);     // Spundomat Kalibrierung
  server.on("/visualisieren", visualisieren); // Spundomat Visualisierung
  server.on("/reqFirm", handleRequestFirm);   // System Infos für WebConfig
  server.on("/reqMisc", handleRequestMisc);   // System Infos für WebConfig
  server.on("/setMisc", handleSetMisc);       // Einstellungen ändern
  server.on("/reqMiscSet", handleRequestMiscSet);
  server.on("/reqMode", handlereqMode);       // WebIf Abfrage Modus
  server.on("/reqEinheit", handlereqEinheit); // WebIf Abfrage Einheit Zeiteingabe
  server.on("/reqGPIO", handlereqGPIO);       // WebIf Abfrage Modus GPIO D7
  server.on("/reqPlan1", handleRequestPlan1);
  server.on("/reqPlan2", handleRequestPlan2);
  server.on("/reqPlan3", handleRequestPlan3);
  server.on("/setPlan1", handleSetPlan1);
  server.on("/setPlan2", handleSetPlan2);
  server.on("/setPlan3", handleSetPlan3);
  server.on("/reqName", handleRequestName);
  server.on("/startHTTPUpdate", startHTTPUpdate); // Firmware WebUpdate
                                                  // FSBrowser initialisieren
  server.on("/edit", HTTP_GET, handleGetEdit);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/edit", HTTP_PUT, handleFileCreate);
  // server.on("/favicon.ico", HTTP_GET, replyOK);
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  server.on(
      "/edit", HTTP_POST, []()
      { server.send(200, "text/plain", ""); loadConfig(); },
      handleFileUpload);
  server.onNotFound(handleWebRequests);
  httpUpdate.setup(&server);
  server.begin();
}
