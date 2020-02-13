bool loadConfig()
{
  Serial.println("------ loadConfig started -------");
  File configFile = SPIFFS.open("/config.txt", "r");
  if (!configFile)
  {
    Serial.println("*** SYSINFO: Fehler beim Laden der Konfiguration");
    Serial.println("------ loadConfig aborted -------");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024)
  {
    Serial.print("*** SYSINFO: Konfigurationsdatei zu groß");
    Serial.println("------ loadConfig aborted -------");
    if (startBuzzer)
      sendAlarm(ALARM_ERROR);
    return false;
  }

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
  {
    Serial.print("Conf: Error Json ");
    Serial.println(error.c_str());
    if (startBuzzer)
      sendAlarm(ALARM_PANIC);
    return false;
  }
  // Spundomat Einstellungen
  JsonArray spundArray = doc["SPUNDOMAT"];
  JsonObject spundObj = spundArray[0];
  if (spundObj.containsKey("PRESSURE"))
    setPressure = spundObj["PRESSURE"];
  if (spundObj.containsKey("CARBONATION"))
    setCarbonation = spundObj["CARBONATION"];
  if (spundObj.containsKey("MODE"))
    setMode = spundObj["MODE"];
  if (spundObj.containsKey("VERZKOMBI"))
    verzKombi = spundObj["VERZKOMBI"];
  if (spundObj.containsKey("EINHEIT"))
    setEinheit = spundObj["EINHEIT"];
  // Setze Startmodus auf Aus (obwohl Modus gespeichert ist)

  Serial.printf("setPressure: %d\n", setPressure);
  Serial.printf("setCarbonation: %d\n", setCarbonation);
  Serial.printf("setMode: %d\n", setMode);
  Serial.printf("verzKombi: %d\n", verzKombi);
  Serial.printf("setEinheit: %d\n", setEinheit);
  Serial.println("--------");

  // Berechne Verzögerung Karbonisierung im Kombi-Modus
  calcVerzSpundomat();

  // InfluxDB Einstellungen
  JsonArray databaseArray = doc["DATABASE"];
  JsonObject databaseObj = databaseArray[0];
  if (databaseObj.containsKey("STARTDB"))
    startDB = databaseObj["STARTDB"];
  if (databaseObj.containsKey("DBSERVER"))
    strlcpy(dbServer, databaseObj["DBSERVER"], sizeof(dbServer));
  if (databaseObj.containsKey("DB"))
    strlcpy(dbDatabase, databaseObj["DB"], sizeof(dbDatabase));
  if (databaseObj.containsKey("DBUSER"))
    strlcpy(dbUser, databaseObj["DBUSER"], sizeof(dbUser));
  if (databaseObj.containsKey("DBPASS"))
    strlcpy(dbPass, databaseObj["DBPASS"], sizeof(dbPass));
  if (databaseObj.containsKey("DBUP"))
    upInflux = databaseObj["DBUP"];

  Serial.printf("Database aktiviert: %d\n", startDB);
  Serial.printf("DB Server: %s\n", dbServer);
  Serial.printf("DB: %s\n", dbDatabase);
  Serial.printf("DB User: %s\n", dbUser);
  Serial.printf("DB Pass: %s\n", dbPass);
  Serial.printf("DB Update: %d\n", upInflux); // in Minuten
  Serial.println("--------");

  // Hardware Einstellungen
  JsonArray hwArray = doc["HARDWARE"];
  JsonObject hwObj = hwArray[0];
  if (hwObj.containsKey("MV1"))
    startMV1 = hwObj["MV1"];
  if (hwObj.containsKey("MV2"))
    startMV2 = hwObj["MV2"];
  if (hwObj.containsKey("BUZZER"))
    startBuzzer = hwObj["BUZZER"];
  if (hwObj.containsKey("MV1OPEN"))
    mv1Open = hwObj["MV1OPEN"];
  if (hwObj.containsKey("MV1CLOSE"))
    mv1Close = hwObj["MV1CLOSE"];
  if (hwObj.containsKey("MV2OPEN"))
    mv2Open = hwObj["MV2OPEN"];
  if (hwObj.containsKey("MV2CLOSE"))
    mv2Close = hwObj["MV2CLOSE"];

  Serial.printf("MV1: %d Open: %d Close: %d\n", startMV1, mv1Open, mv1Close);
  Serial.printf("MV2: %d Open: %d Close: %d\n", startMV2, mv2Open, mv2Close);
  Serial.printf("Buzzer: %d\n", startBuzzer);
  Serial.println("--------");

  // System Einstellungen
  JsonArray miscArray = doc["MISC"];
  JsonObject miscObj = miscArray[0];

  if (miscObj.containsKey("UPPRESSURE"))
    upPressure = miscObj["UPPRESSURE"];
  if (miscObj.containsKey("UPTEMP"))
    upTemp = miscObj["UPTEMP"];
  Serial.printf("Intervall Drucksensor: %d\n", upPressure);
  Serial.printf("Intervall Temperatursensor: %d\n", upTemp);
  if (miscObj.containsKey("NAMEMDNS"))
    strlcpy(nameMDNS, miscObj["NAMEMDNS"], sizeof(nameMDNS));

  if (miscObj.containsKey("MDNS"))
    startMDNS = miscObj["MDNS"];
  if (miscObj.containsKey("TESTMODE"))
    testModus = miscObj["TESTMODE"];
  if (miscObj.containsKey("OFFSET2"))
    offset2 = miscObj["OFFSET2"];
  if (miscObj.containsKey("PRESSOFFSET2"))
    pressureOffset2 = miscObj["PRESSOFFSET2"];

  Serial.printf("nameMDNS: %s\n", nameMDNS);
  Serial.printf("startMDNS: %d\n", startMDNS);
  Serial.printf("Testmodus: %d\n", testModus);
  Serial.printf("Offset2: %d bei %f\n", offset2, pressureOffset2);

  Serial.println("------ loadConfig finished ------");
  configFile.close();
  size_t len = measureJson(doc);
  DEBUG_MSG("*** SYSINFO: JSON Konfiguration Größe: %d\n", len);
  if (len > 1024)
    Serial.println("*** SYSINFO: Fehler JSON Konfiguration zu groß!");

  mv1.change(mv1Open, mv1Close, startMV1);
  mv2.change(mv2Open, mv2Close, startMV2);
  mv1.switchOff();
  mv2.switchOff();

  // Ablaufpläne
  initPlan();                           // Initialisiere Strukturen
  if (SPIFFS.exists("/ablaufplan.txt")) // Lade Ablaufpläne
  {
    file = SPIFFS.open("/ablaufplan.txt", "r");
    readLine(file);
    file.close();
  }

  //setMode = AUS;

  // Setze Intervalle für Ticker Objekte
  TickerPressure.interval(upPressure);
  TickerTemp.interval(upTemp);
  TickerSpundomat.interval(SPUNDOMAT_UPDATE);
  TickerSpundomat.stop();
  if (startBuzzer)
    sendAlarm(ALARM_OK);
}

