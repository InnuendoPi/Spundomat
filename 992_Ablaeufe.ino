void readAblaufplan(int value)
{
    File planfile;
    if (value == PLAN1)
        planfile = LittleFS.open("/ablaufplan1.json", "r");
    else if (value == PLAN2)
        planfile = LittleFS.open("/ablaufplan2.json", "r");
    else if (value == PLAN3)
        planfile = LittleFS.open("/ablaufplan3.json", "r");
    else
        return;

    initAblaufplan();
    if (planfile)
    {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, planfile);
        JsonArray ablaufArray = doc.as<JsonArray>();
        int anzahlSchritte = ablaufArray.size();
        if (anzahlSchritte > maxSchritte)
            anzahlSchritte = maxSchritte;
        int i = 0;
        for (JsonObject ablaufObj : ablaufArray)
        {
            if (i < anzahlSchritte)
            {
                structPlan[i].zieldruckMV1 = ablaufObj["MV1 Druck"].as<float>();
                structPlan[i].intervallMV1Open = ablaufObj["MV1 Dauer"].as<unsigned long>();
                structPlan[i].intervallMV1Close = ablaufObj["MV1 Pause"].as<unsigned long>();
                structPlan[i].zieldruckMV2 = ablaufObj["MV2 Druck"].as<float>();
                structPlan[i].intervallMV2Open = ablaufObj["MV2 Dauer"].as<unsigned long>();
                structPlan[i].intervallMV2Close = ablaufObj["MV2 Pause"].as<unsigned long>();
                // DEBUG_MSG("readablaufPlan1 Schritt #%d DruckMV1: %f MV1Open: %lu MV1Close: %lu\n", i, structPlan1[i].zieldruckMV1, structPlan1[i].intervallMV1Open, structPlan1[i].intervallMV1Close);
                // DEBUG_MSG("readablaufPlan1 Schritt #%d DruckMV2: %f MV2Open: %lu MV2Close: %lu\n", i, structPlan1[i].zieldruckMV2, structPlan1[i].intervallMV2Open, structPlan1[i].intervallMV2Close);
                i++;
            }
        }
        counterPlan = -1;
        stepA = false;
        stepB = false;
        planfile.close();
    } // read file
}

/*
void readAblaufplan2(File &f)
{
    // Umstellung auf JSON!
    // Je Ablauf ein JSON File
    if (file)
    {
        int lineCounter = 0;
        int lineStruktur = 0;
        int headerCounter = 0;
        while (file.available())
        {
            char line[128];
            char c = file.read();
            int cIndex = 0;

            // Header Zeile - Namen für Ablaufplan
            bool headerStruktur = false;
            char *lcdName;
            char *webName;

            // Struktur Ablaufplan
            char *strMV1Druck;
            char *strMV1Open;
            char *strMV1Close;
            char *strMV2Druck;
            char *strMV2Open;
            char *strMV2Close;
            char delimiter[] = "#;";

            while (c != '\n' && cIndex < 128) // Newline oder Array End
            {
                if (c == '#' && cIndex == 0)
                {
                    headerStruktur = true;
                    headerCounter++;
                }
                line[cIndex] = c;
                cIndex++;
                line[cIndex] = '\0';
                if (!file.available())
                    break;
                c = file.read();
            }
            // DEBUG_MSG("Ablauf Line POST Index: %d Line: %s\n", cIndex, line);
            if (headerStruktur) // Planname
            {
                lcdName = strtok(line, delimiter); // Das erste Zeichen muss ein # sein
                webName = strtok(NULL, delimiter);
                if (headerCounter == 1)
                {
                    modes[PLAN1] = lcdName;    // ModusNamen im Display
                    modesWeb[PLAN1] = webName; // Modusname WebIf
                }
                else if (headerCounter == 2)
                {
                    modes[PLAN2] = lcdName;
                    modesWeb[PLAN2] = webName;
                }
                else if (headerCounter == 3)
                {
                    modes[PLAN3] = lcdName;
                    modesWeb[PLAN3] = webName;
                }
                lineStruktur = 0;
            }
            else if (lineStruktur < maxSchritte) // Planstruktur
            {
                strMV1Druck = checkChars(strtok(line, delimiter));
                strMV1Open = checkChars(strtok(NULL, delimiter));
                strMV1Close = checkChars(strtok(NULL, delimiter));
                strMV2Druck = checkChars(strtok(NULL, delimiter));
                strMV2Open = checkChars(strtok(NULL, delimiter));
                strMV2Close = checkChars(strtok(NULL, delimiter));

                if (setMode - PLAN1 + 1 == headerCounter) // 5-5+1 = 1 | 6-5+1 = 2 | 7-5+1 = 3
                {
                    if (isValidDigit(strMV1Druck))
                        structPlan[lineStruktur].zieldruckMV1 = atof(strMV1Druck);
                    if (isValidInt(strMV1Open))
                        structPlan[lineStruktur].intervallMV1Open = atoi(strMV1Open);
                    if (isValidInt(strMV1Close))
                        structPlan[lineStruktur].intervallMV1Close = atoi(strMV1Close);
                    if (isValidDigit(strMV2Druck))
                        structPlan[lineStruktur].zieldruckMV2 = atof(strMV2Druck);
                    if (isValidInt(strMV2Open))
                        structPlan[lineStruktur].intervallMV2Open = atoi(strMV2Open);
                    if (isValidInt(strMV2Close))
                        structPlan[lineStruktur].intervallMV2Close = atoi(strMV2Close);
                    DEBUG_MSG("Plan %d Zeile %d: Druck MV1: %f Open: %d Close: %d Druck MV2: %f Open: %d Close: %d\n",
                              headerCounter, lineStruktur,
                              structPlan[lineStruktur].zieldruckMV1, structPlan[lineStruktur].intervallMV1Open, structPlan[lineStruktur].intervallMV1Close,
                              structPlan[lineStruktur].zieldruckMV2, structPlan[lineStruktur].intervallMV2Open, structPlan[lineStruktur].intervallMV2Close);
                  }
                // DEBUG_MSG("Line %d: strMV1Druck: %s strMV1Open: %s strMV1Close: %s strMV2Druck: %s strMV2Open: %s strMV2Close: %s\n", lineStruktur, strMV1Druck, strMV1Open, strMV1Close, strMV2Druck, strMV2Open, strMV2Close);
                lineStruktur++;
            }
            headerStruktur = false;
            lineCounter++;
            if (lineCounter > (3 * maxSchritte + 3)) // 3 Ablaufpläne + 3 Header Zeilen
                break;
        }
        return;
    }
    return;
}
*/

void initAblaufplan()
{
    struct Ablaufplan structPlan[maxSchritte] = {
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0},
        {0.0, 0, 0, 0.0, 0, 0}};
}
