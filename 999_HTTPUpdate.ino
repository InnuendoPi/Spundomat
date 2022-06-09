void upIn()
{
    std::unique_ptr<BearSSL::WiFiClientSecure> clientup(new BearSSL::WiFiClientSecure);
    clientup->setInsecure();

    HTTPClient https;

    if (https.begin(*clientup, "https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/index.html"))
    {
        int httpCode = https.GET();
        if (httpCode > 0)
        {
            // HTTP header has been send and Server response header has been handled
            // Serial.printf("*** SYSINFO: [HTTPS] GET index.html Antwort: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK)
            {

                // get lenght of document (is -1 when Server sends no Content-Length header)
                int len = https.getSize();

                // create buffer for read
                static uint8_t buff[128] = {0};

                // Open file for write
                fsUploadFile = LittleFS.open("/index.html", "w");
                if (!fsUploadFile)
                {
                    //Serial.printf( F("file open failed"));
                    Serial.println("Abbruch!");
                    Serial.println("*** SYSINFO: Fehler beim Speichern index.html");
                    https.end();
                    return;
                }

                // read all data from server
                while (https.connected() && (len > 0 || len == -1))
                {
                    // get available data size
                    size_t size = clientup->available();
                    //Serial.printf("*** SYSINFO: [HTTPS] index size avail: %d\n", size);

                    if (size)
                    {
                        //read up to 128 byte
                        int c = clientup->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                        // write it to file
                        fsUploadFile.write(buff, c);

                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                Serial.println("*** SYSINFO: Update Index abgeschlossen.");
                // Close LittleFS file
                fsUploadFile.close();
                bool check = LittleFS.remove("/update.txt");
                fsUploadFile = LittleFS.open("/update0.txt", "w");
                int bytesWritten = fsUploadFile.print("0");
                fsUploadFile.close();
            }
            else
                return;
        }
        else
        {
            Serial.println("Abbruch!");
            Serial.printf("*** SYSINFO: Update index.html Fehler: %s\n", https.errorToString(httpCode).c_str());
            https.end();
            LittleFS.end(); // unmount LittleFS
            ESP.restart();

            return;
        }
        https.end();
        return;
    }
    return;
}
void upAb()
{
    std::unique_ptr<BearSSL::WiFiClientSecure> clientup(new BearSSL::WiFiClientSecure);
    clientup->setInsecure();

    HTTPClient https;

    if (https.begin(*clientup, "https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/ablaufplan.html"))
    {
        int httpCode = https.GET();
        if (httpCode > 0)
        {
            // HTTP header has been send and Server response header has been handled
            // Serial.printf("*** SYSINFO: [HTTPS] GET index.html Antwort: %d\n", httpCode);

            // file found at server
            if (httpCode == HTTP_CODE_OK)
            {

                // get lenght of document (is -1 when Server sends no Content-Length header)
                int len = https.getSize();

                // create buffer for read
                static uint8_t buff[128] = {0};

                // Open file for write
                fsUploadFile = LittleFS.open("/ablaufplan.html", "w");
                if (!fsUploadFile)
                {
                    //Serial.printf( F("file open failed"));
                    Serial.println("Abbruch!");
                    Serial.println("*** SYSINFO: Fehler beim Speichern ablaufplan.html");
                    https.end();
                    return;
                }

                // read all data from server
                while (https.connected() && (len > 0 || len == -1))
                {
                    // get available data size
                    size_t size = clientup->available();
                    //Serial.printf("*** SYSINFO: [HTTPS] index size avail: %d\n", size);

                    if (size)
                    {
                        //read up to 128 byte
                        int c = clientup->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                        // write it to file
                        fsUploadFile.write(buff, c);

                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                Serial.println("*** SYSINFO: Update Ablaufplan abgeschlossen.");
                // Close LittleFS file
                fsUploadFile.close();
                bool check = LittleFS.remove("/update0.txt");
                fsUploadFile = LittleFS.open("/update1.txt", "w");
                int bytesWritten = fsUploadFile.print("0");
                fsUploadFile.close();
            }
            else
                return;
        }
        else
        {
            Serial.println("Abbruch!");
            Serial.printf("*** SYSINFO: Update index.html Fehler: %s\n", https.errorToString(httpCode).c_str());
            https.end();
            LittleFS.end(); // unmount LittleFS
            ESP.restart();

            return;
        }
        https.end();
        return;
    }
    return;
}

void upCSS()
{
    std::unique_ptr<BearSSL::WiFiClientSecure> clientup(new BearSSL::WiFiClientSecure);
    clientup->setInsecure();
    HTTPClient https;
    String indexURL;
    indexURL = "https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/bootstrap.min.css";
    if (https.begin(*clientup, indexURL))
    {
        int httpCode = https.GET();
        if (httpCode > 0)
        {
            if (httpCode == HTTP_CODE_OK)
            {
                int len = https.getSize();
                static uint8_t buff[128] = {0};
                fsUploadFile = LittleFS.open("/bootstrap.min.css", "w");
                if (!fsUploadFile)
                {
                    Serial.println("Abbruch!");
                    Serial.println("*** SYSINFO: Fehler beim Speichern bootstrap css");
                    https.end();
                    return;
                }
                while (https.connected() && (len > 0 || len == -1))
                {
                    size_t size = clientup->available();
                    if (size)
                    {
                        int c = clientup->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        fsUploadFile.write(buff, c);
                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                Serial.println("*** SYSINFO: Update bootstrap css abgeschlossen");
                fsUploadFile.close();
                bool check = LittleFS.remove("/update1.txt");
                fsUploadFile = LittleFS.open("/update11.txt", "w");
                int bytesWritten = fsUploadFile.print("0");
                fsUploadFile.close();
            }
            else
                return;
        }
        else
        {
            Serial.println("Cancel update!");
            Serial.printf("*** SYSINFO: Update bootstrap css Fehler: %s\n", https.errorToString(httpCode).c_str());
            https.end();
            LittleFS.end(); // unmount LittleFS
            ESP.restart();
            return;
        }
        https.end();
        return;
    }
    return;
}

void upJS1()
{
    std::unique_ptr<BearSSL::WiFiClientSecure> clientup(new BearSSL::WiFiClientSecure);
    clientup->setInsecure();
    HTTPClient https;
    String indexURL;
    indexURL = "https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/bootstrap.min.js";
    if (https.begin(*clientup, indexURL))
    {
        int httpCode = https.GET();
        if (httpCode > 0)
        {
            if (httpCode == HTTP_CODE_OK)
            {
                int len = https.getSize();
                static uint8_t buff[128] = {0};
                fsUploadFile = LittleFS.open("/bootstrap.min.js", "w");
                if (!fsUploadFile)
                {
                    Serial.println("Abbruch!");
                    Serial.println("*** SYSINFO: Fehler beim Speichern bootstrap js");
                    https.end();
                    return;
                }
                while (https.connected() && (len > 0 || len == -1))
                {
                    size_t size = clientup->available();
                    if (size)
                    {
                        int c = clientup->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        fsUploadFile.write(buff, c);
                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                Serial.println("*** SYSINFO: Update bootstrap js abgeschlossen");
                fsUploadFile.close();
                bool check = LittleFS.remove("/update11.txt");
                fsUploadFile = LittleFS.open("/update111.txt", "w");
                int bytesWritten = fsUploadFile.print("0");
                fsUploadFile.close();
            }
            else
                return;
        }
        else
        {
            Serial.println("Cancel update!");
            Serial.printf("*** SYSINFO: Update bootstrap js Fehler: %s\n", https.errorToString(httpCode).c_str());
            https.end();
            LittleFS.end(); // unmount LittleFS
            ESP.restart();
            return;
        }
        https.end();
        return;
    }
    return;
}

void upJS2()
{
    std::unique_ptr<BearSSL::WiFiClientSecure> clientup(new BearSSL::WiFiClientSecure);
    clientup->setInsecure();
    HTTPClient https;
    String indexURL;
    indexURL = "https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/jquery.tabletojson.min.js";
    if (https.begin(*clientup, indexURL))
    {
        int httpCode = https.GET();
        if (httpCode > 0)
        {
            if (httpCode == HTTP_CODE_OK)
            {
                int len = https.getSize();
                static uint8_t buff[128] = {0};
                fsUploadFile = LittleFS.open("/jquery.tabletojson.min.js", "w");
                if (!fsUploadFile)
                {
                    Serial.println("Abbruch!");
                    Serial.println("*** SYSINFO: Fehler beim Speichern tablejson js");
                    https.end();
                    return;
                }
                while (https.connected() && (len > 0 || len == -1))
                {
                    size_t size = clientup->available();
                    if (size)
                    {
                        int c = clientup->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        fsUploadFile.write(buff, c);
                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                Serial.println("*** SYSINFO: Update tablejson js abgeschlossen");
                fsUploadFile.close();
                bool check = LittleFS.remove("/update1111.txt");
                fsUploadFile = LittleFS.open("/update2.txt", "w");
                int bytesWritten = fsUploadFile.print("0");
                fsUploadFile.close();
            }
            else
                return;
        }
        else
        {
            Serial.println("Cancel update!");
            Serial.printf("*** SYSINFO: Update tablejson js Fehler: %s\n", https.errorToString(httpCode).c_str());
            https.end();
            LittleFS.end(); // unmount LittleFS
            ESP.restart();
            return;
        }
        https.end();
        return;
    }
    return;
}

void upJQ()
{
    std::unique_ptr<BearSSL::WiFiClientSecure> clientup(new BearSSL::WiFiClientSecure);
    clientup->setInsecure();
    HTTPClient https;
    String indexURL;
    indexURL = "https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/master/data/jquery.min.js";

    if (https.begin(*clientup, indexURL))
    {
        int httpCode = https.GET();
        if (httpCode > 0)
        {
            if (httpCode == HTTP_CODE_OK)
            {
                int len = https.getSize();
                static uint8_t buff[128] = {0};
                fsUploadFile = LittleFS.open("/jquery.min.js", "w");
                if (!fsUploadFile)
                {
                    Serial.println("Abbruch!");
                    Serial.println("*** SYSINFO: Fehler beim Speichern JQuery");
                    https.end();
                    return;
                }
                while (https.connected() && (len > 0 || len == -1))
                {
                    size_t size = clientup->available();
                    if (size)
                    {
                        int c = clientup->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        fsUploadFile.write(buff, c);
                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }

                Serial.println("*** SYSINFO: Update JQuery abgeschlossen");
                fsUploadFile.close();
                bool check = LittleFS.remove("/update111.txt");
                fsUploadFile = LittleFS.open("/update1111.txt", "w");
                int bytesWritten = fsUploadFile.print("0");
                fsUploadFile.close();
                check = LittleFS.remove("/bootstrap.bundle.min.js");
                check = LittleFS.remove("/bootstrap.bundle.min.js.map");
                check = LittleFS.remove("/bootstrap.min.css.map");
            }
            else
                return;
        }
        else
        {
            Serial.println("Cancel update!");
            Serial.printf("*** SYSINFO: pdate JQuery Fehler: %s\n", https.errorToString(httpCode).c_str());
            https.end();
            LittleFS.end(); // unmount LittleFS
            ESP.restart();
            return;
        }
        https.end();
        return;
    }
    return;
}

void upCerts()
{
    //    const uint8_t fingerprint[20] = {0xcc, 0xaa, 0x48, 0x48, 0x66, 0x46, 0x0e, 0x91, 0x53, 0x2c, 0x9c, 0x7c, 0x23, 0x2a, 0xb1, 0x74, 0x4d, 0x29, 0x9d, 0x33};
    std::unique_ptr<BearSSL::WiFiClientSecure> clientup(new BearSSL::WiFiClientSecure);
    //clientup->setFingerprint(fingerprint);
    clientup->setInsecure();
    HTTPClient https;

    if (https.begin(*clientup, "https://guest:guest:x-oauth-basic@raw.githubusercontent.com/InnuendoPi/Spundomat/master/Info/ce.rts"))
    {
        int httpCode = https.GET();
        if (httpCode > 0)
        {
            // Serial.printf("*** SYSINFO: [HTTPS] GET certs.ar Antwort: %d\n", httpCode);
            if (httpCode == HTTP_CODE_OK)
            {
                int len = https.getSize();
                static uint8_t buff[128] = {0};
                //fs::File f = LittleFS.open("/certs.ar", "w");
                fsUploadFile = LittleFS.open("/certs.ar", "w");
                if (!fsUploadFile)
                {
                    Serial.println("Abbruch!");
                    Serial.println("*** SYSINFO: Fehler beim Speichern certs.ar");
                    https.end();
                    return;
                }
                while (https.connected() && (len > 0 || len == -1))
                {
                    size_t size = clientup->available();
                    if (size)
                    {
                        int c = clientup->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        fsUploadFile.write(buff, c);
                        if (len > 0)
                        {
                            len -= c;
                        }
                    }
                    delay(1);
                }
                Serial.println("*** SYSINFO: Update Certs abgeschlossen.");
                // Close LittleFS file
                fsUploadFile.close();
                bool check = LittleFS.remove("/update2.txt");
                fsUploadFile = LittleFS.open("/update3.txt", "w");
                int bytesWritten = fsUploadFile.print("0");
                fsUploadFile.close();
            }
            else
            {
                return;
            }
        }
        else
        {
            Serial.println("Abbruch!");
            Serial.printf("*** SYSINFO: Update certs.ar Fehler: %s\n", https.errorToString(httpCode).c_str());
            https.end();
            LittleFS.end(); // unmount LittleFS
            ESP.restart();
            return;
        }
        https.end();
        return;
    }
    return;
}

void upFirm()
{
    BearSSL::CertStore certStore;
    int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    Serial.print(F("*** SYSINFO: Number of CA certs read: "));
    Serial.println(numCerts);
    if (numCerts == 0)
    {
        Serial.println(F("*** SYSINFO: No certs found. Did you run certs-from-mozill.py and upload the LittleFS directory before running?"));
        return; // Can't connect to anything w/o certs!
    }

    BearSSL::WiFiClientSecure clientFirm;
    clientFirm.setCertStore(&certStore);
    clientFirm.setInsecure();

    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    //ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    t_httpUpdate_return ret = ESPhttpUpdate.update(clientFirm, "https://raw.githubusercontent.com/InnuendoPi/Spundomat/master/build/Spundomat.ino.bin");

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("*** SYSINFO: HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("*** SYSINFO: HTTP_UPDATE_NO_UPDATES");
        break;

    case HTTP_UPDATE_OK:
        Serial.println("*** SYSINFO: HTTP_UPDATE_OK");
        break;
    }
    return;
}

void updateSys()
{
    if (LittleFS.exists("/update.txt"))
    {
        fsUploadFile = LittleFS.open("/update.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            bool check = LittleFS.remove("/update.txt");
            Serial.println("*** SYSINFO: ERROR Index Update");
            return;
        }
        fsUploadFile = LittleFS.open("/update.txt", "w");
        int bytesWritten = fsUploadFile.print((i++));
        fsUploadFile.close();
        fsUploadFile = LittleFS.open("/log.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upIndexLCD();
        Serial.print("*** SYSINFO: Update Index gestartet - free heap: ");
        Serial.println(ESP.getFreeHeap());
        upIn();
    }
    if (LittleFS.exists("/update0.txt"))
    {
        fsUploadFile = LittleFS.open("/update0.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            bool check = LittleFS.remove("/update0.txt");
            Serial.println("*** SYSINFO: ERROR Ablaufplan Update");
            return;
        }
        fsUploadFile = LittleFS.open("/update0.txt", "w");
        int bytesWritten = fsUploadFile.print((i++));
        fsUploadFile.close();
        fsUploadFile = LittleFS.open("/log0.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upAblaufLCD();
        Serial.print("*** SYSINFO: Update Ablaufplan gestartet - free heap: ");
        Serial.println(ESP.getFreeHeap());
        upAb();
    }
    if (LittleFS.exists("/update1.txt"))
    {
        fsUploadFile = LittleFS.open("/update1.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            bool check = LittleFS.remove("/update1.txt");
            Serial.println("*** SYSINFO: ERROR update bootstrap css");
            return;
        }
        fsUploadFile = LittleFS.open("/update1.txt", "w");
        i++;
        int bytesWritten = fsUploadFile.print(i);
        fsUploadFile.close();
        fsUploadFile = LittleFS.open("/log1.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upBootstrapCSSLCD();
        Serial.print("*** SYSINFO: Update bootstrap css gestartet - free heap: ");
        Serial.println(ESP.getFreeHeap());
        upCSS();
    }
    if (LittleFS.exists("/update11.txt"))
    {
        fsUploadFile = LittleFS.open("/update11.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            bool check = LittleFS.remove("/update11.txt");
            Serial.println("*** SYSINFO: ERROR update bootstrap js");
            return;
        }
        fsUploadFile = LittleFS.open("/update11.txt", "w");
        i++;
        int bytesWritten = fsUploadFile.print(i);
        fsUploadFile.close();
        fsUploadFile = LittleFS.open("/log11.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upBootstrapJSLCD();
        Serial.print("*** SYSINFO: Update bootstrap js gestartet - free heap: ");
        Serial.println(ESP.getFreeHeap());
        upJS1();
    }
    if (LittleFS.exists("/update111.txt"))
    {
        fsUploadFile = LittleFS.open("/update111.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            bool check = LittleFS.remove("/update111.txt");
            Serial.println("*** SYSINFO: ERROR update JQuery");
            return;
        }
        fsUploadFile = LittleFS.open("/update111.txt", "w");
        i++;
        int bytesWritten = fsUploadFile.print(i);
        fsUploadFile.close();
        fsUploadFile = LittleFS.open("/log111.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upJQueryLCD();
        Serial.print("*** SYSINFO: Update JQuery gestartet - free heap: ");
        Serial.println(ESP.getFreeHeap());
        upJQ();
    }
    if (LittleFS.exists("/update1111.txt"))
    {
        fsUploadFile = LittleFS.open("/update1111.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            bool check = LittleFS.remove("/update1111.txt");
            Serial.println("*** SYSINFO: ERROR update tablejson js");
            return;
        }
        fsUploadFile = LittleFS.open("/update1111.txt", "w");
        i++;
        int bytesWritten = fsUploadFile.print(i);
        fsUploadFile.close();
        fsUploadFile = LittleFS.open("/log1111.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upTableJSLCD();
        Serial.print("*** SYSINFO: Update tablejson js gestartet - free heap: ");
        Serial.println(ESP.getFreeHeap());
        upJS2();
    }
    if (LittleFS.exists("/update2.txt"))
    {
        fsUploadFile = LittleFS.open("/update2.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            bool check = LittleFS.remove("/update2.txt");
            Serial.println("*** SYSINFO: ERROR Cert Update");
            return;
        }
        fsUploadFile = LittleFS.open("/update2.txt", "w");
        int bytesWritten = fsUploadFile.print((i++));
        fsUploadFile.close();
        fsUploadFile = LittleFS.open("/log2.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upCertLCD();
        Serial.print("*** SYSINFO: Update Certs gestartet - free heap: ");
        Serial.println(ESP.getFreeHeap());
        upCerts();
    }
    if (LittleFS.exists("/update3.txt"))
    {
        fsUploadFile = LittleFS.open("/update3.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            bool check = LittleFS.remove("/update3.txt");
            return;
        }
        fsUploadFile = LittleFS.open("/update3.txt", "w");
        int bytesWritten = fsUploadFile.print((i++));
        fsUploadFile.close();
        fsUploadFile = LittleFS.open("/log3.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upFirmLCD();
        Serial.print("*** SYSINFO: Update Firmware gestartet - free heap: ");
        Serial.println(ESP.getFreeHeap());
        upFirm();
    }
}

void startHTTPUpdate()
{
    // Starte Updates
    fsUploadFile = LittleFS.open("/update.txt", "w");
    if (!fsUploadFile)
    {
        DEBUG_MSG("%s\n", "Failed to open update file for writing");
        return;
    }
    else
    {
        int bytesWritten = fsUploadFile.print("0");
        fsUploadFile.close();
    }
    updateLCD();
    server.send(200, "text/plain", "rebooting...");
    LittleFS.end(); // unmount LittleFS
    ESP.restart();
}

void checkLog()
{
    bool check = false;
    if (LittleFS.exists("/log.txt"))
    {
        fsUploadFile = LittleFS.open("/log.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        check = LittleFS.remove("/log.txt");
        Serial.printf("*** SYSINFO: Update Index Anzahl Versuche %s\n", line.c_str());
    }
    if (LittleFS.exists("/log0.txt"))
    {
        fsUploadFile = LittleFS.open("/log0.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        check = LittleFS.remove("/log0.txt");
        Serial.printf("*** SYSINFO: Update Ablaufplan Anzahl Versuche %s\n", line.c_str());
    }
    if (LittleFS.exists("/log11.txt"))
    {
        fsUploadFile = LittleFS.open("/log11.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        check = LittleFS.remove("/log11.txt");
        Serial.printf("*** SYSINFO: Update bootstrap js Anzahl Versuche %s\n", line.c_str());
    }
    if (LittleFS.exists("/log111.txt"))
    {
        fsUploadFile = LittleFS.open("/log111.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        check = LittleFS.remove("/log111.txt");
        Serial.printf("*** SYSINFO: Update JQuery Anzahl Versuche %s\n", line.c_str());
    }
    if (LittleFS.exists("/log1111.txt"))
    {
        fsUploadFile = LittleFS.open("/log1111.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        check = LittleFS.remove("/log1111.txt");
        Serial.printf("*** SYSINFO: Update tablejson js Anzahl Versuche %s\n", line.c_str());
    }
    if (LittleFS.exists("/log1.txt"))
    {
        fsUploadFile = LittleFS.open("/log1.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        check = LittleFS.remove("/log1.txt");
        Serial.printf("*** SYSINFO: Update bootstrap css Anzahl Versuche %s\n", line.c_str());
    }
    if (LittleFS.exists("/log2.txt"))
    {
        fsUploadFile = LittleFS.open("/log2.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        check = LittleFS.remove("/log2.txt");
        Serial.printf("*** SYSINFO: Update Cert Anzahl Versuche %s\n", line.c_str());
    }
    if (LittleFS.exists("/log3.txt"))
    {
        fsUploadFile = LittleFS.open("/log3.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        check = LittleFS.remove("/log3.txt");
        Serial.printf("*** SYSINFO: Update Firmware Anzahl Versuche %s\n", line.c_str());
        check = LittleFS.remove("/update3.txt");
        alertState = true;
    }
}

void update_progress(int cur, int total)
{
    Serial.printf("*** SYSINFO: Firmware Update %d von %d Bytes\n", cur, total);
}

void update_started()
{
    Serial.println("*** SYSINFO: Firmware Update gestartet");
}

void update_finished()
{
    Serial.println("*** SYSINFO: Firmware Update beendet");
    bool check = LittleFS.remove("/update3.txt");
}

void update_error(int err)
{
    Serial.printf("*** SYSINFO: Firmware Update Fehler error code %d\n", err);
    LittleFS.end(); // unmount LittleFS
    ESP.restart();
}
