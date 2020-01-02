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
    doc["debug"] = setDEBUG;
    doc["telnet"] = startTEL;
    doc["pressure"] = setPressure;
    doc["carbonation"] = setCarbonation;
    doc["mode"] = setMode;
    doc["co2"] = calcCarbonation(pressure, temperature);
    doc["druck"] = pressure;
    doc["temperatur"] = sTemperature;
    doc["voltage"] = voltage;
    doc["offset"] = offsetVoltage;

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
        {
            message = "1";
        }
        else
        {
            message = "0";
        }
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
    if (request == "debug")
    {
        if (setDEBUG)
        {
            message = "1";
        }
        else
        {
            message = "0";
        }
        goto SendMessage;
    }
    if (request == "telnet")
    {
        if (startTEL)
        {
            message = "1";
        }
        else
        {
            message = "0";
        }
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
                SPIFFS.remove("/config.json");
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
            setPressure = server.arg(i).toFloat();
        }
        if (server.argName(i) == "carbonation")
        {
            setCarbonation = server.arg(i).toFloat();
        }
        if (server.argName(i) == "mode")
        {
            for (int j = 0; j < sizeOfModes; j++)
            {
                if (server.arg(i) == modesWeb[j])
                    setMode = j;
            }
        }
        if (server.argName(i) == "debug")
        {
            if (server.arg(i) == "1")
                setDEBUG = true;
            else
                setDEBUG = false;
        }
        if (server.argName(i) == "telnet")
        {
            if (server.arg(i) == "1")
                startTEL = true;
            else
            {
                if (Telnet)
                    Telnet.stop();
                startTEL = false;
            }
        }
        yield();
    }
    saveConfig();
    reflashLCD = true;
}

// Some helper functions WebIf
void rebootDevice()
{
    server.send(200, "text/plain", "rebooting...");
    SPIFFS.end(); // unmount SPIFFS
    ESP.restart();
}

void checkTELNET()
{
    if (TelnetServer.hasClient())
    {
        if (!Telnet || !Telnet.connected())
        {
            if (Telnet)
            {
                Telnet.stop();
            }
            Telnet = TelnetServer.available();
        }
        else
        {
            TelnetServer.available().stop();
        }
    }
}

void setMDNS()
{
    if (startMDNS && nameMDNS[0] != '\0' && WiFi.status() == WL_CONNECTED)
    {
        if (!mdns.begin(nameMDNS, WiFi.localIP()))
        {
            DBG_PRINT("*** SYSINFO: Fehler mDNS!");
            startMDNS = false;
        }
        else
        {
            mdns.begin(nameMDNS);
            DBG_PRINT("*** SYSINFO: MDNS gestartet als ");
            DBG_PRINT(nameMDNS);
            DBG_PRINT(" verbunden an ");
            IPAddress ip = WiFi.localIP();
            DBG_PRINTLN(ip.toString());
        }
    }
}

void setTELNET()
{
    TelnetServer.begin();
    TelnetServer.setNoDelay(true);
    DBG_PRINTLN("*** SYSINFO: Connect your telnet Client, exit with ^] and 'quit'");
}

void kalibrieren()
{
    DBG_PRINTLN("*** Kalibrierung");
    server.send(200, "text/plain", "kalibrieren...");
    readPressure();
    offsetVoltage = voltage;
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
    // {"Aus", "CO2 Spund", "Druck Spund", "CO2 Zwang"};
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
