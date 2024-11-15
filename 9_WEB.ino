void handleRoot()
{
    server.sendHeader(PSTR("Content-Encoding"), "gzip");
    server.send_P(200, "text/html", index_htm_gz, index_htm_gz_len);
}

void handleAblauf()
{
    server.sendHeader(PSTR("Content-Encoding"), "gzip");
    server.send_P(200, "text/html", ablaufplan_htm_gz, ablaufplan_htm_gz_len);
}
void handleWebRequests()
{
    if (loadFromLittlefs(server.uri()))
    {
        return;
    }
    String message = "File Not Detected\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

bool loadFromLittlefs(String path)
{
    String dataType = "text/plain";
    if (path.endsWith("/"))
        path += "index.html";

    if (path.endsWith(".src"))
        path = path.substring(0, path.lastIndexOf("."));
    else if (path.endsWith(".html"))
        dataType = "text/html";
    else if (path.endsWith(".htm"))
        dataType = "text/html";
    else if (path.endsWith(".css"))
        dataType = "text/css";
    else if (path.endsWith(".js"))
        dataType = "application/javascript";
    else if (path.endsWith(".png"))
        dataType = "image/png";
    else if (path.endsWith(".gif"))
        dataType = "image/gif";
    else if (path.endsWith(".jpg"))
        dataType = "image/jpeg";
    else if (path.endsWith(".ico"))
        dataType = "image/x-icon";
    else if (path.endsWith(".xml"))
        dataType = "text/xml";
    else if (path.endsWith(".pdf"))
        dataType = "application/pdf";
    else if (path.endsWith(".zip"))
        dataType = "application/zip";

    if (!LittleFS.exists(path.c_str()))
    {
        return false;
    }
    File dataFile = LittleFS.open(path.c_str(), "r");
    int fsize = dataFile.size();
    server.sendHeader("Content-Length", (String)(fsize));
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");

    if (server.hasArg("download"))
        dataType = "application/octet-stream";
    if (server.streamFile(dataFile, dataType) != dataFile.size())
    {
    }
    dataFile.close();
    return true;
}

void handleRequestMiscSet()
{
    JsonDocument doc;
    doc["mdns"] = startMDNS;
    doc["mdns_name"] = nameMDNS;
    doc["pressure"] = setPressure;
    doc["carbonation"] = setCarbonation;
    doc["mode"] = setMode;
    doc["co2"] = ((int)(calcCarbonation(pressure, temperature) * 100)) / 100.0;
    doc["druck"] = pressure;
    doc["temperatur"] = temperature;
    doc["offset"] = psettings.offset0;
    doc["offset2"] = psettings.offset2;
    doc["buz"] = buzzer;
    doc["mv1"] = startMV1;
    doc["mv2"] = startMV2;
    doc["senoffset"] = senOffset;
    doc["dichtheit"] = ((int)(ergDichtheit * 1000)) / 1000.0;
    if (setMode < PLAN1 || setMode > PLAN3)
    {
        doc["mv1opendisp"] = mv1Open;
        doc["mv1closedisp"] = mv1Close;
        doc["mv2opendisp"] = mv2Open;
        doc["mv2closedisp"] = mv2Close;
    }
    else
    {
        doc["mv1opendisp"] = structPlan[counterPlan].intervallMV1Open;
        doc["mv1closedisp"] = structPlan[counterPlan].intervallMV1Close;
        doc["mv2opendisp"] = structPlan[counterPlan].intervallMV2Open;
        doc["mv2closedisp"] = structPlan[counterPlan].intervallMV2Close;
    }
    doc["startdb"] = startDB;
    doc["vistag"] = dbVisTag;
    doc["visstate"] = visState;
    doc["startvis"] = startVis;
    doc["einheit"] = setEinheit;
    doc["ablauf"] = (counterPlan + 1);
    doc["alertstate"] = alertState;
    if (alertState)
        alertState = false;

    doc["restverz"] = "0";
    if (setEinheit == 0)
    {
        doc["delayspund"] = (int)(verzKarbonisierung / 1000 / 60);
        if (verzKarbonisierung > 0)
        {
            long restzeit = (lastTimeSpundomat + verzKarbonisierung - millis()) / 1000;
            if (restzeit < 0)
                doc["restverz"] = "0";
            int resth = restzeit / 60 / 60;
            int restm = restzeit / 60;
            int rests = restzeit;
            resth % 24 > 0 ? resth = resth % 24 : resth = 0;
            restm % 60 > 0 ? restm = restm % 60 : restm = 0;
            rests % 60 > 0 ? rests = rests % 60 : rests = 0;
            char countdown[9]{'\0'};
            int x = snprintf(countdown, sizeof(countdown), "%02d:%02d:%02d", resth, restm, rests);
            if (x > 0)
                doc["restverz"] = countdown;
            else
                doc["restverz"] = "0";
        }
    }
    else if (setEinheit == 1)
    {
        doc["delayspund"] = (int)(verzKarbonisierung / 1000 / 60 / 60);
        if (verzKarbonisierung > 0)
        {
            long restzeit = (lastTimeSpundomat + verzKarbonisierung - millis()) / 1000;
            if (restzeit < 0)
                doc["restverz"] = "0";
            int resth = restzeit / 60 / 60;
            int restm = restzeit / 60;
            int rests = restzeit;
            resth % 24 > 0 ? resth = resth % 24 : resth = 0;
            restm % 60 > 0 ? restm = restm % 60 : restm = 0;
            rests % 60 > 0 ? rests = rests % 60 : rests = 0;
            char countdown[9]{'\0'};
            int x = snprintf(countdown, sizeof(countdown), "%02d:%02d:%02d", resth, restm, rests);
            if (x > 0)
                doc["restverz"] = countdown;
            else
                doc["restverz"] = "0";
        }
    }
    else
    {
        doc["delayspund"] = minKarbonisierung;
    }

    if (setMode == DICHTHEIT)
    {
        long restzeit = (lastTimeSpundomat + PAUSE5MIN + PAUSE2MIN - millis()) / 1000;
        if (restzeit < 0)
            doc["restverz"] = "0";
        int resth = restzeit / 60 / 60;
        int restm = restzeit / 60;
        int rests = restzeit;
        resth % 24 > 0 ? resth = resth % 24 : resth = 0;
        restm % 60 > 0 ? restm = restm % 60 : restm = 0;
        rests % 60 > 0 ? rests = rests % 60 : rests = 0;
        // doc["restverz"] = String(resth) + ":" + String(restm) + ":" + String(rests);
        char countdown[9]{'\0'};
        int x = snprintf(countdown, sizeof(countdown), "%02d:%02d:%02d", resth, restm, rests);
        if (x > 0)
            doc["restverz"] = countdown;
        else
            doc["restverz"] = "0";
        // berechneCountdown(lastTimeSpundomat + PAUSE5MIN + PAUSE2MIN);
    }
    char response[measureJson(doc) + 1];
    serializeJson(doc, response, sizeof(response));
    replyResponse(response);
}

void handleRequestMisc()
{
    JsonDocument doc;
    doc["mdns"] = startMDNS;
    doc["mdns_name"] = nameMDNS;
    doc["ntp"] = ntpServer;
    doc["zone"] = ntpZone;
    doc["pressure"] = setPressure;
    doc["carbonation"] = setCarbonation;
    doc["buz"] = buzzer;
    doc["mv1"] = startMV1;
    doc["mv2"] = startMV2;
    doc["senoffset"] = senOffset;
    doc["mv1open"] = mv1Open;
    doc["mv1close"] = mv1Close;
    doc["mv2open"] = mv2Open;
    doc["mv2close"] = mv2Close;
    doc["testmode"] = testModus;
    doc["dev"] = devBranch;
    doc["uppressure"] = upPressure;
    doc["uptemp"] = upTemp;
    if (setEinheit == 0 || setEinheit == 1)
        doc["verzkombi"] = (int)verzKombi;
    else
        doc["verzkombi"] = verzKombi;
    doc["offset"] = psettings.offset0;
    doc["offset2"] = psettings.offset2;
    doc["dbserver"] = dbServer;
    doc["startdb"] = startDB;
    doc["dbdatabase"] = dbDatabase;
    doc["dbuser"] = dbUser;
    doc["dbpass"] = dbPass;
    doc["dbup"] = upInflux / 1000;
    doc["vistag"] = dbVisTag;
    doc["rssi"] = WiFi.RSSI();
    doc["dichtheit"] = ((int)(ergDichtheit * 1000)) / 1000.0;
    doc["useback"] = useBacklightTimer;
    doc["disptimer"] = DisplayTimerTime;

    char response[measureJson(doc) + 1];
    serializeJson(doc, response, sizeof(response));
    replyResponse(response);
}

void handleRequestSudID()
{
    String request = server.arg(0);
    String message = dbVisTag;
    server.send_P(200, "application/json", message.c_str());
}

void handleRequestName()
{
    String request = server.arg(0);
    String message = "";
    if (request == "planname1")
    {
        message = modesWeb[PLAN1];
        // DEBUG_MSG("Web: Plan1 message %s\n", message.c_str());
    }
    else if (request == "planname1short")
    {
        message = modes[PLAN1];
        // DEBUG_MSG("Web: Plan1s message %s\n", message.c_str());
    }
    else if (request == "planname2")
    {
        message = modesWeb[PLAN2];
        // DEBUG_MSG("Web: Plan2 message %s\n", message.c_str());
    }
    else if (request == "planname2short")
    {
        message = modes[PLAN2];
        // DEBUG_MSG("Web: Plan2s message %s\n", message.c_str());
    }
    else if (request == "planname3")
    {
        message = modesWeb[PLAN3];
        // DEBUG_MSG("Web: Plan3 message %s\n", message.c_str());
    }
    else if (request == "planname3short")
    {
        message = modes[PLAN3];
        // DEBUG_MSG("Web: Plan3s message %s\n", message.c_str());
    }
    server.send_P(200, "application/json", message.c_str());
}

void handleRequestFirm()
{
    String request = server.arg(0);
    String message;
    if (startMDNS)
    {
        message = nameMDNS;
        message += F(" V");
    }
    else
        message = F("Spundomat V ");

    message += Version;
    server.send_P(200, "application/json", message.c_str());
}

void handleSetMisc()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg(0));
    if (error)
    {
        DEBUG_MSG("error deserializeJson setMisc: %s", error.c_str());
        server.send(500, FPSTR("text/plain"), "Server error\r\n");
        return;
    }
    if (doc["reset"] && doc["clear"])
    {
        LittleFS.remove(CONFIG);
        eraseFlash();
        WiFi.disconnect();
        wifiManager.resetSettings();
        millis2wait(PAUSE2SEC);
        ESP.reset();
    }
    if (doc["reset"])
    {
        WiFi.disconnect();
        wifiManager.resetSettings();
        millis2wait(PAUSE2SEC);
        ESP.reset();
    }
    if (doc["clear"])
    {
        LittleFS.remove(CONFIG);
        eraseFlash();
        WiFi.disconnect();
        wifiManager.resetSettings();
        millis2wait(PAUSE2SEC);
        ESP.reset();
    }
    if (doc["eeprom"])
    {
        eraseFlash();
    }

    String tmpVal = ""; // helper var

    strlcpy(nameMDNS, doc["mdns_name"] | "", maxHostSign);
    // checkChars(nameMDNS);
    startMDNS = doc["mdns"];
    strlcpy(ntpServer, doc["ntp"] | NTP_ADDRESS, maxNTPSign);
    strlcpy(ntpZone, doc["zone"] | NTP_ZONE, maxNTPSign);
    setMode = doc["mode"];
    newMode = setMode;
    setPressure = doc["pressure"];
    setCarbonation = doc["carbonation"];
    startMV1 = doc["mv1"];
    mv1Open = doc["mv1open"];
    mv1Close = doc["mv1close"];
    startMV2 = doc["mv2"];
    mv2Open = doc["mv2open"];
    mv2Close = doc["mv2close"];
    buzzer = doc["buz"];
    upPressure = doc["uppressure"];
    senOffset = doc["senoffset"];
    upTemp = doc["uptemp"];
    useBacklightTimer = doc["useback"];
    DisplayTimerTime = doc["disptimer"];
    tmpVal = doc["startdb"].as<String>();
    dbServer = doc["dbserver"].as<String>();
    dbDatabase = doc["dbdatabase"].as<String>();
    dbUser = doc["dbuser"].as<String>();
    dbPass = doc["dbpass"].as<String>();
    upInflux = doc["dbup"];
    testModus = doc["testmode"];
    devBranch = doc["devbranch"];
    setEinheit = doc["einheit"];
    verzKombi = doc["delayspund"];

    if (startMV1)
        pinMode(PIN_MV1, OUTPUT);
    if (startMV2)
        pinMode(PIN_MV2, OUTPUT);

    TickerDisplayTimer.stop();
    TickerDisplayTimer.interval(DisplayTimerTime);
    if (useBacklightTimer)
    {
        if (TickerDisplayTimer.state() != RUNNING)
            TickerDisplayTimer.start();
    }
    else
    {
        if (TickerDisplayTimer.state() == RUNNING)
            TickerDisplayTimer.stop();
        // Displaybeleuchtung jetzt starten
        lcd.display();                     // show display (with the previous content)
        lcd.setBacklight(LedBackLight_On); // turn on display background
        DisplayIsOn = true;
    }

    if (setEinheit == 0)
        verzKarbonisierung = verzKombi * 1000 * 60;
    else if (setEinheit == 1)
        verzKarbonisierung = verzKombi * 1000 * 60 * 60;
    else if (setEinheit == 2)
    {
        verzKarbonisierung = 0;
        minKarbonisierung = verzKombi;
    }

    if (tmpVal.equals("InfluxDB1"))
    {
        startDB = 1;
    }
    else if (tmpVal.equals("InfluxDB2"))
    {
        startDB = 2;
    }
    else
    {
        startDB = 0;
    }

    server.send(200, "text/plain", "ok");
    saveConfig();
    reflashLCD = true;
}