bool saveConfig()
{
  DEBUG_MSG("%s\n", "------ saveConfig started -------");
  StaticJsonDocument<1024> doc;
  // Spundomat Einstellungen
  JsonArray spundArray = doc.createNestedArray("SPUNDOMAT");
  JsonObject spundObj = spundArray.createNestedObject();
  spundObj["PRESSURE"] = setPressure;
  spundObj["CARBONATION"] = setCarbonation;
  spundObj["MODE"] = setMode;
  spundObj["VERZKOMBI"] = verzKombi;
  spundObj["EINHEIT"] = setEinheit;
  DEBUG_MSG("setPressure: %f\n", setPressure);
  DEBUG_MSG("setCarbonation: %f\n", setCarbonation);
  DEBUG_MSG("verzKombi: %d\n", verzKombi);
  DEBUG_MSG("Einheit: %d\n", setEinheit);
  DEBUG_MSG("%s\n", "--------");

  // Hardware Einstellungen
  JsonArray hwArray = doc.createNestedArray("HARDWARE");
  JsonObject hwObj = hwArray.createNestedObject();
  hwObj["MV1"] = startMV1;
  hwObj["MV1OPEN"] = mv1Open;
  hwObj["MV1CLOSE"] = mv1Close;
  hwObj["MV2"] = startMV2;
  hwObj["MV2OPEN"] = mv2Open;
  hwObj["MV2CLOSE"] = mv2Close;
  hwObj["BUZZER"] = startBuzzer;
  DEBUG_MSG("MV1: %d Open: %d Close %d\n", startMV1, mv1Open, mv1Close);
  DEBUG_MSG("MV2: %d Open: %d Close %d\n", startMV2, mv2Open, mv2Close);
  DEBUG_MSG("Buzzer: %d\n", startBuzzer);
  DEBUG_MSG("%s\n", "--------");

  // Datenbank Einstellungen
  JsonArray databaseArray = doc.createNestedArray("DATABASE");
  JsonObject databaseObj = databaseArray.createNestedObject();
  databaseObj["STARTDB"] = startDB;
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
  miscObj["MDNS"] = startMDNS;
  miscObj["UPPRESSURE"] = upPressure;
  miscObj["UPTEMP"] = upTemp;
  miscObj["TESTMODE"] = testModus;
  miscObj["OFFSET2"] = offset2;
  miscObj["PRESSOFFSET2"] = pressureOffset2;

  DEBUG_MSG("Interval Drucksensor: %d\n", upPressure);
  DEBUG_MSG("Interval Temperatursensor: %d\n", upTemp);
  DEBUG_MSG("Offset2: %d bei %f\n", offset2, pressureOffset2);

  DEBUG_MSG("nameMDNS: %s\n", nameMDNS);
  DEBUG_MSG("startMDNS: %d\n", startMDNS);
  DEBUG_MSG("setMode: %d\n", setMode);

  size_t len = measureJson(doc);
  int memoryUsed = doc.memoryUsage();
  if (len > 1024 || memoryUsed > 1024)
  {
    DEBUG_MSG("JSON config length: %d\n", len);
    DEBUG_MSG("JSON memory usage: %d\n", memoryUsed);
    DEBUG_MSG("%s\n", "Failed to write config file - config too large");
    DEBUG_MSG("%s\n", "------ saveConfig aborted ------");
    return false;
  }

  File configFile = SPIFFS.open("/config.txt", "w");
  if (!configFile)
  {
    DEBUG_MSG("%s\n", "Failed to open config file for writing");
    DEBUG_MSG("%s\n", "------ saveConfig aborted ------");
    if (startBuzzer)
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

  if (setMode != SPUNDOMAT)
    TickerSpundomat.stop();
  else
    TickerSpundomat.start();

  if (setMode == AUS)
    TickerPressure.start();
  else
    TickerPressure.pause();

  // Setze Open/Close Standard für MV1/MV2
  mv1.change(mv1Open, mv1Close, startMV1);
  mv2.change(mv2Open, mv2Close, startMV2);
  DEBUG_MSG("%s\n", "------------");

  // Ablaufpläne
  if (startBuzzer)
    sendAlarm(ALARM_OK);

  switch (setMode)
  {
  case AUS: // aus
    mv1.switchOff();
    mv2.switchOff();
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
  case SPUNDOMAT:          // Spunden und Karbonisieren
    calcVerzSpundomat();   // Berechne Verzögerung Karbonisierung im Kombi-Modus
    prevMillis = millis(); // Zeitstempel
    break;
  case PLAN1:
    // for (int test = 0; test < 20; test++)
    // {
    //   DEBUG_MSG("Line %d: x1: %f y1: %d z1: %d x2: %f y2: %d z2: %d\n", test, structPlan1[test].zieldruckMV1, structPlan1[test].intervallMV1Open, structPlan1[test].intervallMV1Close, structPlan1[test].zieldruckMV2, structPlan1[test].intervallMV2Open, structPlan1[test].intervallMV2Close);
    // }
  case PLAN2:
  case PLAN3:
    initPlan();
    if (SPIFFS.exists("/ablaufplan.txt"))
    {
      file = SPIFFS.open("/ablaufplan.txt", "r");
      readLine(file);
      file.close();
    }
    counterPlan = -1;
    stepA = false;
    stepB = false;
    break;
  default:
    mv1.switchOff();
    mv2.switchOff();
    break;
  }
}

