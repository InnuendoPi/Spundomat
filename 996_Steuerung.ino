void steuerung()
{
    // Minimale Gärsteuerung für glassart
    // MV1 Kühlung
    // MV2 Heizung
    // diff ist der absolute Abstand zw IST und Zieltemperatur
    // Erreichen der Zieltemperatur +/- 0.5 Grad - in diesem Bereich sind MV1/MV2 aus

    float diff = ((int) ((targetTemp - temperature) * 10)) / 10.0;
    if (abs(diff) < ABWEICHUNG) // Temperaturdifferenz kleiner 0,5?
        diff = 0.0;

    if (diff > 0.0)
    {
        // Heizen
        controller = 2;
        mv2.switchOn();
        if (mv1.getState())
            mv1.switchOff();

        if (!checkTemp)
        {
            TickerCon.pause();
            // DEBUG_MSG("TickerCon 2 Pause Heizung Status %d\n", TickerCon.state());
        }
        DEBUG_MSG("Steuerplan Heizung MV1 %d MV2 %d Temp %3.2f Target %3.2f\n", mv1.getState(), mv2.getState(), temperature, targetTemp );
    }
    else if (diff < 0.0)
    {
        // Kühlen
        controller = 1;
        mv1.switchOn();
        if (mv2.getState())
            mv2.switchOff();

        if (!checkTemp) // checkTemp ist solange false, bis die Differenz zur Zieltemperatur kleiner 0.5
        {
            TickerCon.pause();
            // DEBUG_MSG("TickerCon 1 Pause Kühlung Status %d\n", TickerCon.state());
        }
        DEBUG_MSG("Steuerplan Kühlung MV1 %d MV2 %d Temp %3.2f Target %3.2f\n", mv1.getState(), mv2.getState(), temperature, targetTemp);
    }
    else
    {
        // do nothing
        if (!checkTemp)
        {
            checkTemp = true; // Differenz zur Zieltemp kleiner 0,5 -> setze checkTemp true
            TickerCon.reset();
            TickerCon.resume();
            TickerCon.update();
            // DEBUG_MSG("TickerCon RESUME elapsed: %ld\n", TickerCon.elapsed());
        }
        controller = 0;
        if (mv2.getState())
            mv2.switchOff();
        if (mv1.getState())
            mv1.switchOff();
        DEBUG_MSG("Steuerplan MV1 %d MV2 %d Temp %3.2f Target %3.2f\n", mv1.getState(), mv2.getState(), temperature, targetTemp );
        // DEBUG_MSG("TickerCon Status %d\n", TickerCon.state());
    }
}

void startCon()
{
    targetTemp = structCon[counterCon].zieltemp;     // setze Zieltemperatur
    TickerCon.interval(structCon[counterCon].timer); // setze Zeitintervall aus Steuerplan
    steuerung();                                     // starte Steuerung MV1 / MV2
    DEBUG_MSG("startCon: TickerCon Update zieltemp: %f timer: %d counter: %d\n",
              targetTemp, structCon[counterCon].timer / 1000, counterCon);
}

void readSteuerplan(File &f)
{
    if (file)
    {
        int lineCounter = 0;
        int lineStruktur = 0;
        int headerCounter = 0;
        while (file.available())
        {
            char line[128];
            char c = file.read();
            char cIndex = 0;

            // Header Zeile - Namen für Steuerplan
            bool headerStruktur = false;
            char *shortName;
            char *longName;
            char startDeliHeader = '#';
            char endDelimiter = '\n';

            // Struktur Steuerplan
            char *strTemp;    // Zieltemperatur
            char *strTimer;   // Dauer Rast auf Zieltemperatur
            char *strDelta24; // Temperaturänderung innerhalb 24h (def. 0.0)
            char delimiter = ';';

            while (c != '\n' && cIndex < 128)
            {
                if (c == '#' && cIndex == 0)
                {
                    headerStruktur = true;
                    headerCounter++;
                }
                line[cIndex] = c;
                line[cIndex + 1] = '\0';
                cIndex++;
                if (!file.available())
                    break;
                c = file.read();
            }
            if (headerStruktur) // Planname
            {
                shortName = strtok(line, &startDeliHeader); // Das erste Zeichen muss ein # sein
                shortName = strtok(shortName, &delimiter);
                longName = strtok(NULL, &endDelimiter);
                // DEBUG_MSG("Line# %d shortName: %s longName: %s\n", lineCounter, shortName, longName);
                if (headerCounter == 1)
                {
                    modes[CON1] = shortName;   // ModusNamen im Display
                    modesWeb[CON1] = longName; // Modusname WebIf
                }
                else if (headerCounter == 2)
                {
                    modes[CON2] = shortName;
                    modesWeb[CON2] = longName;
                }
                lineStruktur = 0;
            }
            else if (lineStruktur < maxCon) // Planstruktur
            {
                strTemp = checkChars(strtok(line, &delimiter));
                strTimer = checkChars(strtok(NULL, &delimiter));
                strDelta24 = checkChars(strtok(NULL, &delimiter));

                if (setMode - CON1 + 1 == headerCounter) // 5-5+1 = 1 | 6-5+1 = 2 | 7-5+1 = 3
                {
                    if (isValidDigit(strTemp))
                        structCon[lineStruktur].zieltemp = atof(strTemp);
                    if (isValidInt(strTimer))
                        structCon[lineStruktur].timer = atoi(strTimer);
                    if (isValidInt(strDelta24))
                        structCon[lineStruktur].delta24 = atoi(strDelta24);

                    DEBUG_MSG("Con %d Zeile %d: Temp: %f Timer: %d Delta24: %f\n",
                              headerCounter, lineStruktur,
                              structCon[lineStruktur].zieltemp, structCon[lineStruktur].timer,
                              structCon[lineStruktur].delta24);
                }

                lineStruktur++;
            }
            headerStruktur = false;
            lineCounter++;
            if (lineCounter > (2 * maxCon + 2)) // 2 Pläne + maxCon Schritte + 2 Header Zeilen
                break;
        }
        return;
    }
    return;
}

void initSteuerplan()
{
    struct Steuerplan structCon[maxCon] = {
        {0.0, 0, 0.0},
        {0.0, 0, 0.0},
        {0.0, 0, 0.0},
        {0.0, 0, 0.0},
        {0.0, 0, 0.0}};
}