void handleSetMisc2()
{
    for (int i = 0; i < server.args(); i++)
    {
        if (server.argName(i) == "planname1s")
        {
            modes[PLAN1] = server.arg(i);
            modesWeb[PLAN1] = server.arg(i);
        }
        if (server.argName(i) == "planname2s")
        {
            modes[PLAN2] = server.arg(i);
            modesWeb[PLAN2] = server.arg(i);
        }
        if (server.argName(i) == "planname3s")
        {
            modes[PLAN3] = server.arg(i);
            modesWeb[PLAN3] = server.arg(i);
        }
    }
    server.send(200, "text/plain", "ok");
    saveConfig();
}

void visualisieren()
{
    for (int i = 0; i < server.args(); i++)
    {
        if (server.argName(i) == "vistag")
        {
            dbVisTag = server.arg(i);
            // server.arg(i).toCharArray(dbVisTag, 15);
            // checkChars2(dbVisTag);
        }
        if (server.argName(i) == "startvis")
        {
            startVis = server.arg(i).toInt();
            // Serial.printf("Web: startvis %d - %s\n", startVis, server.arg(i).c_str() );
            if (startVis)
            {
                TickerInfluxDB.interval(upInflux);
                TickerInfluxDB.start();
            }
            else
            {
                TickerInfluxDB.stop();
            }
        }
    }
    if (startDB > 0 && startVis)
    {
        TickerInfluxDB.resume();
        sendDBData();
    }
    else
        TickerInfluxDB.pause();

    saveConfig();
    server.send(200, "text/plain", "ok");
}

