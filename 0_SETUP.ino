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

  // Load filesystem
  if (LittleFS.begin())
  {
    updateSys();                      // WebUpdate
    if (LittleFS.exists("/config.txt")) // Load configuration
      loadConfig();
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

  //Starte mDNS Dienst
  if (startMDNS)
    setMDNS();
  else
  {
    Serial.printf("*** SYSINFO: ESP8266 IP Addresse: %s Time: %s RSSI: %d\n", WiFi.localIP().toString().c_str(), timeClient.getFormattedTime().c_str(), WiFi.RSSI());
  }

  // Starte I2C
  Wire.begin();
  Wire.beginTransmission(0x27);

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

  // EEPROM
  EEPROM.begin(512);
  // writeFloat(0, 121.0);
  // writeFloat(4, 362.0);
  offset0 = readFloat(0); // Lese Offset (Kalibrierung)
  offset2 = readFloat(4); // Lese Offset (Kalibrierung)

  // Starte Drucksensor
  TickerPressure.start();
  // Starte Display
  TickerDisplay.start();

  if (setMode == STEUERUNG)
  {
    TickerSteuerung.start();
    TickerAlarmierung.start();
    steuerung();
  }
  if (setMode == CON1 || setMode == CON2)
  {
    TickerCon.start();
    TickerSteuerung.start();
    TickerAlarmierung.start();
    startCon();
    steuerung();
  }

  // LCD
  startLCD();

  // Influx Datenbank
  if (startDB)
    setInfluxDB();

  if (startDB && startVis)
    TickerInfluxDB.start();
  
  if (startCO2)
  {
    co2Serial.begin(9600);
    myMHZ19.begin(co2Serial);
    initCO2();
    TickerCO2.start();
  }

  // Check Update logs
  checkLog();
}

// Webserver
void setupServer()
{
  server.on("/", handleRoot);
  server.on("/reboot", rebootDevice);         // Spundomat reboot
  server.on("/kalibrieren", kalibrieren);     // Spundomat Kalibrierung
  server.on("/visualisieren", visualisieren); // Spundomat Visualisierung
  server.on("/reqFirm", handleRequestFirm); // System Infos für WebConfig
  server.on("/reqMisc", handleRequestMisc); // System Infos für WebConfig
  server.on("/setMisc", handleSetMisc);     // Einstellungen ändern
  server.on("/reqMiscSet", handleRequestMiscSet);
  server.on("/reqMode", handlereqMode);           // WebIf Abfrage Modus
  server.on("/reqEinheit", handlereqEinheit);     // WebIf Abfrage Einheit Zeiteingabe
  server.on("/reqGPIO", handlereqGPIO);           // WebIf Abfrage Modus GPIO D7
  server.on("/startHTTPUpdate", startHTTPUpdate); // Firmware WebUpdate
  // FSBrowser initialisieren
  server.on("/list", HTTP_GET, handleFileList); // list directory
  server.on("/edit", HTTP_GET, []() {           // load editor
    if (!handleFileRead("/edit.htm"))
    {
      server.send(404, "text/plain", "FileNotFound");
    }
  });
  server.on("/edit", HTTP_PUT, handleFileCreate);    // create file
  server.on("/edit", HTTP_DELETE, handleFileDelete); // delete file
  server.on(
      "/edit", HTTP_POST, []() {
        server.send(200, "text/plain", "");
      },
      handleFileUpload);

  server.onNotFound(handleWebRequests); // Sonstiges

  httpUpdate.setup(&server); // ESP8266HTTPUpdateServer
  server.begin();
}
