bool loadConfig()
{
  Serial.println("------ loadConfig started ------");
  File configFile = LittleFS.open("/config.txt", "r");
  if (!configFile)
  {
    Serial.println("*** SYSINFO: Fehler beim Laden der Konfiguration");
    Serial.println("------ loadConfig aborted -------");
    return false;
  }

  size_t size = configFile.size();
  if (size > 2048)
  {
    Serial.print("*** SYSINFO: Konfigurationsdatei zu groß");
    Serial.println("------ loadConfig aborted -------");
    if (setGPIO == 1)
      sendAlarm(ALARM_ERROR);
    return false;
  }

  DynamicJsonDocument doc(2500);
  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
  {
    Serial.print("Conf: Error Json ");
    Serial.println(error.c_str());
    if (setGPIO == 1)
      sendAlarm(ALARM_ERROR);
    return false;
  }
  // Spundomat Einstellungen
  JsonArray spundArray = doc["SPUNDOMAT"];
  JsonObject spundObj = spundArray[0];
  setPressure = spundObj["PRESSURE"] | DEF_PRESSURE;
  setCarbonation = spundObj["CARBONATION"] | DEF_CARB;
  setMode = spundObj["MODE"] | AUS;
  newMode = setMode;
  verzKombi = spundObj["VERZKOMBI"] | 0.0;
  setEinheit = spundObj["EINHEIT"] | 0;

  Serial.printf("Modus: %d\n", setMode);
  Serial.printf("Zielwert Druck: %2.2f\n", setPressure);
  Serial.printf("Zielwert CO2 Gehalt: %2.2f\n", setCarbonation);
  Serial.printf("Verzögerung Karbonisierung: %.2f\n", verzKombi);
  Serial.printf("Einheit Verzögerung: %d\n", setEinheit);

  calcVerzSpundomat(); // Berechne Verzögerung Karbonisierung im Kombi-Modus
  Serial.printf("Verzögerung: %d minCarb: %2.2f\n", verzKarbonisierung, minKarbonisierung);
  Serial.println("--------");

  // Hardware Einstellungen
  JsonArray hwArray = doc["HARDWARE"];
  JsonObject hwObj = hwArray[0];
  startMV1 = hwObj["MV1"] | 0;
  startMV2 = hwObj["MV2"] | 0;
  setGPIO = hwObj["GPIO"] | 0;
  mv1Open = hwObj["MV1OPEN"] | DEFAULT_OPEN;
  mv1Close = hwObj["MV1CLOSE"] | DEFAULT_CLOSE;
  mv2Open = hwObj["MV2OPEN"] | DEFAULT_OPEN;
  mv2Close = hwObj["MV2CLOSE"] | DEFAULT_CLOSE;
  senOffset = hwObj["SENOFFSET"] | 0.0;
  Serial.printf("MV1 Spunden:        %d Open: %d Close: %d\n", startMV1, mv1Open, mv1Close);
  Serial.printf("MV2 Karbonisieren:  %d Open: %d Close: %d\n", startMV2, mv2Open, mv2Close);
  Serial.printf("Modus GPIO: %d\n", setGPIO);
  Serial.printf("Sensor Offset: %.2f\n", senOffset);
  Serial.println("--------");

  // System Einstellungen
  JsonArray miscArray = doc["MISC"];
  JsonObject miscObj = miscArray[0];
  upPressure = miscObj["UPPRESSURE"] | PRESSURE_UPDATE;
  upTemp = miscObj["UPTEMP"] | TEMPERATUR_UPDATE;
  startMDNS = miscObj["MDNS"] | 0;
  strlcpy(nameMDNS, miscObj["NAMEMDNS"] | "spundomat", maxHostSign);

  Serial.printf("Intervall Drucksensor: %d\n", upPressure);
  Serial.printf("Intervall Temperatursensor: %d\n", upTemp);
  Serial.printf("Aktiviere mDNS: %d\n", startMDNS);
  Serial.printf("mDNS Name: %s\n", nameMDNS);

  // EEPROM
  offset0 = readFloat(0); // Lese Offset (Kalibrierung)
  offset2 = readFloat(4); // Lese Offset (Kalibrierung)
  testModus = miscObj["TESTMODE"] | 0;
  Serial.printf("Testmodus: %d\n", testModus);
  if (testModus)
  {
    ergDichtheit = 0.0;
    if (offset0 == 0.0 || offset2 == 0.0)
    {
      offset0 = 121.0; // readFloat(0);           // Lese Offset (Kalibrierung)
      offset2 = 362.0; // readFloat(4); // Lese Offset (Kalibrierung)
    }
    Serial.printf("Testmodus Offset 0bar: %.2f\n", offset0);
    Serial.printf("Testmodus Offset 2bar: %.2f\n", offset2);
    Serial.printf("Testmodus Dichtheit: %.2f\n", ergDichtheit);
  }
  else
  {
    Serial.printf("Eeprom Offset 0bar: %.2f\n", offset0);
    Serial.printf("Eeprom Offset 2bar: %.2f\n", offset2);
    Serial.printf("Dichtheit: %.2f\n", ergDichtheit);
  }
  Serial.println("--------");

  // InfluxDB Einstellungen
  JsonArray databaseArray = doc["DATABASE"];
  JsonObject databaseObj = databaseArray[0];
  startDB = databaseObj["STARTDB"] | 0;
  strlcpy(dbServer, databaseObj["DBSERVER"] | "", MAXHOSTSIGN);
  strlcpy(dbDatabase, databaseObj["DB"] | "", MAXDBSSIGN);
  strlcpy(dbUser, databaseObj["DBUSER"] | "", MAXDBSSIGN);
  strlcpy(dbPass, databaseObj["DBPASS"] | "", MAXDBSSIGN);
  upInflux = databaseObj["DBUP"] | DB_UPDATE;

  Serial.printf("Visualisierung aktiviert: %d\n", startDB);
  Serial.printf("DB Server: %s\n", dbServer);
  Serial.printf("DB Name: %s\n", dbDatabase);
  Serial.printf("DB Benutzer: %s\n", dbUser);
  Serial.printf("DB Password: %s\n", dbPass);
  Serial.printf("Update Interval DB: %d\n", upInflux); // in Minuten
  Serial.println("--------");

  JsonArray visArray = doc["VISUALISIERUNG"];
  JsonObject visObj = visArray[0];

  startVis = visObj["VISSTARTED"] | 0;
  strlcpy(dbVisTag, visObj["SUDID"] | "", MAXDBSSIGN);

  Serial.printf("Visualisierung gestartet: %d ", startVis);
  Serial.printf("Sud-Id: %s\n", dbVisTag);

  DEBUG_MSG("%s\n", "--------");
  JsonArray ablaufArray = doc["ABLAUF"];
  JsonObject ablaufObj = ablaufArray[0];
  modesWeb[PLAN1] = ablaufObj["P1S"] | "Plan #1";
  modes[PLAN1] = ablaufObj["P1S"] | "Plan #1";
  modesWeb[PLAN2] = ablaufObj["P2S"] | "Plan #2";
  modes[PLAN2] = ablaufObj["P2S"] | "Plan #2";
  modesWeb[PLAN3] = ablaufObj["P3S"] | "Plan #3";
  modes[PLAN3] = ablaufObj["P3S"] | "Plan #3";
  DEBUG_MSG("P1S %s\n", modesWeb[PLAN1].c_str());
  DEBUG_MSG("P2S %s\n", modesWeb[PLAN2].c_str());
  DEBUG_MSG("P3S %s\n", modesWeb[PLAN3].c_str());

  Serial.println("------ loadConfig finished ------");
  configFile.close();
  size_t len = measureJson(doc);
  DEBUG_MSG("*** SYSINFO: JSON Konfiguration Größe: %d\n", len);
  if (len > 2048)
    Serial.println("*** SYSINFO: Fehler JSON Konfiguration zu groß!");

  mv1.change(mv1Open, mv1Close, startMV1);
  mv2.change(mv2Open, mv2Close, startMV2);
  mv1.switchOff();
  mv2.switchOff();

  // Ablaufpläne
  initAblaufplan(); // Initialisiere Strukturen
  TickerPressure.interval(upPressure);
  TickerInfluxDB.interval(upInflux);
  TickerTemp.interval(upTemp);

  if (setGPIO == 1 && setMode == AUS)
    sendAlarm(ALARM_OK);
  else if (setGPIO == 1)
    sendAlarm(ALARM_ON);

  return true;
}