void kalibrieren()
{
    server.send_P(200, "text/plain", "ok");
    if (psettings.offset0 == 0.0) // Keine Kalibrierung bei 0 bar
    {
        psettings.offset0 = readSensor();
        writeFlash();
        // DEBUG_MSG("*** 1. Kalibrierung offset0: %f\n", psettings.offset0);
    }
    else if (psettings.offset0 > 0 && readSensor() < 200.0)
    {
        psettings.offset0 = readSensor();
        writeFlash();
        // DEBUG_MSG("*** Re-Kalibrierung offset0: %f\n", psettings.offset0);
    }
    else if (psettings.offset0 > 0 && readSensor() > 200.0) // 2bar ca. 330
    {
        psettings.offset2 = readSensor();
        writeFlash();
        // DEBUG_MSG("*** 2. Kalibrierung offset0: %f offset2: %f\n", psettings.offset0, psettings.offset2);
    }
    readPressure();
    //    page = 2;
    //    menuitem = 0;
    reflashLCD = true;
}

void handlereqMode()
{
    String message;
    message += F("<option>");
    message += modesWeb[setMode];
    message += F("</option><option disabled>──────────</option>");
    for (int i = 0; i < sizeOfModes; i++)
    {
        if (setMode != i)
        {
            message += F("<option value=\"");
            message += i;
            message += "\">";
            message += modesWeb[i];
            message += F("</option>");
        }
    }
    yield();
    server.send_P(200, "application/json", message.c_str());
}

