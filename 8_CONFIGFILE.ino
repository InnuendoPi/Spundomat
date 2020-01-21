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
  if (size > 512)
  {
    Serial.print("*** SYSINFO: Konfigurationsdatei zu groß");
    Serial.println("------ loadConfig aborted -------");
    return false;
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
  {
    Serial.print("Conf: Error Json ");
    Serial.println(error.c_str());
    return false;
  }

  JsonArray spundArray = doc["SPUNDOMAT"];
  JsonObject spundObj = spundArray[0];
  if (spundObj.containsKey("PRESSURE"))
    setPressure = spundObj["PRESSURE"];
  if (spundObj.containsKey("CARBONATION"))
    setCarbonation = spundObj["CARBONATION"];
  if (spundObj.containsKey("MODE"))
    setMode = spundObj["MODE"];

  setMode = 0;

  Serial.print("setPressure: ");
  Serial.println(setPressure);
  Serial.print("setCarbonation: ");
  Serial.println(setCarbonation);
  Serial.print("setMode: ");
  Serial.println(setMode);
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

  Serial.print("MV1: ");
  Serial.print(startMV1);
  Serial.print(" Open: ");
  Serial.print(mv1Open);
  Serial.print(" Close: ");
  Serial.println(mv1Close);
  Serial.print("MV2: ");
  Serial.print(startMV2);
  Serial.print(" Open: ");
  Serial.print(mv2Open);
  Serial.print(" Close: ");
  Serial.println(mv2Close);
  Serial.print("Buzzer: ");
  Serial.println(startBuzzer);
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

  Serial.printf("nameMDNS: %s\n", nameMDNS);
  Serial.printf("startMDNS: %d\n", startMDNS);
  Serial.println("------ loadConfig finished ------");
  configFile.close();
  size_t len = measureJson(doc);
  DEBUG_MSG("*** SYSINFO: JSON Konfiguration Größe: %d\n", len);
  if (len > 384)
    Serial.println("*** SYSINFO: Fehler JSON Konfiguration zu groß!");

  TickerPressure.interval(upPressure);
  TickerTemp.interval(upTemp);
}

bool saveConfig()
{
  DEBUG_MSG("%s\n", "------ saveConfig started -------");
  StaticJsonDocument<512> doc;

  JsonArray spundArray = doc.createNestedArray("SPUNDOMAT");
  JsonObject spundObj = spundArray.createNestedObject();
  spundObj["PRESSURE"] = setPressure;
  spundObj["CARBONATION"] = setCarbonation;
  spundObj["MODE"] = setMode;
  DEBUG_MSG("setPressure: %f\n", setPressure);
  DEBUG_MSG("setCarbonation: %f\n", setCarbonation);
  DEBUG_MSG("setMode: %d\n", setMode);
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

  // System Einstellungen
  JsonArray miscArray = doc.createNestedArray("MISC");
  JsonObject miscObj = miscArray.createNestedObject();

  miscObj["NAMEMDNS"] = nameMDNS;
  miscObj["MDNS"] = startMDNS;
  miscObj["UPPRESSURE"] = upPressure;
  miscObj["UPTEMP"] = upTemp;
  
  DEBUG_MSG("Interval Drucksensor: %d\n", upPressure);
  DEBUG_MSG("Interval Temperatursensor: %d\n", upTemp);

  DEBUG_MSG("nameMDNS: %s\n", nameMDNS);
  DEBUG_MSG("startMDNS: %d\n", startMDNS);
  DEBUG_MSG("setMode: %d\n", setMode);
  
  size_t len = measureJson(doc);
  int memoryUsed = doc.memoryUsage();
  if (len > 512 || memoryUsed > 384)
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
    return false;
  }

  serializeJson(doc, configFile);
  configFile.close();
  DEBUG_MSG("%s\n", "------ saveConfig finished ------");
  
  TickerPressure.interval(upPressure);
  TickerTemp.interval(upTemp);

  // TickerMV1.stop();
  // TickerMV2.stop();

  switch (setMode)
  {
  case 0: // aus
    break;
  case 1: // CO2 Spunden

    TickerMV1.interval(mv1Open);
    statusMV1 = true;
    TickerMV1.start();
    break;
  case 2: // Druck Spunden
    TickerMV1.interval(mv1Open);
    statusMV1 = true;
    TickerMV1.start();
    break;
  case 3: // CO2 Karbonisieren
    TickerMV2.interval(mv2Open);
    statusMV2 = true;
    TickerMV2.start();
    break;
  default:
    break;
  }
}
