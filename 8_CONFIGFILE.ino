bool loadConfig()
{
  Serial.println("------ loadConfig started -------");
  File configFile = LittleFS.open("/config.txt", "r");
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
    if (setGPIO == 1)
      sendAlarm(ALARM_ERROR);
    return false;
  }

  StaticJsonDocument<1024> doc;
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
  if (spundObj.containsKey("PRESSURE"))
    setPressure = spundObj["PRESSURE"];
  if (spundObj.containsKey("CARBONATION"))
    setCarbonation = spundObj["CARBONATION"];
  if (spundObj.containsKey("MODE"))
  {
    setMode = spundObj["MODE"];
    newMode = setMode;
  }

  if (spundObj.containsKey("VERZKOMBI"))
    verzKombi = spundObj["VERZKOMBI"];
  if (spundObj.containsKey("EINHEIT"))
    setEinheit = spundObj["EINHEIT"];
  if (spundObj.containsKey("TARGET"))
    targetTemp = spundObj["TARGET"];

  Serial.printf("setPressure: %2.2f\n", setPressure);
  Serial.printf("setCarbonation: %2.2f\n", setCarbonation);
  Serial.printf("setMode: %d\n", setMode);
  Serial.printf("verzKombi: %f\n", verzKombi);
  Serial.printf("setEinheit: %d\n", setEinheit);
  Serial.printf("targetTemp: %3.2f\n", targetTemp);
  calcVerzSpundomat(); // Berechne Verzögerung Karbonisierung im Kombi-Modus
  Serial.printf("Verzögerung: %d minCarb: %2.2f\n", verzKarbonisierung, minKarbonisierung);
  Serial.println("--------");

  // InfluxDB Einstellungen
  JsonArray databaseArray = doc["DATABASE"];
  JsonObject databaseObj = databaseArray[0];

  if (databaseObj["STARTDB"] || databaseObj["STARTDB"] == "1")
    startDB = true;
  else
    startDB = false;

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

  if (hwObj["MV1"] || hwObj["MV1"] == "1")
    startMV1 = true;
  else
    startMV1 = false;
  if (hwObj["MV2"] || hwObj["MV2"] == "1")
    startMV2 = true;
  else
    startMV2 = false;
  if (hwObj.containsKey("GPIO"))
    setGPIO = hwObj["GPIO"];
  if (hwObj.containsKey("MV1OPEN"))
    mv1Open = hwObj["MV1OPEN"];
  if (hwObj.containsKey("MV1CLOSE"))
    mv1Close = hwObj["MV1CLOSE"];
  if (hwObj.containsKey("MV2OPEN"))
    mv2Open = hwObj["MV2OPEN"];
  if (hwObj.containsKey("MV2CLOSE"))
    mv2Close = hwObj["MV2CLOSE"];
  if (hwObj["CO2SEN"] || hwObj["CO2SEN"] == "1")
    startCO2 = true;
  else
    startCO2 = false;

  Serial.printf("MV1: %d Open: %d Close: %d\n", startMV1, mv1Open, mv1Close);
  Serial.printf("MV2: %d Open: %d Close: %d\n", startMV2, mv2Open, mv2Close);
  Serial.printf("startCO2: %d\n", startCO2);
  Serial.printf("GPIO: %d\n", setGPIO);
  Serial.println("--------");

  // System Einstellungen
  JsonArray miscArray = doc["MISC"];
  JsonObject miscObj = miscArray[0];

  if (miscObj.containsKey("UPPRESSURE"))
    upPressure = miscObj["UPPRESSURE"];
  if (miscObj.containsKey("UPTEMP"))
    upTemp = miscObj["UPTEMP"];
  if (miscObj.containsKey("UPTARGET"))
    upTarget = miscObj["UPTARGET"];
  if (miscObj.containsKey("UPCON"))
    upCon = miscObj["UPCON"];
  if (miscObj["MDNS"] || miscObj["MDNS"] =="1")
    startMDNS = true;
  else
    startMDNS = false;

  if (miscObj["TESTMODE"] || miscObj["TESTMODE"] == "1")
    testModus = true;
  else
    testModus = false;
  if (miscObj.containsKey("NAMEMDNS"))
    strlcpy(nameMDNS, miscObj["NAMEMDNS"], sizeof(nameMDNS));

  Serial.printf("Intervall Drucksensor: %d\n", upPressure);
  Serial.printf("Intervall Temperatursensor: %d\n", upTemp);
  Serial.printf("Intervall Gärsteuerung: %d\n", upTarget);
  Serial.printf("Intervall Gär Controller: %d\n", upCon);
  Serial.printf("nameMDNS: %s\n", nameMDNS);
  Serial.printf("startMDNS: %d\n", startMDNS);
  Serial.printf("Testmodus: %d\n", testModus);
  Serial.println("--------");

  // Visualisierungs Einstellungen
  JsonArray visArray = doc["VISUALISIERUNG"];
  JsonObject visObj = visArray[0];

  if (visObj["VISSTARTED"] || visObj["VISSTARTED"] == "1")
    startVis = true;
  else
    startVis = false;
  if (visObj.containsKey("SUDID"))
    strlcpy(dbVisTag, visObj["SUDID"], sizeof(dbVisTag));

  Serial.printf("Visualisierung: %d\n", startVis);
  Serial.printf("Sud-Id: %s\n", dbVisTag);

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
  initAblaufplan();                       // Initialisiere Strukturen
  if (LittleFS.exists("/ablaufplan.txt")) // Lade Ablaufpläne
  {
    file = LittleFS.open("/ablaufplan.txt", "r");
    readAblaufplan(file);
    file.close();
  }
  initSteuerplan();
  if (LittleFS.exists("/steuerplan.txt")) // Lade Ablaufpläne
  {
    file = LittleFS.open("/steuerplan.txt", "r");
    readSteuerplan(file);
    file.close();
  }

  // Setze Intervalle für Ticker Objekte
  TickerPressure.interval(upPressure);
  TickerInfluxDB.interval(upInflux);
  TickerTemp.interval(upTemp);
  TickerSteuerung.interval(upTarget);
  TickerCon.interval(upCon);
  TickerAlarmierung.interval(ZUSATZALARM);

  if (setGPIO == 1 && setMode == AUS)
    sendAlarm(ALARM_OK);
  else if (setGPIO == 1)
    sendAlarm(ALARM_ON);
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
  spundObj["TARGET"] = targetTemp;
  DEBUG_MSG("setPressure: %2.2f\n", setPressure);
  DEBUG_MSG("setCarbonation: %2.2f\n", setCarbonation);
  DEBUG_MSG("verzKombi: %d\n", verzKombi);
  DEBUG_MSG("Einheit: %d\n", setEinheit);
  DEBUG_MSG("Target: %3.2f\n", targetTemp);
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
  hwObj["CO2SEN"] = (int)startCO2;
  hwObj["GPIO"] = setGPIO;
  DEBUG_MSG("MV1: %d Open: %d Close %d\n", startMV1, mv1Open, mv1Close);
  DEBUG_MSG("MV2: %d Open: %d Close %d\n", startMV2, mv2Open, mv2Close);
  DEBUG_MSG("startCO2: %d\n", startCO2);
  DEBUG_MSG("GPIO: %d\n", setGPIO);
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
  miscObj["UPTARGET"] = upTarget;
  miscObj["UPCON"] = upCon;
  miscObj["TESTMODE"] = (int)testModus;

  DEBUG_MSG("Interval Drucksensor: %d\n", upPressure);
  DEBUG_MSG("Interval Temperatursensor: %d\n", upTemp);
  DEBUG_MSG("Interval Gärsteuerung: %d\n", upTarget);
  DEBUG_MSG("Interval Gär Controller: %d\n", upCon);
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

  size_t len = measureJson(doc);
  int memoryUsed = doc.memoryUsage();
  if (len > 1024 || memoryUsed > 1024)
  {
    DEBUG_MSG("JSON config length: %d\n", len);
    DEBUG_MSG("JSON memory usage: %d\n", memoryUsed);
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

  // Setze Intervall Gärsteuerung Ticker
  TickerSteuerung.interval(upTarget);
  TickerCon.interval(upCon);
  TickerAlarmierung.interval(ZUSATZALARM);

  // Setze Open/Close Standard für MV1/MV2
  mv1.change(mv1Open, mv1Close, startMV1);
  mv2.change(mv2Open, mv2Close, startMV2);
  DEBUG_MSG("%s\n", "------------");
  if (setMode != AUS && setGPIO == 1)
    sendAlarm(ALARM_ON);
  if (setMode != STEUERUNG)
  {
    TickerSteuerung.stop();
    TickerAlarmierung.stop();
  }
  else if (setMode != CON1 || setMode != CON2)
  {
    TickerCon.stop();
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
  case PLAN2:
  case PLAN3:
    // for (int test = 0; test < 20; test++)
    // {
    //   DEBUG_MSG("Line %d: x1: %f y1: %d z1: %d x2: %f y2: %d z2: %d\n", test, structPlan1[test].zieldruckMV1, structPlan1[test].intervallMV1Open, structPlan1[test].intervallMV1Close, structPlan1[test].zieldruckMV2, structPlan1[test].intervallMV2Open, structPlan1[test].intervallMV2Close);
    // }
    initAblaufplan();
    if (LittleFS.exists("/ablaufplan.txt"))
    {
      file = LittleFS.open("/ablaufplan.txt", "r");
      readAblaufplan(file);
      file.close();
    }
    counterPlan = -1;
    stepA = false;
    stepB = false;
    break;
  case DICHTHEIT:
    DEBUG_MSG("%s\n", "Start Überprüfung Dichtheit");
    // lastTimeSpundomat = millis();
    // dichtPressure = pressure;
    lastTimeSpundomat = 0.0;
    break;
  case STEUERUNG:
    if (TickerSteuerung.state() != RUNNING)
      TickerSteuerung.start();
    if (TickerAlarmierung.state() != RUNNING)
      TickerAlarmierung.start();
    steuerung();
    break;
  case CON1:
  case CON2:
    counterCon = 0;
    initSteuerplan();
    if (LittleFS.exists("/steuerplan.txt")) // Lade Ablaufpläne
    {
      file = LittleFS.open("/steuerplan.txt", "r");
      readSteuerplan(file);
      file.close();
    }
    targetTemp = structCon[counterCon].zieltemp;
    TickerCon.interval(structCon[counterCon].timer); // setze Zeitintervall aus Steuerplan
    if (TickerCon.state() != RUNNING)
      TickerCon.start();
    checkTemp = false;
    startCon();
    break;
  default:
    mv1.switchOff();
    mv2.switchOff();
    break;
  }
}