void handlereqVisual()
{
    String message;
    if (startDB == 0)
    {
        message += F("<option>Aus</option>");
        message += F("</option><option disabled>──────────</option>");
        message += F("<option>InfluxDB1</option>");
        message += F("<option>InfluxDB2</option>");
    }
    else if (startDB == 1)
    {
        message += F("<option>InfluxDB1</option>");
        message += F("</option><option disabled>──────────</option>");
        message += F("<option>Aus</option>");
        message += F("<option>InfluxDB2</option>");
    }
    else if (startDB == 2)
    {
        message += F("<option>InfluxDB2</option>");
        message += F("</option><option disabled>──────────</option>");
        message += F("<option>Aus</option>");
        message += F("<option>InfluxDB1</option>");
    }
    server.send_P(200, "application/json", message.c_str());
}

void handlereqEinheit()
{
    String message;
    message += F("<option>");
    message += einheit[setEinheit];
    message += F("</option><option disabled>──────────</option>");
    for (int i = 0; i < anzAuswahl; i++)
    {
        if (setEinheit != i)
        {
            message += F("<option value=\"");
            message += i;
            message += "\">";
            message += einheit[i];
            message += F("</option>");
        }
    }
    yield();
    server.send_P(200, "application/json", message.c_str());
}

void rebootDevice()
{
    server.sendHeader("Location", "/", true);
    server.send(205, FPSTR("text/plain"), "reboot");
    LittleFS.end(); // unmount LittleFS
    ESP.restart();
}

