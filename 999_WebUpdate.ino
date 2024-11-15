// WebUpdate Board configuration ESP8266
// "configuration": "xtal=80,vt=flash,exception=disabled,stacksmash=disabled,ssl=basic,mmu=4816H,non32xfer=fast,eesz=4M2M,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600",

bool upTools(String url, String fname, WiFiClientSecure &clientup)
{
    HTTPClient https;
    char line[120];
    millis2wait(100);
    if (https.begin(clientup, url + fname))
    {
        int16_t httpCode = https.GET();
        if (httpCode > 0)
        {
            if (httpCode == HTTP_CODE_OK)
            {
                uint8_t buff[128] = {0};
                WiFiClient *stream = https.getStreamPtr(); // get tcp stream
                bool check = LittleFS.remove("/" + fname);
                fsUploadFile = LittleFS.open("/" + fname, "w");
                int32_t len = https.getSize();
                int32_t startlen = len;
                while (https.connected() && (len > 0 || len == -1))
                {
                    size_t size = stream->available(); // get available data size
                    if (size > 0)
                    {
                        int32_t c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size)); // lese maximal 128 byte in buff
                        fsUploadFile.write(buff, c);                                                        // schreibe buff in Datei
                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                fsUploadFile.close();
                fsUploadFile = LittleFS.open("/" + fname, "r");
                if (fsUploadFile.size() == startlen)
                {
                    sprintf(line, "Framework/Tools update %s getSize: %d fileSize: %d", fname.c_str(), startlen, fsUploadFile.size());
                    debugLog(UPDATELOG, line);
                    DEBUG_MSG("%s", line);
                }
                else
                {
                    sprintf(line, "Framework/Tools update Fehler %s getSize: %d fileSize: %d", fname.c_str(), startlen, fsUploadFile.size());
                    debugLog(UPDATELOG, line);
                    DEBUG_MSG("%s", line);
                }
                fsUploadFile.close();
                https.end();
                return true;
            }
            else
                return false;
        }
        else
        {
            sprintf(line, "Framework/Tools update Fehler %s %s", fname.c_str(), https.errorToString(httpCode).c_str());
            debugLog(UPDATELOG, line);
            DEBUG_MSG("%s", line);
            https.end();
            return false;
        }
    }
    else
    {
        sprintf(line, "Framework/Tools update Fehler https start %s", fname.c_str());
        debugLog(UPDATELOG, line);
        DEBUG_MSG("%s", line);
        return false;
    }
}

void upFirm()
{
    WiFiClientSecure clientup;
    clientup.setInsecure();
    char line[120];
    ESPhttpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onError(update_error);
    t_httpUpdate_return ret;

    if (LittleFS.exists(DEVBRANCH))
        ret = ESPhttpUpdate.update(clientup, "https://raw.githubusercontent.com/InnuendoPi/Spundomat/development/build/Spundomat.ino.bin");
    else
        ret = ESPhttpUpdate.update(clientup, "https://raw.githubusercontent.com/InnuendoPi/Spundomat/master/build/Spundomat.ino.bin");

    return;
}

