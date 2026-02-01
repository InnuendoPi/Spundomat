void handleFermenter()
{
    server.sendHeader(PSTR("Content-Encoding"), "gzip");
    server.send_P(200, "text/html", splan_htm_gz, splan_htm_gz_len);
}

void steuerung()
{
    // Minimale Gärsteuerung
    // MV1 Kühlung
    // MV2 Heizung
    // diff ist der absolute Abstand zw IST und Zieltemperatur
    // Erreichen der Zieltemperatur +/- 0.5 Grad - in diesem Bereich sind MV1/MV2 aus
    //
    // Die Gärsteuerung wird momentan nicht weiter entwickelt.
    // Zur Aktivierung muss im WebIf (index.html) Zeile 21 auskommentiert werden
    // <a href="/splan.html" class="btn btn-outline-primary">Gärsteuerung</a>
    // Die Tabellenansicht benötigt tabletojson.min.js

    float diff = int((targetTemp - temperature) * 100) / 100.0;
    if (fabs(diff) <= ABWEICHUNG) // Temperaturdifferenz kleiner 0,5?
        diff = 0.0;

    if (diff > 0.0)
    {
        // Heizen
        controller = 2;
        // mv2.switchOn();
        if (mv1.getState())
            mv1.switchOff();

        mv2.switchOn();

        if (!checkTemp)
        {
            TickerCon.pause();
            // DEBUG_MSG("TickerCon 2 Pause Heizung Status %d\n", TickerCon.state());
        }
        Serial.printf("Gärsteuerung Heizung MV1 %d MV2 %d Temp %3.2f Target %3.2f\n", mv1.getState(), mv2.getState(), temperature, targetTemp);
    }
    else if (diff < 0.0)
    {
        // Kühlen
        controller = 1;
        // mv1.switchOn();
        if (mv2.getState())
            mv2.switchOff();

        mv1.switchOn();

        if (!checkTemp) // checkTemp ist solange false, bis die Differenz zur Zieltemperatur kleiner 0.5
        {
            TickerCon.pause();
            // DEBUG_MSG("TickerCon 1 Pause Kühlung Status %d\n", TickerCon.state());
        }
        DEBUG_MSG("Gärsteuerung Kühlung MV1 %d MV2 %d Temp %3.2f Target %3.2f\n", mv1.getState(), mv2.getState(), temperature, targetTemp);
    }
    else
    {
        // Starte Time für Rastdauer
        if (!checkTemp)
        {
            checkTemp = true; // Differenz zur Zieltemp kleiner 0,5 -> setze checkTemp true
            TickerCon.reset();
            TickerCon.resume();
            TickerCon.update();
            lastTimeSteuerung = millis(); // Zeitstempel
            DEBUG_MSG("TickerCon RESET RESUME UPDATE elapsed: %ld\n", TickerCon.elapsed());
        }
        controller = 0;
        if (mv2.getState())
            mv2.switchOff();
        if (mv1.getState())
            mv1.switchOff();
        DEBUG_MSG("Gärsteuerung MV1 %d MV2 %d Temp %3.2f Target %3.2f\n", mv1.getState(), mv2.getState(), temperature, targetTemp);
        // DEBUG_MSG("TickerCon Status %d\n", TickerCon.state());
    }
}

void startCon()
{
    if (setMode == CON1)
    {
        targetTemp = structOG[counterCon].Stemp;               // setze Zieltemperatur
        TickerCon.interval(structOG[counterCon].Dauer * 60 * 1000); // setze Zeitintervall aus Steuerplan
        // DEBUG_MSG("startCon: TickerCon Update zieltemp: %f timer: %d counter: %d\n",
        //           targetTemp, structOG[counterCon].Dauer / 1000, counterCon);
    }
    else if (setMode == CON2)
    {
        targetTemp = structUG[counterCon].Stemp;               // setze Zieltemperatur
        TickerCon.interval(structUG[counterCon].Dauer * 60 * 1000); // setze Zeitintervall aus Steuerplan
    }
    else
    {
        targetTemp = DEF_TARGET_TEMP;
        TickerCon.interval(CON_UPDATE);
    }
    steuerung(); // starte Steuerung MV1 / MV2
}

void handleSetFerm()
{
    uint8_t id = server.arg(0).toInt();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, server.arg(1));
    if (error)
    {
        DEBUG_MSG("error deserializeJson setOG: %s", error.c_str());
        server.send(500, FPSTR("text/plain"), "Server error\r\n");
        return;
    }
    if (id == 1)
        planfile = LittleFS.open(OGPLAN, "w");
    else if (id == 2)
        planfile = LittleFS.open(UGPLAN, "w");

    if (planfile)
    {
        serializeJson(doc, planfile);
        planfile.close();
        server.send(200, "text/plain", "ok");
    }
    else
        server.send(500, "text/plain", "Server error");
}

