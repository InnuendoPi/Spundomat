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
  if (size > 512)
  {
    Serial.print("Config file size is too large");
    Serial.println("------ loadConfig aborted -------");
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<384> doc;
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

  // Hardware Einstellungen
  JsonArray hwArray = doc["HARDWARE"];
  JsonObject hwObj = hwArray[0];
  if (hwObj.containsKey("MV1"))
    startMV1 = hwObj["MV1"];
  if (hwObj.containsKey("MV2"))
    startMV2 = hwObj["MV2"];
  if (hwObj.containsKey("BUZZER"))
    startBuzzer = hwObj["BUZZER"];
  Serial.print("MV1: ");
  Serial.println(startMV1);
  Serial.print("MV2: ");
  Serial.println(startMV2);
  Serial.print("Buzzer: ");
  Serial.println(startBuzzer);
  Serial.println("--------");

  // System Einstellungen
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

  Serial.printf("nameMDNS: %s\n", nameMDNS);
  Serial.printf("startMDNS: %d\n", startMDNS);
  Serial.printf("setDEBUG: %d\n", setDEBUG);
  Serial.printf("startTelnet: %d\n", startTEL);
  Serial.println("------ loadConfig finished ------");
  configFile.close();
  size_t len = measureJson(doc);
  DBGPRINT("*** SYSINFO: JSON Konfiguration Größe: %d", len);
  if (len > 384)
    Serial.println("*** SYSINFO: Fehler JSON Konfiguration zu groß!");
}

bool saveConfig()
{
  DBGPRINT("%s", "------ saveConfig started -------");
  StaticJsonDocument<384> doc;
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    DBGPRINT("%s", "Failed to open config file for writing");
    DBGPRINT("%s", "------ saveConfig aborted ------");
    return false;
  }

  JsonArray spundArray = doc.createNestedArray("SPUNDOMAT");
  JsonObject spundObj = spundArray.createNestedObject();
  spundObj["PRESSURE"] = setPressure;
  spundObj["CARBONATION"] = setCarbonation;
  spundObj["MODE"] = setMode;
  DBGPRINT("setPressure: %f", setPressure);
  DBGPRINT("setCarbonation: %f", setCarbonation);
  DBGPRINT("setMode: %d", setMode);
  DBGPRINT("%s", "--------");

  // Hardware Einstellungen
  JsonArray hwArray = doc.createNestedArray("HARDWARE");
  JsonObject hwObj = hwArray.createNestedObject();

  hwObj["MV1"] = startMV1;
  hwObj["MV2"] = startMV2;
  hwObj["BUZZER"] = startBuzzer;
  DBGPRINT("MV1: %d", startMV1);
  DBGPRINT("MV2: %d", startMV2);
  DBGPRINT("Buzzer: %d", startBuzzer);
  DBGPRINT("%s", "--------");

  // System Einstellungen
  JsonArray miscArray = doc.createNestedArray("MISC");
  JsonObject miscObj = miscArray.createNestedObject();

  miscObj["NAMEMDNS"] = nameMDNS;
  miscObj["MDNS"] = startMDNS;
  miscObj["DEBUG"] = setDEBUG;
  miscObj["TEST"] = testModus;
  miscObj["TELNET"] = startTEL;

  DBGPRINT("nameMDNS: %s", nameMDNS);
  DBGPRINT("startMDNS: %d", startMDNS);
  DBGPRINT("setMode: %d", setMode);
  DBGPRINT("setDEBUG: %d", setDEBUG);
  DBGPRINT("Test: %d", testModus);
  DBGPRINT("startTELNET: %d", startTEL);
  serializeJson(doc, configFile);
  configFile.close();
  DBGPRINT("%s", "------ saveConfig finished ------");
}