void setMDNS()
{
    if (startMDNS && nameMDNS[0] != '\0' && WiFi.status() == WL_CONNECTED)
    {
        if (MDNS.begin(nameMDNS))
            Serial.printf("*** SYSINFO: http://%s mit IP Adresse %s - Uhrzeit: %s WLAN Signal: %ddBm\n", nameMDNS, WiFi.localIP().toString().c_str(), zeit, WiFi.RSSI());
        else
            Serial.printf("*** SYSINFO: Fehler mDNS! Aktuelle IP Adresse: %s Uhrzeit: %s RSSI: %d\n", WiFi.localIP().toString().c_str(), zeit, WiFi.RSSI());
    }
}

void startDichtheit()
{
    // DEBUG_MSG("%s\n", "Start Überprüfung Dichtheit");
    setMode = DICHTHEIT;
    lastTimeSpundomat = millis();
    dichtPressure = pressure;
}

void handlePlan()
{
    JsonDocument docIn;
    JsonDocument docOut;
    DeserializationError error = deserializeJson(docIn, planfile);
    JsonArray ablaufArray = docIn.as<JsonArray>();
    int anzahlSchritte = ablaufArray.size();
    if (anzahlSchritte > maxSchritte)
        anzahlSchritte = maxSchritte;

    // DEBUG_MSG("Read Plan1 Anzahl Schritte: %d\n", anzahlSchritte);
    int i = 0;
    for (JsonObject ablaufObj : ablaufArray)
    {
        if (i < anzahlSchritte)
        {
            // JsonObject responseObj = docOut.createNestedObject();
            JsonObject responseObj = docOut.add<JsonObject>();
            responseObj["mv1druck"] = ablaufObj["MV1 Druck"].as<float>();
            responseObj["mv1dauer"] = ablaufObj["MV1 Dauer"].as<unsigned long>();
            responseObj["mv1pause"] = ablaufObj["MV1 Pause"].as<unsigned long>();
            responseObj["mv2druck"] = ablaufObj["MV2 Druck"].as<float>();
            responseObj["mv2dauer"] = ablaufObj["MV2 Dauer"].as<unsigned long>();
            responseObj["mv2pause"] = ablaufObj["MV2 Pause"].as<unsigned long>();

            // DEBUG_MSG("readablaufPlan1 Schritt #%d DruckMV1: %f MV1Open: %lu MV1Close: %lu\n", i, structPlan1[i].zieldruckMV1, structPlan1[i].intervallMV1Open, structPlan1[i].intervallMV1Close);
            // DEBUG_MSG("readablaufPlan1 Schritt #%d DruckMV2: %f MV2Open: %lu MV2Close: %lu\n", i, structPlan1[i].zieldruckMV2, structPlan1[i].intervallMV2Open, structPlan1[i].intervallMV2Close);
            i++;
        }
    }

    planResponse = "";
    serializeJson(docOut, planResponse);
    planfile.close();
    server.send_P(200, "application/json", planResponse.c_str());
}

