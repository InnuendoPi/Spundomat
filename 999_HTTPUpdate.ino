void upIn()
{
    //    const uint8_t fingerprint[20] = {0xcc, 0xaa, 0x48, 0x48, 0x66, 0x46, 0x0e, 0x91, 0x53, 0x2c, 0x9c, 0x7c, 0x23, 0x2a, 0xb1, 0x74, 0x4d, 0x29, 0x9d, 0x33};
    std::unique_ptr<BearSSL::WiFiClientSecure> clientup(new BearSSL::WiFiClientSecure);
    //clientup->setFingerprint(fingerprint);
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
                fsUploadFile = SPIFFS.open("/index.html", "w");
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

                Serial.println("*** SYSINFO: Index Update abgeschlossen.");
                // Close SPIFFS file
                fsUploadFile.close();
                SPIFFS.remove("/update.txt");
                fsUploadFile = SPIFFS.open("/update2.txt", "w");
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
            SPIFFS.end(); // unmount SPIFFS
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
                //fs::File f = SPIFFS.open("/certs.ar", "w");
                fsUploadFile = SPIFFS.open("/certs.ar", "w");
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
                Serial.println("*** SYSINFO: Certs Update abgeschlossen.");
                // Close SPIFFS file
                fsUploadFile.close();
                SPIFFS.remove("/update2.txt");
                fsUploadFile = SPIFFS.open("/update3.txt", "w");
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
            SPIFFS.end(); // unmount SPIFFS
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
    int numCerts = certStore.initCertStore(SPIFFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    Serial.print(F("*** SYSINFO: Number of CA certs read: "));
    Serial.println(numCerts);
    if (numCerts == 0)
    {
        Serial.println(F("*** SYSINFO: No certs found. Did you run certs-from-mozill.py and upload the SPIFFS directory before running?"));
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
    if (SPIFFS.exists("/update.txt"))
    {
        fsUploadFile = SPIFFS.open("/update.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            SPIFFS.remove("/update.txt");
            Serial.println("*** SYSINFO: ERROR Index Update");
            return;
        }
        fsUploadFile = SPIFFS.open("/update.txt", "w");
        int bytesWritten = fsUploadFile.print((i++));
        fsUploadFile.close();
        fsUploadFile = SPIFFS.open("/log1.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upSSLLCD();
        Serial.print("*** SYSINFO: Start Index update Free Heap: ");
        Serial.println(ESP.getFreeHeap());
        upIn();
    }
    if (SPIFFS.exists("/update2.txt"))
    {
        fsUploadFile = SPIFFS.open("/update2.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            SPIFFS.remove("/update2.txt");
            Serial.println("*** SYSINFO: ERROR Cert Update");
            return;
        }
        fsUploadFile = SPIFFS.open("/update2.txt", "w");
        int bytesWritten = fsUploadFile.print((i++));
        fsUploadFile.close();
        fsUploadFile = SPIFFS.open("/log2.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upIndexLCD();
        Serial.print("*** SYSINFO: Start Cert update Free Heap: ");
        Serial.println(ESP.getFreeHeap());
        upCerts();
    }
    if (SPIFFS.exists("/update3.txt"))
    {
        fsUploadFile = SPIFFS.open("/update3.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        int i = line.toInt();
        if (i > 3)
        {
            SPIFFS.remove("/update3.txt");
            return;
        }
        fsUploadFile = SPIFFS.open("/update3.txt", "w");
        int bytesWritten = fsUploadFile.print((i++));
        fsUploadFile.close();
        fsUploadFile = SPIFFS.open("/log3.txt", "w");
        bytesWritten = fsUploadFile.print((i));
        fsUploadFile.close();
        upFirmLCD();
        Serial.print("*** SYSINFO: Start Firmware update Free Heap: ");
        Serial.println(ESP.getFreeHeap());
        upFirm();
    }
}

void startHTTPUpdate()
{
    // Starte Updates
    fsUploadFile = SPIFFS.open("/update.txt", "w");
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
    SPIFFS.end(); // unmount SPIFFS
    ESP.restart();
}

void checkLog()
{
    if (SPIFFS.exists("/log1.txt"))
    {
        fsUploadFile = SPIFFS.open("/log1.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        Serial.printf("*** SYSINFO: Update Index Anzahl Versuche %s\n", line.c_str());
        SPIFFS.remove("/log1.txt");
    }
    if (SPIFFS.exists("/log2.txt"))
    {
        fsUploadFile = SPIFFS.open("/log2.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        Serial.printf("*** SYSINFO: Update Cert Anzahl Versuche %s\n", line.c_str());
        SPIFFS.remove("/log2.txt");
    }
    if (SPIFFS.exists("/log3.txt"))
    {
        fsUploadFile = SPIFFS.open("/log3.txt", "r");
        String line;
        while (fsUploadFile.available())
        {
            line = char(fsUploadFile.read());
        }
        fsUploadFile.close();
        Serial.printf("*** SYSINFO: Update Firmware Anzahl Versuche %s\n", line.c_str());
        SPIFFS.remove("/log3.txt");
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
    SPIFFS.remove("/update3.txt");
}

void update_error(int err)
{
    Serial.printf("*** SYSINFO: Firmware Update Fehler error code %d\n", err);
    SPIFFS.end(); // unmount SPIFFS
    ESP.restart();
}