void updateTools()
{
    configTzTime(ntpZone, ntpServer);
    getLocalTime(&timeinfo);
    strftime(zeit, sizeof(zeit), "%H:%M:%S", &timeinfo);

    WiFiClientSecure clientup;
    clientup.setInsecure();
    fsUploadFile = LittleFS.open(LOGUPDATETOOLS, "r");
    int8_t anzahlVersuche = 0;
    if (fsUploadFile)
    {
        char anzahlV = char(fsUploadFile.read()) - '0';
        anzahlVersuche = (int8_t)anzahlV;
    }
    fsUploadFile.close();
    if (anzahlVersuche > 3)
    {
        LittleFS.remove(UPDATETOOLS);
        DEBUG_MSG("ERROR update tools - %d", anzahlVersuche);
        return;
    }
    anzahlVersuche++;
    fsUploadFile = LittleFS.open(LOGUPDATETOOLS, "w");
    int32_t bytesWritten = fsUploadFile.print((anzahlVersuche));
    fsUploadFile.close();
    DEBUG_MSG("ESP8266 IP address: %s time: %s WLAN Signal: %d", WiFi.localIP().toString().c_str(), zeit, WiFi.RSSI());
    char line[120];
    sprintf(line, "Update Framework/Tools #%d started - free heap: %d", anzahlVersuche, ESP.getFreeHeap());
    debugLog(UPDATELOG, line);
    DEBUG_MSG("%s", line);
    if (anzahlVersuche == 1)
    {
        sprintf(line, "Firmware Version: %s", Version);
        debugLog(UPDATELOG, line);
        DEBUG_MSG("%s", line);
    }
    bool test;
    if (LittleFS.exists(DEVBRANCH))
    {
        test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "spundomat.ttf", clientup);
        test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "spundomat.min.css", clientup);
        test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "spundomat.min.js", clientup);
        test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "favicon.ico", clientup);
        // test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "ce.rts", clientup);

        // test = upTools("https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "ce.rts");
        // test = upTools("https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "spundomat.ttf");
        // test = upTools("https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "spundomat.min.css");
        // test = upTools("https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "spundomat.min.js");
        // test = upTools("https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/development/data/", "favicon.ico");
    }
    else
    {
        test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/", "spundomat.ttf", clientup);
        test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/", "spundomat.min.css", clientup);
        test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/", "spundomat.min.js", clientup);
        test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/", "favicon.ico", clientup);
        // test = upTools("https://raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/", "ce.rts", clientup);
    }

    LittleFS.remove(UPDATETOOLS);
    LittleFS.end();
    millis2wait(1000);
    ESP.restart();
}

void updateSys()
{
    configTzTime(ntpZone, ntpServer);
    getLocalTime(&timeinfo);
    strftime(zeit, sizeof(zeit), "%H:%M:%S", &timeinfo);
    fsUploadFile = LittleFS.open(LOGUPDATESYS, "r");

    int anzahlVersuche = 0;
    if (fsUploadFile)
    {
        char anzahlV = char(fsUploadFile.read()) - '0';
        anzahlVersuche = (int)anzahlV;
    }
    fsUploadFile.close();
    if (anzahlVersuche > 3)
    {
        LittleFS.remove(UPDATESYS);
        Serial.println("*** SYSINFO: ERROR update firmware");
        return;
    }
    fsUploadFile = LittleFS.open(LOGUPDATESYS, "w");
    anzahlVersuche++;
    int bytesWritten = fsUploadFile.print(anzahlVersuche);
    fsUploadFile.close();
    Serial.printf("*** SYSINFO: WebUpdate firmware #%d gestartet - free heap: %d\n", anzahlVersuche, ESP.getFreeHeap());
    if (anzahlVersuche == 1)
        bool check = LittleFS.remove(UPDATELOG);

    char line[120];
    sprintf(line, "WebUpdate firmware #%d started - free heap: %d", anzahlVersuche, ESP.getFreeHeap());
    debugLog(UPDATELOG, line);
    DEBUG_MSG("%s", line);
    sprintf(line, "Firmware Version: %s", Version);
    debugLog(UPDATELOG, line);
    DEBUG_MSG("%s", line);
    upFirm();
}