void readSteuerOG()
{
    Serial.println("readSteuerOG");
    planfile = LittleFS.open(OGPLAN, "r");
    if (planfile)
    {
        Serial.println("readSteuerOG planfile");
        JsonDocument docIn;
        JsonDocument docOut;
        DeserializationError error = deserializeJson(docIn, planfile);
        if (error)
        {
            planfile.close();
#ifdef ESP32
            Serial.printf("error could not open config.txt - JSON error %s\n", error.c_str());
#endif
            return;
        }
        JsonArray steuerArray = docIn.as<JsonArray>();
        int anzahlRampen = steuerArray.size();
        Serial.printf("Read OG Anzahl Rampen: %d\n", anzahlRampen);
        if (anzahlRampen > MAXFERM)
            anzahlRampen = MAXFERM;

        int i = 0;
        for (JsonObject steuerObj : steuerArray)
        {
            if (i < anzahlRampen)
            {
                // float tmpTemp = steuerObj["Temperatur"];
                // unsigned long tmpDauer = steuerObj["Dauer"];
                structOG[i].Name = steuerObj["Fermentationsschritt"].as<String>();
                structUG[i].Dauer = steuerObj["Dauer"].as<int>();
                structUG[i].Stemp = steuerObj["Start-Temperatur"].as<float>();
                structUG[i].Etemp = steuerObj["End-Temperatur"].as<float>();
                structUG[i].Ramp = steuerObj["linear"].as<bool>();

                JsonObject outObj = docOut.createNestedObject();
                outObj["Name"] = structOG[i].Name;
                outObj["Stemp"] = structOG[i].Stemp;
                outObj["Etemp"] = structOG[i].Etemp;
                outObj["Ramp"] = structOG[i].Ramp;
                outObj["Dauer"] = structOG[i].Dauer;
                Serial.printf("readSteuerOG Schritt #%d Name: %s Starttemp: %.01f Ramp :%d Endtemp: %.01f Dauer: %lu\n", i, structOG[i].Name.c_str(), structOG[i].Stemp, structOG[i].Ramp, structOG[i].Etemp, structOG[i].Dauer);
                i++;
            }
        }
        planfile.close();
        // char response[measureJson(docOut) + 1];
        // serializeJson(docOut, response);
        String ogResponse = "";
        serializeJson(docOut, ogResponse);
        server.send_P(200, "application/json", ogResponse.c_str());
        Serial.printf("ogRes: %s\n", ogResponse.c_str());
        // server.send_P(200, "application/json", ogResponse.c_str());
        return;
    } // read planfile
    server.send(200);
    return;
}

void readSteuerUG()
{
    planfile = LittleFS.open(UGPLAN, "r");
    if (planfile)
    {
        JsonDocument docIn;
        JsonDocument docOut;
        DeserializationError error = deserializeJson(docIn, planfile);
        if (error)
        {
            planfile.close();
#ifdef ESP32
            Serial.printf("error could not open config.txt - JSON error %s\n", error.c_str());
#endif
            return;
        }
        JsonArray steuerArray = docIn.as<JsonArray>();
        int anzahlRampen = steuerArray.size();
        if (anzahlRampen > MAXFERM)
            anzahlRampen = MAXFERM;
        Serial.printf("Read UG Anzahl Rampen: %d\n", anzahlRampen);
        int i = 0;
        for (JsonObject steuerObj : steuerArray)
        {
            if (i < anzahlRampen)
            {
                // float tmpTemp = steuerObj["Temperatur"];
                // unsigned long tmpDauer = steuerObj["Dauer"];
                structUG[i].Name = steuerObj["Fermentationsschritt"].as<String>();
                structUG[i].Dauer = steuerObj["Dauer"].as<int>();
                structUG[i].Stemp = steuerObj["Start-Temperatur"].as<float>();
                structUG[i].Etemp = steuerObj["End-Temperatur"].as<float>();
                structUG[i].Ramp = steuerObj["linear"].as<bool>();
                

                JsonObject outObj = docOut.createNestedObject();
                outObj["Name"] = structUG[i].Name;
                outObj["Dauer"] = structUG[i].Dauer;
                outObj["Stemp"] = structUG[i].Stemp;
                outObj["Etemp"] = structUG[i].Etemp;
                outObj["Ramp"] = structUG[i].Ramp;
                
                Serial.printf("readSteuerUG Schritt#%d Name: %s Starttemp: %.01f Ramp :%d Endtemp: %.01f Dauer: %lu\n", i, structUG[i].Name.c_str(), structUG[i].Stemp, structUG[i].Ramp, structUG[i].Etemp, structUG[i].Dauer);
                i++;
            }
        }
        String ugResponse = "";
        serializeJson(docOut, ugResponse);
        planfile.close();
        // server.send_P(200, "application/json", ugResponse.c_str());
        // char response[measureJson(docOut) + 1];
        // serializeJson(docOut, response);
        server.send_P(200, "application/json", ugResponse.c_str());
        return;
    } // read planfile
    server.send(200);
    return;
}

void initSteuerplan()
{
    struct Steuerplan structOG[MAXFERM] = {
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0}};

    struct Steuerplan structUG[MAXFERM] = {
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0},
        {"", 0.0, 0.0, 0, 0}};
}
