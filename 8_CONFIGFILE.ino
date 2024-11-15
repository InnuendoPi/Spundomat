bool loadConfig()
{
  DEBUG_MSG("%s\n", "------ laden config.txt gestartet ------");
  File configFile = LittleFS.open(CONFIG, "r");
  if (!configFile)
  {
    DEBUG_MSG("%s\n", "error open config file - cannot read config.txt\n");
    DEBUG_MSG("%s\n", "------ laden config.txt abgebrochen ------");
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
  {
    DEBUG_MSG("config.txt Fehler JSON %s\n", error.c_str());
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

  DEBUG_MSG("Modus: %d\n", setMode);
  DEBUG_MSG("Zielwert Druck: %2.2f\n", setPressure);
  DEBUG_MSG("Zielwert CO2 Gehalt: %2.2f\n", setCarbonation);
  DEBUG_MSG("Verzögerung Karbonisierung: %.2f\n", verzKombi);
  DEBUG_MSG("Einheit Verzögerung: %d\n", setEinheit);

  calcVerzSpundomat(); // Berechne Verzögerung Karbonisierung im Kombi-Modus
  DEBUG_MSG("Verzögerung: %d minCarb: %2.2f\n", verzKarbonisierung, minKarbonisierung);
  DEBUG_MSG("%s\n", TRENNLINIE);

  // Hardware Einstellungen
  JsonArray hwArray = doc["HARDWARE"];
  JsonObject hwObj = hwArray[0];
  startMV1 = hwObj["MV1"] | 0;
  startMV2 = hwObj["MV2"] | 0;
  buzzer = hwObj["buz"] | 0;
  mv1Open = hwObj["MV1OPEN"] | DEFAULT_OPEN;
  mv1Close = hwObj["MV1CLOSE"] | DEFAULT_CLOSE;
  mv2Open = hwObj["MV2OPEN"] | DEFAULT_OPEN;
  mv2Close = hwObj["MV2CLOSE"] | DEFAULT_CLOSE;
  senOffset = hwObj["SENOFFSET"] | 0.0;
  DEBUG_MSG("MV1 Spunden:        %d Open: %d Close: %d\n", startMV1, mv1Open, mv1Close);
  DEBUG_MSG("MV2 Karbonisieren:  %d Open: %d Close: %d\n", startMV2, mv2Open, mv2Close);
  DEBUG_MSG("Buzzer: %d\n", buzzer);
  DEBUG_MSG("Sensor Offset: %.2f\n", senOffset);
  DEBUG_MSG("%s\n", TRENNLINIE);

  // System Einstellungen
  JsonArray miscArray = doc["MISC"];
  JsonObject miscObj = miscArray[0];
  upPressure = miscObj["UPPRESSURE"] | PRESSURE_UPDATE;
  upTemp = miscObj["UPTEMP"] | TEMPERATUR_UPDATE;
  startMDNS = miscObj["MDNS"] | 0;
  strlcpy(nameMDNS, miscObj["NAMEMDNS"] | "spundomat", maxHostSign);
  strlcpy(ntpServer, miscObj["ntp"] | NTP_ADDRESS, maxNTPSign);
  strlcpy(ntpZone, miscObj["zone"] | NTP_ZONE, maxNTPSign);

  DEBUG_MSG("Intervall Drucksensor: %d\n", upPressure);
  DEBUG_MSG("Intervall Temperatursensor: %d\n", upTemp);
  DEBUG_MSG("Aktiviere mDNS: %d\n", startMDNS);
  DEBUG_MSG("mDNS Name: %s\n", nameMDNS);

  // EEPROM
  readFlash();
  
  testModus = miscObj["TESTMODE"] | 0;
  devBranch = miscObj["devbranch"] | 0;
  useBacklightTimer = miscObj["useback"] | 0;
  DisplayTimerTime = miscObj["disptimer"] | TIMEOUTDISPLAY;
  DEBUG_MSG("useBacklightTimer: %d\n", useBacklightTimer);
  DEBUG_MSG("Aktiviere Display Timer: %d\n", DisplayTimerTime);
  DEBUG_MSG("Testmodus: %d\n", testModus);
  if (testModus)
  {
    ergDichtheit = 0.0;
    if (psettings.offset0 == 0.0 || psettings.offset2 == 0.0)
    {
      psettings.offset0 = 121.0; // readFloat(0);           // Lese Offset (Kalibrierung)
      psettings.offset2 = 362.0; // readFloat(4); // Lese Offset (Kalibrierung)
    }
    DEBUG_MSG("Testmodus Offset 0bar: %.2f\n", psettings.offset0);
    DEBUG_MSG("Testmodus Offset 2bar: %.2f\n", psettings.offset2);
    DEBUG_MSG("Testmodus Dichtheit: %.2f\n", ergDichtheit);
  }
  else
  {
    DEBUG_MSG("Eeprom Offset 0bar: %.2f\n", psettings.offset0);
    DEBUG_MSG("Eeprom Offset 2bar: %.2f\n", psettings.offset2);
    DEBUG_MSG("Dichtheit: %.2f\n", ergDichtheit);
  }
  DEBUG_MSG("%s\n", TRENNLINIE);

  // InfluxDB Einstellungen
  JsonArray databaseArray = doc["DATABASE"];
  JsonObject databaseObj = databaseArray[0];
  startDB = databaseObj["STARTDB"] | 0;
  dbServer = databaseObj["DBSERVER"] | "";
  dbDatabase = databaseObj["DB"] | "";
  dbUser = databaseObj["DBUSER"] | "";
  dbPass = databaseObj["DBPASS"] | "";
  upInflux = databaseObj["DBUP"] | DB_UPDATE;

  DEBUG_MSG("Visualisierung aktiviert: %d\n", startDB);
  DEBUG_MSG("DB Server: %s\n", dbServer);
  DEBUG_MSG("DB Name: %s\n", dbDatabase);
  DEBUG_MSG("DB Benutzer: %s\n", dbUser);
  DEBUG_MSG("DB Password: %s\n", dbPass);
  DEBUG_MSG("Update Interval DB: %d\n", upInflux); // in Minuten
  DEBUG_MSG("%s\n", TRENNLINIE);

  JsonArray visArray = doc["VISUALISIERUNG"];
  JsonObject visObj = visArray[0];

  startVis = visObj["VISSTARTED"] | 0;
  dbVisTag = visObj["SUDID"] | "";
  
  DEBUG_MSG("Visualisierung gestartet: %d ", startVis);
  DEBUG_MSG("Sud-Id: %s\n", dbVisTag);

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

  DEBUG_MSG("%s\n", "------ laden config.txt abgeschlossen ------");
  configFile.close();
  
  // System setup
  setupTime();

  mv1.change(mv1Open, mv1Close, startMV1);
  mv2.change(mv2Open, mv2Close, startMV2);
  mv1.switchOff();
  mv2.switchOff();

  // Ablaufpläne
  initAblaufplan(); // Initialisiere Strukturen
  TickerPressure.interval(upPressure);
  TickerInfluxDB.interval(upInflux);
  TickerTemp.interval(upTemp);

  if (buzzer && setMode == AUS)
    sendAlarm(ALARM_OK);
  else if (buzzer)
    sendAlarm(ALARM_ON);

  return true;
}

bool saveConfig()
{
  DEBUG_MSG("%s\n", "------ saveConfig started -------");
  JsonDocument doc;

  // Spundomat Einstellungen
  JsonArray spundArray = doc["SPUNDOMAT"].to<JsonArray>();
  JsonObject spundObj = spundArray.add<JsonObject>();
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
  JsonArray hwArray = doc["HARDWARE"].to<JsonArray>();
  JsonObject hwObj = hwArray.add<JsonObject>();
  hwObj["MV1"] = (int)startMV1;
  hwObj["MV2"] = (int)startMV2;
  hwObj["MV1OPEN"] = mv1Open;
  hwObj["MV1CLOSE"] = mv1Close;
  hwObj["MV2OPEN"] = mv2Open;
  hwObj["MV2CLOSE"] = mv2Close;
  hwObj["SENOFFSET"] = (int(senOffset * 100)) / 100.0;

  hwObj["buz"] = (int)buzzer;
  DEBUG_MSG("MV1: %d Open: %d Close %d\n", startMV1, mv1Open, mv1Close);
  DEBUG_MSG("MV2: %d Open: %d Close %d\n", startMV2, mv2Open, mv2Close);
  DEBUG_MSG("Buzzer: %d\n", buzzer);
  DEBUG_MSG("Sensor Offset: %.2f\n", senOffset);
  DEBUG_MSG("%s\n", "--------");

  // Datenbank Einstellungen
  JsonArray databaseArray = doc["DATABASE"].to<JsonArray>();
  JsonObject databaseObj = databaseArray.add<JsonObject>();

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

  JsonArray visArray = doc["VISUALISIERUNG"].to<JsonArray>();
  JsonObject visObj = visArray.add<JsonObject>();
  visObj["VISSTARTED"] = (int)startVis;
  visObj["SUDID"] = dbVisTag;
  DEBUG_MSG("Visualisierung: %d\n", startVis);
  DEBUG_MSG("Sud-Id: %s\n", dbVisTag);

  JsonArray ablaufArray = doc["ABLAUF"].to<JsonArray>();
  JsonObject ablaufObj = ablaufArray.add<JsonObject>();

  ablaufObj["P1S"] = modes[PLAN1];
  ablaufObj["P2S"] = modes[PLAN2];
  ablaufObj["P3S"] = modes[PLAN3];
  DEBUG_MSG("P1S: %s\n", modes[PLAN1].c_str());
  DEBUG_MSG("P2S: %s\n", modes[PLAN2].c_str());
  DEBUG_MSG("P3S: %s\n", modes[PLAN3].c_str());

  // System Einstellungen
  JsonArray miscArray = doc["MISC"].to<JsonArray>();
  JsonObject miscObj = miscArray.add<JsonObject>();

  miscObj["NAMEMDNS"] = nameMDNS;
  miscObj["MDNS"] = (int)startMDNS;
  miscObj["ntp"] = ntpServer;
  miscObj["zone"] = ntpZone;
  miscObj["UPPRESSURE"] = upPressure;
  miscObj["UPTEMP"] = upTemp;
  miscObj["TESTMODE"] = (int)testModus;
  miscObj["devbranch"] = (int)devBranch;
  miscObj["useback"] = (int)useBacklightTimer;
  miscObj["disptimer"] = DisplayTimerTime;
  miscObj["VER"] = Version;

  DEBUG_MSG("Interval Drucksensor: %d\n", upPressure);
  DEBUG_MSG("Interval Temperatursensor: %d\n", upTemp);
  DEBUG_MSG("nameMDNS: %s\n", nameMDNS);
  DEBUG_MSG("startMDNS: %d\n", startMDNS);
  DEBUG_MSG("setMode: %d\n", setMode);
  DEBUG_MSG("DevBranch: %d\n", devBranch);
  DEBUG_MSG("testModus: %d\n", testModus);
  DEBUG_MSG("useBacklightTimer: %d\n", useBacklightTimer);
  DEBUG_MSG("Aktiviere Display Timer: %d\n", DisplayTimerTime);
  DEBUG_MSG("%s\n", "--------");

  File configFile = LittleFS.open(CONFIG, "w");

  if (!configFile)
  {
    DEBUG_MSG("%s\n", "Failed to open config file for writing");
    DEBUG_MSG("%s\n", "------ saveConfig aborted ------");
    if (buzzer)
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

  if (setMode != AUS && buzzer)
  {
    sendAlarm(ALARM_ON);
  }
  switch (setMode)
  {
  case AUS: // aus
    mv1.switchOff();
    mv2.switchOff();
    if (buzzer)
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