void startHTTPUpdate()
{
    replyOK();
    fsUploadFile = LittleFS.open(UPDATESYS, "w");
    if (!fsUploadFile)
    {
        DEBUG_MSG("%s", "Error WebUpdate firmware create file (LittleFS)");
        return;
    }
    else
    {
        DEBUG_MSG("%s", "WebUpdate firmware create file (LittleFS)");
        int32_t bytesWritten = fsUploadFile.print("0");
        fsUploadFile.close();
    }
    char line[120];
    bool check = LittleFS.remove(UPDATELOG);
    debugLog(UPDATELOG, "WebUpdate started");
    sprintf(line, "Firmware version: %s", Version);
    debugLog(UPDATELOG, line);
    DEBUG_MSG("%s", line);
    if (devBranch)
    {
        fsUploadFile = LittleFS.open(DEVBRANCH, "w");
        if (!fsUploadFile)
        {
            DEBUG_MSG("%s", "Error WebUpdate firmware dev create file (LittleFS)");
            return;
        }
        else
        {
            DEBUG_MSG("%s", "WebUpdate firmware dev create file (LittleFS)");
            int32_t bytesWritten = fsUploadFile.print("0");
            debugLog(UPDATELOG, "WebUpdate development branch ausgewählt");
            fsUploadFile.close();
        }
    }
    else
    {
        if (LittleFS.exists(DEVBRANCH))
            bool check = LittleFS.remove(DEVBRANCH);
    }
    DEBUG_MSG("%s", "WebUpdate Firmware reboot");
    LittleFS.end();
    millis2wait(1000);
    ESP.restart();
}

void update_finished()
{
    debugLog(UPDATELOG, "Firmware update successful");
    DEBUG_MSG("%s", "Firmware update finished");
    fsUploadFile = LittleFS.open(UPDATETOOLS, "w");
    if (fsUploadFile)
    {
        int32_t bytesWritten = fsUploadFile.print(0);
        fsUploadFile.close();
    }
    LittleFS.remove(UPDATESYS);
    LittleFS.end();
    millis2wait(1000);
    ESP.restart();
}

void update_error(int16_t err)
{
    char line[120];
#ifdef ESP32
    sprintf(line, "Firmware update error code %d: %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
#elif ESP8266
    sprintf(line, "Firmware update error code %d: %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
#endif
    debugLog(UPDATELOG, line);
    DEBUG_MSG("%s", line);
    LittleFS.end();
    millis2wait(1000);
    ESP.restart();
}

void checkLog()
{
    if (LittleFS.exists(LOGUPDATETOOLS)) // WebUpdate Firmware
    {
        fsUploadFile = LittleFS.open(LOGUPDATETOOLS, "r");
        int anzahlTools = 0;
        if (fsUploadFile)
        {
            anzahlTools = char(fsUploadFile.read()) - '0';
        }
        fsUploadFile.close();

        bool check = LittleFS.remove(LOGUPDATETOOLS);
        if (LittleFS.exists(DEVBRANCH)) // WebUpdate Firmware
        {
            // Serial.printf("*** SYSINFO: Update tools devbranch retries count: %d\n", anzahlTools);
            // check = LittleFS.remove(DEVBRANCH);
        }
        // else
        // {
        //     Serial.printf("*** SYSINFO: Update tools retries count: %d\n", anzahlTools);
        // }
    }

    if (LittleFS.exists(LOGUPDATESYS)) // WebUpdate Firmware
    {
        fsUploadFile = LittleFS.open(LOGUPDATESYS, "r");
        int anzahlSys = 0;
        if (fsUploadFile)
        {
            anzahlSys = char(fsUploadFile.read()) - '0';
        }
        fsUploadFile.close();
        bool check = LittleFS.remove(LOGUPDATESYS);
        if (LittleFS.exists(DEVBRANCH)) // WebUpdate Firmware
        {
            // Serial.printf("*** SYSINFO: Update firmware devbranch retries count: %d\n", anzahlSys);
            check = LittleFS.remove(DEVBRANCH);
        }
        else
        {
            // Serial.printf("*** SYSINFO: Update firmware retries count: %d\n", anzahlSys);
        }
        alertState = true;
    }
}

void debugLog(String valFile, String valText)
{
    File debLog = LittleFS.open(valFile, "a");
    if (debLog)
    {
        debLog.print(zeit);
        debLog.print("\t");
        debLog.print(valText);
        debLog.print("\n");
        debLog.close();
    }
}
