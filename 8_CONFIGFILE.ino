bool loadConfig()
{
  Serial.println("------ loadConfig started -------");
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile)
  {
    Serial.println("Failed to open config file");
    Serial.println("------ loadConfig aborted -------");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024)
  {
    Serial.print("Config file size is too large");
    Serial.println("------ loadConfig aborted -------");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, buf.get());
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

  Serial.print("setPressure: ");
  Serial.println(setPressure);
  Serial.print("setCarbonation: ");
  Serial.println(setCarbonation);
  Serial.print("setMode: ");
  Serial.println(setMode);
  Serial.println("--------");

  // Misc Settings
  JsonArray miscArray = doc["MISC"];
  JsonObject miscObj = miscArray[0];

  if (miscObj.containsKey("NAMEMDNS"))
    strlcpy(nameMDNS, miscObj["NAMEMDNS"], sizeof(nameMDNS));

  if (miscObj.containsKey("MDNS"))
    startMDNS = miscObj["MDNS"];
  if (miscObj.containsKey("DEBUG"))
    setDEBUG = miscObj["DEBUG"];
  if (miscObj.containsKey("TEST"))
    testModus = miscObj["TEST"];
  if (miscObj.containsKey("TELNET"))
    startTEL = miscObj["TELNET"];
    
  Serial.print("nameMDNS: ");
  Serial.println(nameMDNS);
  Serial.print("startMDNS: ");
  Serial.println(startMDNS);
  Serial.print("setDEBUG: ");
  Serial.println(setDEBUG);
  Serial.print("startTelnet: ");
  Serial.println(startTEL);
  Serial.println("------ loadConfig finished ------");
  configFile.close();
  size_t len = measureJson(doc);
  DBG_PRINT("*** SYSINFO: JSON Konfiguration Größe: ");
  DBG_PRINTLN(len);
  if (len > 256)
    Serial.println("*** SYSINFO: Fehler JSON Konfiguration zu groß!");
}

bool saveConfig()
{
  DBG_PRINTLN("------ saveConfig started -------");
  StaticJsonDocument<256> doc;
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    DBG_PRINTLN("Failed to open config file for writing");
    DBG_PRINTLN("------ saveConfig aborted ------");
    return false;
  }

  JsonArray spundArray = doc.createNestedArray("SPUNDOMAT");
  JsonObject spundObj = spundArray.createNestedObject();
  spundObj["PRESSURE"] = setPressure;
  spundObj["CARBONATION"] = setCarbonation;
  spundObj["MODE"] = setMode;
  DBG_PRINT("setPressure: ");
  DBG_PRINTLN(setPressure);
  DBG_PRINT("setCarbonation: ");
  DBG_PRINTLN(setCarbonation);
  DBG_PRINT("setMode: ");
  DBG_PRINTLN(setMode);
  DBG_PRINTLN("--------");

  JsonArray miscArray = doc.createNestedArray("MISC");
  JsonObject miscObj = miscArray.createNestedObject();

  miscObj["NAMEMDNS"] = nameMDNS;
  miscObj["MDNS"] = startMDNS;
  miscObj["DEBUG"] = setDEBUG;
  miscObj["TEST"] = testModus;
  miscObj["TELNET"] = startTEL;

  DBG_PRINT("nameMDNS: ");
  DBG_PRINTLN(nameMDNS);
  DBG_PRINT("startMDNS: ");
  DBG_PRINTLN(startMDNS);
  DBG_PRINT("setMode: ");
  DBG_PRINTLN(setMode);
  DBG_PRINT("setDEBUG: ");
  DBG_PRINTLN(setDEBUG);
  DBG_PRINT("startTELNET: ");
  DBG_PRINTLN(startTEL);
  serializeJson(doc, configFile);
  configFile.close();
  DBG_PRINTLN("------ saveConfig finished ------");
}
