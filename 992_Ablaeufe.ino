void readLine(File &f)
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

            // Header Zeile - Namen für Ablaufplan
            bool headerStruktur = false;
            char *shortName;
            char *longName;
            char startDeliHeader = '#';
            char endDelimiter = '\n';

            // Struktur Ablaufplan
            char *strMV1Druck;
            char *strMV1Open;
            char *strMV1Close;
            char *strMV2Druck;
            char *strMV2Open;
            char *strMV2Close;
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
                    modes[PLAN1] = shortName;   // ModusNamen im Display
                    modesWeb[PLAN1] = longName; // Modusname WebIf
                }
                else if (headerCounter == 2)
                {
                    modes[PLAN2] = shortName;
                    modesWeb[PLAN2] = longName;
                }
                else if (headerCounter == 3)
                {
                    modes[PLAN3] = shortName;
                    modesWeb[PLAN3] = longName;
                }
                lineStruktur = 0;
            }
            else if (lineStruktur < maxSchritte) // Planstruktur
            {
                strMV1Druck = checkChars(strtok(line, &delimiter));
                strMV1Open = checkChars(strtok(NULL, &delimiter));
                strMV1Close = checkChars(strtok(NULL, &delimiter));
                strMV2Druck = checkChars(strtok(NULL, &delimiter));
                strMV2Open = checkChars(strtok(NULL, &delimiter));
                strMV2Close = checkChars(strtok(NULL, &endDelimiter));

                if (setMode - PLAN1 + 1 == headerCounter) // 4-4+1 = 1 | 5-4+1 = 2 | 6-4+1 = 3  
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

void initPlan()
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
