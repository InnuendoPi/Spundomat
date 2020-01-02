void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    yield(); // wait for serial port to connect. Needed for native USB port only
  }

  // Connect WiFi
  wifiManager.autoConnect("SpundomatAP");
  
  if (!SPIFFS.begin())
  {
    Serial.println("*** SYSINFO: Fehler - Dateisystem SPIFFS konnte nicht eingebunden werden!");
  }
  else
  {
    DBG_PRINTLN("");
    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_PRINT("FS File: ");
      DBG_PRINT(fileName.c_str());
      DBG_PRINT(" size: ");
      DBG_PRINTLN(formatBytes(fileSize).c_str());
    }
  }

  // Load configuration
  if (SPIFFS.exists("/config.json"))
    loadConfig();
  else
    Serial.println("*** SYSINFO: Konfigurationsdatei config.json nicht vorhanden. Setze Standardwerte ...");

  // Starte Telnet Server
  if (startTEL)
    setTELNET();
  // Starte mDNS Dienst
  if (startMDNS)
    setMDNS();

  // Starte Webserver
  setupServer();

  //Start I2C
  Wire.begin();
  Wire.beginTransmission(0x27);

  //Start Temperatursensor
  sensors.begin();

  //Pin Definitionen
  pinMode(PIN_VALVE, OUTPUT);       // D8
  pinMode(PIN_BUZZER, OUTPUT);      // D0

  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), tick, CHANGE);  // D5
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_B), tick, CHANGE);  // D6

  button.attachClick(click);

  //EEPROM
  EEPROM.begin(512);
  offsetVoltage = readFloat(0);     // Lese Offset (Kalibrierung)

  //LCD
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.print(" Spundomat V ");
  lcd.print(Version);
  millis2wait(PAUSE2SEC);
  lcd.setCursor(0, 1);
  lcd.print(" 2020 - Innuendo");
  millis2wait(PAUSE2SEC);

  readTemparature();

  // Timer Temperatur einlesen
  os_timer_setfn(&TimerTemp, timerTempCallback, NULL);
  os_timer_arm(&TimerTemp, 30000, true);                // Zeitintervall Temperatursensor 30sek
  // Timer Druck einlesen
  os_timer_setfn(&TimerPressure, timerPressureCallback, NULL);
  os_timer_arm(&TimerPressure, 1000, true);             // Zeitintervall Drucksensor 1sek
}

// Webserver
void setupServer()
{
  server.on("/", handleRoot);
  server.on("/reboot", rebootDevice);     // Spundomat reboot
  server.on("/kalibrieren", kalibrieren); // Spundomat reboot

  server.on("/reqMisc", handleRequestMisc); // System Infos für WebConfig
  server.on("/setMisc", handleSetMisc);     // Einstellungen ändern
  server.on("/reqMiscSet", handleRequestMiscSet);
  server.on("/reqMode", handlereqMode);

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
