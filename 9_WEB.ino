void handleRoot()
{
    server.sendHeader("Location", "/index.html", true); //Redirect to our html web page
    server.send(302, "text/plain", "");
}

void handleWebRequests()
{
    if (loadFromSpiffs(server.uri()))
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

bool loadFromSpiffs(String path)
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

    if (!SPIFFS.exists(path.c_str()))
    {
        return false;
    }
    File dataFile = SPIFFS.open(path.c_str(), "r");
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
    StaticJsonDocument<256> doc;
    doc["mdns"] = startMDNS;
    doc["mdns_name"] = nameMDNS;
    doc["pressure"] = setPressure;
    doc["carbonation"] = setCarbonation;
    doc["mode"] = setMode;
    // char sPressure[5];
    // char sCalcCarb[5];
    // dtostrf(pressure, 5, 1, sPressure);
    // dtostrf(calcCarbonation(pressure, temperature), 5, 1, sCalcCarb);
    // doc["co2"] = calcCarbonation(pressure, temperature);
    // doc["druck"] = pressure;
    // doc["co2"] = sCalcCarb;
    // doc["druck"] = sPressure;

    doc["co2"] = (int)(calcCarbonation(pressure, temperature) * 100) / 100.0;
    doc["druck"] = (int)(pressure * 100) / 100.0;
    doc["temperatur"] = (int)(temperature * 10) / 10.0;
    doc["voltage"] = (int)(voltage * 100) / 100.0;
    doc["offset"] = (int)(offsetVoltage * 100) / 100.0;
    doc["mv1"] = startMV1;
    doc["mv2"] = startMV2;
    doc["buzzer"] = startBuzzer;
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleRequestMisc()
{
    String request = server.arg(0);
    String message;
    if (request == "mdns_name")
    {
        message = nameMDNS;
        goto SendMessage;
    }
    if (request == "mdns")
    {
        if (startMDNS)
            message = "1";
        else
            message = "0";
        goto SendMessage;
    }
    if (request == "pressure")
    {
        message = setPressure;
        goto SendMessage;
    }
    if (request == "carbonation")
    {
        message = setCarbonation;
        goto SendMessage;
    }
    if (request == "mode")
    {
        message = setMode;
        goto SendMessage;
    }
    if (request == "mv1")
    {
        if (startMV1)
            message = "1";
        else
            message = "0";
        goto SendMessage;
    }
    if (request == "mv1open")
    {
        message = mv1Open;
        goto SendMessage;
    }
    if (request == "mv1close")
    {
        message = mv1Close;
        goto SendMessage;
    }
    if (request == "mv2")
    {
        if (startMV2)
            message = "1";
        else
            message = "0";
        goto SendMessage;
    }
    if (request == "mv2open")
    {
        message = mv2Open;
        goto SendMessage;
    }
    if (request == "mv2close")
    {
        message = mv2Close;
        goto SendMessage;
    }
    if (request == "buzzer")
    {
        if (startBuzzer)
            message = "1";
        else
            message = "0";
        goto SendMessage;
    }
    if (request == "testmode")
    {
        if (testModus)
            message = "1";
        else
            message = "0";
        goto SendMessage;
    }
    if (request == "uppressure")
    {
        message = upPressure;
        goto SendMessage;
    }
    if (request == "uptemp")
    {
        message = upTemp;
        goto SendMessage;
    }
    if (request == "firmware")
    {
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
            if (server.arg(i) == "1")
            {
                eraseEeprom();
            }
        }
        if (server.argName(i) == "reset")
        {
            if (server.arg(i) == "1")
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
            if (server.arg(i) == "1")
            {
                SPIFFS.remove("/config.txt");
                WiFi.disconnect();
                wifiManager.resetSettings();
                millis2wait(PAUSE2SEC);
                ESP.reset();
            }
        }
        if (server.argName(i) == "mdns_name")
            server.arg(i).toCharArray(nameMDNS, 16);
        if (server.argName(i) == "mdns")
        {
            if (server.arg(i) == "1")
                startMDNS = true;
            else
                startMDNS = false;
        }
        if (server.argName(i) == "pressure")
        {
            setPressure = formatDOT(server.arg(i));
        }
        if (server.argName(i) == "carbonation")
        {
            setCarbonation = formatDOT(server.arg(i));
        }
        if (server.argName(i) == "mode")
        {
            for (int j = 0; j < sizeOfModes; j++)
            {
                if (server.arg(i) == modesWeb[j])
                    setMode = j;
            }
        }
        if (server.argName(i) == "mv1")
        {
            if (server.arg(i) == "1")
            {
                startMV1 = true;
                pinMode(PIN_MV1, OUTPUT);
            }
            else
            {
                startMV1 = false;
                pinMode(PIN_MV1, INPUT);
            }
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
                if (checkRange(server.arg(i)))
                    mv1Close = server.arg(i).toInt();
            }
        }
        if (server.argName(i) == "mv2")
        {
            if (server.arg(i) == "1")
            {
                startMV2 = true;
                pinMode(PIN_MV2, OUTPUT);
            }
            else
            {
                startMV2 = false;
                pinMode(PIN_MV2, INPUT);
            }
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
        if (server.argName(i) == "buzzer")
        {
            if (server.arg(i) == "1")
            {
                startBuzzer = true;
                pinMode(PIN_BUZZER, OUTPUT);
            }
            else
            {
                startBuzzer = false;
                pinMode(PIN_BUZZER, INPUT);
            }
        }
        if (server.argName(i) == "testmode")
        {
            if (server.arg(i) == "1")
                testModus = true;

            else
                testModus = false;
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
        yield();
    }
    saveConfig();
    reflashLCD = true;
}

void eraseEeprom()
{
    writeFloat(0, 0.00);
}

void kalibrieren()
{
    DEBUG_MSG("%s\n", "*** Kalibrierung");
    server.send(200, "text/plain", "kalibrieren...");
    readPressure();
    offsetVoltage = voltage;
    //offsetVoltage = (0.5 - voltage);
    writeFloat(0, offsetVoltage);
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
            message += F("<option>");
            message += modesWeb[i];
            message += F("</option>");
        }
    }
    yield();
    server.send(200, "text/plain", message);
}

void rebootDevice()
{
    server.send(200, "text/plain", "rebooting...");
    SPIFFS.end(); // unmount SPIFFS
    ESP.restart();
}

void setMDNS()
{
    if (startMDNS && nameMDNS[0] != '\0' && WiFi.status() == WL_CONNECTED)
    {
        if (MDNS.begin(nameMDNS))
            Serial.printf("*** SYSINFO: mDNS gestartet als %s verbunden an %s\n", nameMDNS, WiFi.localIP().toString().c_str());
    }
    else
        Serial.printf("%s\n", "*** SYSINFO: Fehler Start mDNS! IP Adresse: %s\n", WiFi.localIP().toString().c_str());
}
