void handleRoot()
{
    server.sendHeader("Location", "/index.html", true); //Redirect to our html web page
    server.send(302, "text/plain", "");
}

void handleWebRequests()
{
    if (loadFromLittleFS(server.uri()))
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

bool loadFromLittleFS(String path)
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
    StaticJsonDocument<512> doc;
    doc["mdns"] = startMDNS;
    doc["mdns_name"] = nameMDNS;
    doc["pressure"] = ((int)(setPressure * 100)) / 100.0;
    doc["carbonation"] = ((int)(setCarbonation * 100)) / 100.0;
    doc["mode"] = setMode;
    doc["co2"] = ((int)(calcCarbonation(pressure, temperature) * 100)) / 100.0;
    doc["druck"] = ((int)(pressure * 100)) / 100.0;
    doc["temperatur"] = ((int)(temperature * 10)) / 10.0;
    doc["targettemp"] = ((int)(targetTemp * 10)) / 10.0;
    doc["offset"] = offset0;
    doc["offset2"] = offset2;
    doc["mv1"] = startMV1;
    doc["mv2"] = startMV2;
    doc["co2sen"] = startCO2;
    doc["wertco2"] = wertCO2;
    doc["controller"] = controller;
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
    doc["gpio"] = setGPIO;
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
        doc["delayspund"] = minKarbonisierung;
    doc["dichtheit"] = ((int)(ergDichtheit * 1000)) / 1000.0;
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
        //berechneCountdown(lastTimeSpundomat + PAUSE5MIN + PAUSE2MIN);
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleRequestMisc()
{
    StaticJsonDocument<512> doc;
    doc["mdns"] = startMDNS;
    doc["mdns_name"] = nameMDNS;
    doc["pressure"] = setPressure;
    doc["carbonation"] = setCarbonation;
    doc["mv1"] = startMV1;
    doc["mv2"] = startMV2;
    doc["mv1open"] = mv1Open;
    doc["mv1close"] = mv1Close;
    doc["mv2open"] = mv2Open;
    doc["mv2close"] = mv2Close;
    doc["testmode"] = testModus;
    doc["uppressure"] = upPressure;
    doc["uptemp"] = upTemp;
    if (setEinheit == 0 || setEinheit == 1)
        doc["verzkombi"] = (int)verzKombi;
    else
        doc["verzkombi"] = verzKombi;
    doc["offset"] = offset0;
    doc["offset2"] = offset2;
    doc["dbserver"] = dbServer;
    doc["startdb"] = startDB;
    doc["dbdatabase"] = dbDatabase;
    doc["dbuser"] = dbUser;
    doc["dbuser"] = dbPass;
    doc["dbup"] = upInflux / 1000;
    doc["vistag"] = dbVisTag;
    doc["co2sen"] = startCO2;
    doc["rssi"] = WiFi.RSSI();
    doc["uptarget"] = upTarget;
    doc["targettemp"] = targetTemp;
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleRequestFirm()
{
    String request = server.arg(0);
    String message;
    if (request == "firmware")
    {
        if (startMDNS)
        {
            message = nameMDNS;
            message += " V";
        }
        else
            message = "Spundomat V ";
        message += Version;
        goto SendMessage;
    }

SendMessage:
    server.send(200, "text/plain", message);
}

void handleSetMisc()
{
    for (int i = 0; i < server.args(); i++)
    {
        if (server.argName(i) == "eeprom")
        {
            if (server.arg(i) == "true")
            {
                eraseEeprom();
            }
        }
        if (server.argName(i) == "reset")
        {
            if (server.arg(i) == "true")
            {
                WiFi.disconnect();
                wifiManager.resetSettings();
                unsigned long last = millis();
                millis2wait(PAUSE2SEC);
                ESP.reset();
            }
        }
        if (server.argName(i) == "clear")
        {
            if (server.arg(i) == "true")
            {
                LittleFS.remove("/config.txt");
                eraseEeprom();
                WiFi.disconnect();
                wifiManager.resetSettings();
                millis2wait(PAUSE2SEC);
                ESP.reset();
            }
        }
        if (server.argName(i) == "mdns_name")
        {
            server.arg(i).toCharArray(nameMDNS, 16);
            checkChars2(nameMDNS);
            if (strlen(nameMDNS) == 0)
                strlcpy(nameMDNS, "spundomat", sizeof(nameMDNS));
        }
        if (server.argName(i) == "mdns")
        {
            startMDNS = checkBool(server.arg(i));
        }
        if (server.argName(i) == "pressure")
        {
            if (checkRangeDruck(server.arg(i)))
                setPressure = formatDOT(server.arg(i));
        }
        if (server.argName(i) == "carbonation")
        {
            if (checkRangeCO2(server.arg(i)))
                setCarbonation = formatDOT(server.arg(i));
        }
        if (server.argName(i) == "mode")
        {
            if (isValidInt(server.arg(i)))
                setMode = server.arg(i).toInt();
            newMode = setMode;
        }
        if (server.argName(i) == "mv1")
        {

            startMV1 = checkBool(server.arg(i));
            if (startMV1)
                pinMode(PIN_MV1, OUTPUT);
        }
        if (server.argName(i) == "mv1open")
        {
            if (isValidInt(server.arg(i)))
            {
                if (checkRange(server.arg(i)))
                    mv1Open = server.arg(i).toInt();
            }
        }
        if (server.argName(i) == "mv1close")
        {
            if (isValidInt(server.arg(i)))
            {
                if (checkRange2(server.arg(i)))
                    mv1Close = server.arg(i).toInt();
            }
        }
        if (server.argName(i) == "mv2")
        {

            startMV2 = checkBool(server.arg(i));
            if (startMV2)
                pinMode(PIN_MV2, OUTPUT);
        }
        if (server.argName(i) == "mv2open")
        {
            if (isValidInt(server.arg(i)))
            {
                if (checkRange(server.arg(i)))
                    mv2Open = server.arg(i).toInt();
            }
        }
        if (server.argName(i) == "mv2close")
        {
            if (isValidInt(server.arg(i)))
            {
                if (checkRange(server.arg(i)))
                    mv2Close = server.arg(i).toInt();
            }
        }
        if (server.argName(i) == "testmode")
        {
            testModus = checkBool(server.arg(i));
        }
        if (server.argName(i) == "uppressure")
        {
            if (isValidInt(server.arg(i)))
            {
                if (checkRange(server.arg(i)))
                    upPressure = server.arg(i).toInt();
            }
        }
        if (server.argName(i) == "uptemp")
        {
            if (isValidInt(server.arg(i)))
            {
                if (checkRange(server.arg(i)))
                    upTemp = server.arg(i).toInt();
            }
        }
        if (server.argName(i) == "einheit")
        {
            if (isValidInt(server.arg(i)))
                setEinheit = server.arg(i).toInt();
        }

        if (server.argName(i) == "verzkombi")
        {
            if (setEinheit == 0)
            {
                if (isValidInt(server.arg(i)))
                {
                    verzKombi = server.arg(i).toInt();
                    verzKarbonisierung = verzKombi * 1000 * 60;
                }
            }
            else if (setEinheit == 1)
            {
                if (isValidInt(server.arg(i)))
                {
                    verzKombi = server.arg(i).toInt();
                    verzKarbonisierung = verzKombi * 1000 * 60 * 60;
                }
            }
            else if (setEinheit == 2)
            {
                if (checkRangeCO2(server.arg(i)))
                {
                    verzKombi = formatDOT(server.arg(i));
                    verzKarbonisierung = 0;
                    minKarbonisierung = verzKombi;
                }
            }
        }
        if (server.argName(i) == "offset2")
        {
            offset2 = formatDOT(server.arg(i));
        }
        if (server.argName(i) == "dbserver")
        {
            server.arg(i).toCharArray(dbServer, 30);
            checkChars2(dbServer);
        }
        if (server.argName(i) == "startdb")
        {
            startDB = checkBool(server.arg(i));
        }
        if (server.argName(i) == "dbdatabase")
        {
            server.arg(i).toCharArray(dbDatabase, 15);
            checkChars2(dbDatabase);
            if (strlen(dbDatabase) == 0)
                strlcpy(dbDatabase, "spundomat", sizeof(dbDatabase));
        }
        if (server.argName(i) == "dbuser")
        {
            server.arg(i).toCharArray(dbUser, 15);
            checkChars2(dbUser);
        }
        if (server.argName(i) == "dbpass")
        {
            server.arg(i).toCharArray(dbPass, 15);
            checkChars2(dbPass);
        }

        if (server.argName(i) == "dbup")
        {
            if (isValidInt(server.arg(i)))
            {
                upInflux = server.arg(i).toInt() * 1000;
            }
        }
        if (server.argName(i) == "alertstate")
        {
            alertState = false;
        }
        if (server.argName(i) == "co2sen")
        {

            startCO2 = checkBool(server.arg(i));
        }
        if (server.argName(i) == "gpio")
        {
            if (isValidInt(server.arg(i)))
                setGPIO = server.arg(i).toInt();
        }
        if (server.argName(i) == "uptarget")
        {
            if (isValidInt(server.arg(i)))
            {
                if (checkRange(server.arg(i)))
                    upTarget = server.arg(i).toInt();
            }
        }
        if (server.argName(i) == "targettemp")
        {
            if (checkRange(server.arg(i)))
                targetTemp = formatDOT(server.arg(i));
        }
        yield();
    }
    server.send(201, "text/plain", "created");
    saveConfig();
    reflashLCD = true;
}

void eraseEeprom()
{
    offset0 = 0;
    offset2 = 0;
    for (int i = 0; i < 8; i++) // Lösche 4 bytes offset0 und 4bytes offset2 (float)
    {
        EEPROM.write(i, 0);
        EEPROM.commit();
    }
    server.send(201, "text/plain", "erased");
    saveConfig();
}

void visualisieren()
{
    for (int i = 0; i < server.args(); i++)
    {
        if (server.argName(i) == "vistag")
        {
            server.arg(i).toCharArray(dbVisTag, 15);
            checkChars2(dbVisTag);
        }
        if (server.argName(i) == "startvis")
        {
            if (server.arg(i) == "true")
            {
                startVis = true;
                TickerInfluxDB.interval(upInflux);
                TickerInfluxDB.start();
            }
            else
            {
                startVis = false;
                TickerInfluxDB.stop();
            }
        }
        yield();
    }
    if (startDB && startVis)
    {
        TickerInfluxDB.resume();
        sendDBData();
    }
    else
        TickerInfluxDB.pause();

    server.send(201, "text/plain", "created");
}

void kalibrieren()
{
    server.send(200, "text/plain", "kalibrieren...");
    if (offset0 == 0.0) // Keine Kalibrierung bei 0 bar
    {
        offset0 = readSensor();
        writeFloat(0, offset0);
        DEBUG_MSG("*** 1. Kalibrierung offset0: %f\n", offset0);
    }
    else if (offset0 > 0 && readSensor() < 200.0)
    {
        offset0 = readSensor();
        writeFloat(0, offset0);
        DEBUG_MSG("*** Re-Kalibrierung offset0: %f\n", offset0);
    }
    else if (offset0 > 0 && readSensor() > 200.0) // 2bar ca. 330
    {
        offset2 = readSensor();
        writeFloat(4, offset2);
        DEBUG_MSG("*** 2. Kalibrierung offset0: %f offset2: %f\n", offset0, offset2);
    }

    readPressure();
    // saveConfig();
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
    server.send(200, "text/plain", message);
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
    server.send(200, "text/plain", message);
}

void handlereqGPIO()
{
    String message;
    message += F("<option>");
    message += modesGPIO[setGPIO];
    message += F("</option><option disabled>──────────</option>");
    for (int i = 0; i < sizeOfGPIO; i++)
    {
        if (setGPIO != i)
        {
            message += F("<option value=\"");
            message += i;
            message += "\">";
            message += modesGPIO[i];
            message += F("</option>");
        }
    }
    yield();
    server.send(200, "text/plain", message);
}

void rebootDevice()
{
    server.send(205, "text/plain", "rebooting...");
    LittleFS.end(); // unmount LittleFS
    ESP.restart();
}

void setMDNS()
{
    if (startMDNS && nameMDNS[0] != '\0' && WiFi.status() == WL_CONNECTED)
    {
        if (MDNS.begin(nameMDNS))
            Serial.printf("*** SYSINFO: mDNS gestartet als %s verbunden an %s Time: %s RSSI: %d\n", nameMDNS, WiFi.localIP().toString().c_str(), timeClient.getFormattedTime().c_str(), WiFi.RSSI());
    }
    else
        Serial.printf("*** SYSINFO: Fehler Start mDNS! IP Adresse: %s Time: %s RSSI: %d\n", WiFi.localIP().toString().c_str(), timeClient.getFormattedTime().c_str(), WiFi.RSSI());
}

void startDichtheit()
{
    DEBUG_MSG("%s\n", "Start Überprüfung Dichtheit");
    setMode = DICHTHEIT;
    lastTimeSpundomat = millis();
    dichtPressure = pressure;
}