void handleRequestPlan1()
{
    planfile = LittleFS.open(ABLAUFPLAN1, "r");
    if (planfile)
        handlePlan();
    planfile.close();
}
void handleRequestPlan2()
{
    planfile = LittleFS.open(ABLAUFPLAN2, "r");
    if (planfile)
        handlePlan();
    planfile.close();
}
void handleRequestPlan3()
{
    planfile = LittleFS.open(ABLAUFPLAN3, "r");
    handlePlan();
    planfile.close();
}

void handleSetPlan()
{
    uint8_t id = server.arg(0).toInt();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg(1));
    if (error)
    {
        DEBUG_MSG("error deserializeJson setPlan: %s", error.c_str());
        server.send(500, FPSTR("text/plain"), "Server error\r\n");
        return;
    }

    if (id == 1)
        planfile = LittleFS.open(ABLAUFPLAN1, "w");
    else if (id == 2)
        planfile = LittleFS.open(ABLAUFPLAN3, "w");
    else if (id == 3)
        planfile = LittleFS.open(ABLAUFPLAN3, "w");
    if (planfile)
    {
        serializeJson(doc, planfile);
        planfile.close();
        server.send(200, "text/plain", "ok");
    }
    else
        server.send(500, "text/plain", "Server error");
}

/*void handleSetPlan1()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg(0));
    if (error)
    {
        // DEBUG_MSG("Plan1: deserialize Json error %s\n", error.c_str());
        return;
    }
    File planfile = LittleFS.open(ABLAUFPLAN1, "w");
    if (planfile)
    {
        serializeJson(doc, planfile);
        planfile.close();
        server.send(200, "text/plain", "ok");
    }
    else
        server.send(500, "text/plain", "Server error");
}

void handleSetPlan2()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg(0));
    if (error)
    {
        // DEBUG_MSG("Plan2: deserialize Json error %s\n", error.c_str());
        return;
    }
    File planfile = LittleFS.open(ABLAUFPLAN2, "w");
    if (planfile)
    {
        serializeJson(doc, planfile);
        planfile.close();
        server.send(200, "text/plain", "ok");
    }
    else
        server.send(500, "text/plain", "Server error");
}
void handleSetPlan3()
{
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg(0));
    if (error)
    {
        // DEBUG_MSG("Plan3: deserialize Json error %s\n", error.c_str());
        server.send(500, "text/plain", error.c_str());
        return;
    }
    File planfile = LittleFS.open(ABLAUFPLAN3, "w");
    if (planfile)
    {
        serializeJson(doc, planfile);
        planfile.close();
        server.send(200, "text/plain", "ok");
    }
    else
        server.send(500, "text/plain", "Server error");
}*/

void handleRestore()
{
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = "config.txt"; // upload.filename;
        if (!filename.startsWith("/"))
            filename = "/" + filename;
        // DEBUG_MSG("WEB restore config file: %s\n", filename.c_str());
        fsUploadFile = LittleFS.open(filename, "w"); // Open the file for writing in LittleFS (create if it doesn't exist)
        filename = String();
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (fsUploadFile)
            fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (fsUploadFile)
        {                         // If the file was successfully created
            fsUploadFile.close(); // Close the file again
            // DEBUG_MSG("WEB restore configuration Size: %d\n", upload.totalSize);
            server.sendHeader("Location", "/index"); // Redirect the client to the success page
            server.send(303);
            // server.send(201, "ok");
            rebootDevice();
        }
    }
}