bool saveConfig()
{
  DEBUG_MSG("%s\n", "------ saveConfig started -------");
  DynamicJsonDocument doc(2500);

  // Spundomat Einstellungen
  JsonArray spundArray = doc.createNestedArray("SPUNDOMAT");
  JsonObject spundObj = spundArray.createNestedObject();
  spundObj["PRESSURE"] = setPressure;
  spundObj["CARBONATION"] = setCarbonation;
  spundObj["MODE"] = setMode;
  spundObj["VERZKOMBI"] = verzKombi;
  spundObj["EINHEIT"] = setEinheit;

  DEBUG_MSG("setPressure: %2.2f\n", setPressure);
  DEBUG_MSG("setCarbonation: %2.2f\n", setCarbonation);
  DEBUG_MSG("verzKombi: %d\n", verzKombi);
  DEBUG_MSG("Einheit: %d\n", setEinheit);
  calcVerzSpundomat(); // Berechne Verzögerung Karbonisierung im Kombi-Modus
  DEBUG_MSG("Verzögerung: %d minCarb: %2.2f\n", verzKarbonisierung, minKarbonisierung);
  DEBUG_MSG("%s\n", "--------");

  // Hardware Einstellungen
  JsonArray hwArray = doc.createNestedArray("HARDWARE");
  JsonObject hwObj = hwArray.createNestedObject();
  hwObj["MV1"] = (int)startMV1;
  hwObj["MV2"] = (int)startMV2;
  hwObj["MV1OPEN"] = mv1Open;
  hwObj["MV1CLOSE"] = mv1Close;
  hwObj["MV2OPEN"] = mv2Open;
  hwObj["MV2CLOSE"] = mv2Close;
  hwObj["SENOFFSET"] = (int(senOffset * 100)) / 100.0;

  hwObj["GPIO"] = setGPIO;
  DEBUG_MSG("MV1: %d Open: %d Close %d\n", startMV1, mv1Open, mv1Close);
  DEBUG_MSG("MV2: %d Open: %d Close %d\n", startMV2, mv2Open, mv2Close);
  DEBUG_MSG("GPIO: %d\n", setGPIO);
  DEBUG_MSG("Sensor Offset: %.2f\n", senOffset);
  DEBUG_MSG("%s\n", "--------");

  // Datenbank Einstellungen
  JsonArray databaseArray = doc.createNestedArray("DATABASE");
  JsonObject databaseObj = databaseArray.createNestedObject();

  databaseObj["STARTDB"] = (int)startDB;
  databaseObj["DBSERVER"] = dbServer;
  databaseObj["DB"] = dbDatabase;
  databaseObj["DBUSER"] = dbUser;
  databaseObj["DBPASS"] = dbPass;
  databaseObj["DBUP"] = upInflux;
  DEBUG_MSG("Database aktiviert: %d\n", startDB);
  DEBUG_MSG("DB Server: %s\n", dbServer);
  DEBUG_MSG("DB: %s\n", dbDatabase);
  DEBUG_MSG("DB User: %s\n", dbUser);
  DEBUG_MSG("DB Pass: %s\n", dbPass);
  DEBUG_MSG("DB Update: %d\n", upInflux);
  DEBUG_MSG("%s\n", "--------");

  // System Einstellungen
  JsonArray miscArray = doc.createNestedArray("MISC");
  JsonObject miscObj = miscArray.createNestedObject();

  miscObj["NAMEMDNS"] = nameMDNS;
  miscObj["MDNS"] = (int)startMDNS;
  miscObj["UPPRESSURE"] = upPressure;
  miscObj["UPTEMP"] = upTemp;
  miscObj["TESTMODE"] = (int)testModus;

  DEBUG_MSG("Interval Drucksensor: %d\n", upPressure);
  DEBUG_MSG("Interval Temperatursensor: %d\n", upTemp);
  DEBUG_MSG("nameMDNS: %s\n", nameMDNS);
  DEBUG_MSG("startMDNS: %d\n", startMDNS);
  DEBUG_MSG("setMode: %d\n", setMode);
  DEBUG_MSG("%s\n", "--------");

  JsonArray visArray = doc.createNestedArray("VISUALISIERUNG");
  JsonObject visObj = visArray.createNestedObject();
  visObj["VISSTARTED"] = (int)startVis;
  visObj["SUDID"] = dbVisTag;
  DEBUG_MSG("Visualisierung: %d\n", startVis);
  DEBUG_MSG("Sud-Id: %s\n", dbVisTag);

  JsonArray ablaufArray = doc.createNestedArray("ABLAUF");
  JsonObject ablaufObj = ablaufArray.createNestedObject();
  ablaufObj["P1S"] = modes[PLAN1];
  ablaufObj["P2S"] = modes[PLAN2];
  ablaufObj["P3S"] = modes[PLAN3];
  DEBUG_MSG("P1S: %s\n", modes[PLAN1].c_str());
  DEBUG_MSG("P2S: %s\n", modes[PLAN2].c_str());
  DEBUG_MSG("P3S: %s\n", modes[PLAN3].c_str());

  size_t len = measureJson(doc);
  int memoryUsed = doc.memoryUsage();
  DEBUG_MSG("JSON config length: %d\n", len);
  DEBUG_MSG("JSON memory usage: %d\n", memoryUsed);

  if (len > 2048 || memoryUsed > 2500)
  {
    // DEBUG_MSG("JSON config length: %d\n", len);
    // DEBUG_MSG("JSON memory usage: %d\n", memoryUsed);

    DEBUG_MSG("%s\n", "Failed to write config file - config too large");
    DEBUG_MSG("%s\n", "------ saveConfig aborted ------");
    if (setGPIO == 1)
      sendAlarm(ALARM_ERROR);
    return false;
  }

  File configFile = LittleFS.open("/config.txt", "w");

  if (!configFile)
  {
    DEBUG_MSG("%s\n", "Failed to open config file for writing");
    DEBUG_MSG("%s\n", "------ saveConfig aborted ------");
    if (setGPIO == 1)
      sendAlarm(ALARM_ERROR);
    return false;
  }

  serializeJson(doc, configFile);
  configFile.close();
  DEBUG_MSG("%s\n", "------ saveConfig finished ------");

  // Setze Intervall Temperatur Ticker
  TickerTemp.interval(upTemp);

  // Setze Intervall Drucksensor Ticker
  TickerPressure.interval(upPressure);

  // Setze Intervall Datenbank Ticker
  TickerInfluxDB.interval(upInflux);

  // Setze Open/Close Standard für MV1/MV2
  mv1.change(mv1Open, mv1Close, startMV1);
  mv2.change(mv2Open, mv2Close, startMV2);
  DEBUG_MSG("%s\n", "------------");

  if (setMode != AUS && setGPIO == 1)
  {
    sendAlarm(ALARM_ON);
  }
  switch (setMode)
  {
  case AUS: // aus
    mv1.switchOff();
    mv2.switchOff();
    if (setGPIO == 1)
      sendAlarm(ALARM_OFF);
    break;
  case SPUNDEN_CO2: // CO2 Spunden
    mv2.switchOff();
    break;
  case SPUNDEN_DRUCK: // Druck Spunden
    mv2.switchOff();
    break;
  case KARBONISIEREN_CO2: // CO2 Karbonisieren
    mv1.switchOff();
    break;
  case KARBONISIEREN_DRUCK: // CO2 Karbonisieren
    mv1.switchOff();
    break;
  case SPUNDOMAT:                 // Spunden und Karbonisieren
    calcVerzSpundomat();          // Berechne Verzögerung Karbonisierung im Kombi-Modus
    lastTimeSpundomat = millis(); // Zeitstempel
    break;
  case PLAN1:
    readAblaufplan(PLAN1);
    break;
  case PLAN2:
    readAblaufplan(PLAN2);
    break;
  case PLAN3:
    readAblaufplan(PLAN3);
    break;
  case DICHTHEIT:
    DEBUG_MSG("%s\n", "Start Überprüfung Dichtheit");
    // lastTimeSpundomat = millis();
    // dichtPressure = pressure;
    lastTimeSpundomat = 0.0;
    break;
  default:
    mv1.switchOff();
    mv2.switchOff();
    break;
  }
  return true;
}
