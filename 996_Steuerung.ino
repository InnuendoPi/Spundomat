/*

Deaktiviert

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
        DEBUG_MSG("Gärsteuerung Heizung MV1 %d MV2 %d Temp %3.2f Target %3.2f\n", mv1.getState(), mv2.getState(), temperature, targetTemp);
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
        targetTemp = structOG[counterCon].Temperatur;     // setze Zieltemperatur
        TickerCon.interval(structOG[counterCon].Dauer *60 *1000); // setze Zeitintervall aus Steuerplan
        // DEBUG_MSG("startCon: TickerCon Update zieltemp: %f timer: %d counter: %d\n",
        //           targetTemp, structOG[counterCon].Dauer / 1000, counterCon);
    }
    else if (setMode == CON2)
    {
        targetTemp = structUG[counterCon].Temperatur;     // setze Zieltemperatur
        TickerCon.interval(structUG[counterCon].Dauer *60 *1000); // setze Zeitintervall aus Steuerplan
    }
    else
    {
        targetTemp = DEF_TARGET_TEMP;
        TickerCon.interval(CON_UPDATE);
    }
    steuerung(); // starte Steuerung MV1 / MV2
}

void readSteuerOG()
{
    file = LittleFS.open("/steuerplanOG.txt", "r");
    if (file)
    {
        DynamicJsonDocument docIn(1024);
        DynamicJsonDocument docOut(1024);
        DeserializationError error = deserializeJson(docIn, file);
        JsonArray steuerArray = docIn.as<JsonArray>();
        int anzahlRampen = steuerArray.size();
        if (anzahlRampen > maxCon)
            anzahlRampen = maxCon;

        DEBUG_MSG("Read OG Anzahl Rampen: %d\n", anzahlRampen);
        int i = 0;
        for (JsonObject steuerObj : steuerArray)
        {
            if (i < anzahlRampen)
            {
                // float tmpTemp = steuerObj["Temperatur"];
                // unsigned long tmpDauer = steuerObj["Dauer"];
                structOG[i].Temperatur = steuerObj["Temperatur"];
                structOG[i].Dauer = steuerObj["Dauer"];

                JsonObject outObj = docOut.createNestedObject();
                outObj["Temperatur"] = structOG[i].Temperatur;
                outObj["Dauer"] = structOG[i].Dauer;
                DEBUG_MSG("readSteuerOG Rampe #%d Temp: %f Dauer: %lu\n", i, structOG[i].Temperatur, structOG[i].Dauer);
                i++;
            }
        }
        ogResponse = "";
        serializeJson(docOut, ogResponse);
        file.close();
        return;
    } // read file
    return;
}

void readSteuerUG()
{
    file = LittleFS.open("/steuerplanUG.txt", "r");
    if (file)
    {
        DynamicJsonDocument docIn(1024);
        DynamicJsonDocument docOut(1024);
        DeserializationError error = deserializeJson(docIn, file);
        JsonArray steuerArray = docIn.as<JsonArray>();
        int anzahlRampen = steuerArray.size();
        if (anzahlRampen > maxCon)
            anzahlRampen = maxCon;
        DEBUG_MSG("Read UG Anzahl Rampen: %d\n", anzahlRampen);
        int i = 0;
        for (JsonObject steuerObj : steuerArray)
        {
            if (i < anzahlRampen)
            {
                // float tmpTemp = steuerObj["Temperatur"];
                // unsigned long tmpDauer = steuerObj["Dauer"];
                structUG[i].Temperatur = steuerObj["Temperatur"];
                structUG[i].Dauer = steuerObj["Dauer"];

                JsonObject outObj = docOut.createNestedObject();
                outObj["Temperatur"] = structUG[i].Temperatur;
                outObj["Dauer"] = structUG[i].Dauer;
                DEBUG_MSG("readSteuerUG Rampe#%d Temp: %f Dauer: %lu\n", i, structUG[i].Temperatur, structUG[i].Dauer);
                i++;
            }
        }
        ugResponse = "";
        serializeJson(docOut, ugResponse);
        file.close();
        return;
    } // read file
    return;
}

void initSteuerplan()
{
    struct Steuerplan structOG[maxCon] = {
        {0.0, 0},
        {0.0, 0},
        {0.0, 0},
        {0.0, 0},
        {0.0, 0}};

    struct Steuerplan structUG[maxCon] = {
        {0.0, 0},
        {0.0, 0},
        {0.0, 0},
        {0.0, 0},
        {0.0, 0}};
}
*/