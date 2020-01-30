void setup()
{
  Serial.begin(115200);
#ifdef DEBUG_ESP_PORT
  Serial.setDebugOutput(true);
#endif
  while (!Serial)
  {
    yield(); // wait for serial port to connect. Needed for native USB port only
  }
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
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  // Load filesystem
  if (SPIFFS.begin())
  {
    updateSys(); // WebUpdate

    if (SPIFFS.exists("/config.txt")) // Load configuration
    {
      loadConfig();
      // if (testModus) // Ignorieren!!!
      //   sensorValueTest = setSensorValueTest(setMode);
      setTicker();
    }
    else
      Serial.println("*** SYSINFO: Konfigurationsdatei config.txt nicht vorhanden. Setze Standardwerte ...");
  }
  else
    Serial.println("*** SYSINFO: Fehler - Dateisystem SPIFFS konnte nicht eingebunden werden!");

  Serial.print("*** SYSINFO: Verbunden mit WLAN SSID: ");
  Serial.println(WiFi.SSID());

  // Starte Webserver
  setupServer();

  //Starte mDNS Dienst
  if (startMDNS)
    setMDNS();
  else
  {
    Serial.print("*** SYSINFO: ESP8266 IP Addresse: ");
    Serial.println(WiFi.localIP().toString());
  }

  // Starte I2C
  Wire.begin();
  Wire.beginTransmission(0x27);

  // Starte Temperatursensor
  sensors.begin();
  readTemparature();
  TickerTemp.start();

  // Pin Definitionen
  if (startMV1)
  {
    digitalWrite(PIN_MV1, LOW);
    pinMode(PIN_MV1, OUTPUT); // D8
  }
  if (startMV2)
  {
    digitalWrite(PIN_MV2, LOW);
    pinMode(PIN_MV2, OUTPUT); // D0
  }
  if (startBuzzer)
    pinMode(PIN_BUZZER, OUTPUT); // D4
  
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), tick, CHANGE); // D5
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_B), tick, CHANGE); // D6

  button.attachClick(click);

  // EEPROM
  EEPROM.begin(512);
  offsetVoltage = readFloat(0); // Lese Offset (Kalibrierung)

  // Lese vom Drucksensor
  TickerPressure.start();

  // Starte Encoder
  TickerEncoder.start();
  TickerButton.start();

  // Zeitserver via NTP
  timeClient.begin();
  timeClient.update();
  
  // LCD
  startLCD();

  // Uhrzeit
  Serial.printf("*** SYSINFO: %s\n", timeClient.getFormattedTime().c_str());
  if (startBuzzer)
    sendAlarm(ALARM_ON);
  checkLog();
}

// Webserver
void setupServer()
{
  server.on("/", handleRoot);
  server.on("/reboot", rebootDevice);     // Spundomat reboot
  server.on("/kalibrieren", kalibrieren); // Spundomat Kalibrierung

  server.on("/reqMisc", handleRequestMisc); // System Infos für WebConfig
  server.on("/setMisc", handleSetMisc);     // Einstellungen ändern
  server.on("/reqMiscSet", handleRequestMiscSet);
  server.on("/reqMode", handlereqMode);           // WebIf Abfrage Modus
  server.on("/startHTTPUpdate", startHTTPUpdate); // Firmware ebUpdate
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
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  },
            handleFileUpload);

  server.onNotFound(handleWebRequests); // Sonstiges

  httpUpdate.setup(&server); // ESP8266HTTPUpdateServer
  server.begin();
}
