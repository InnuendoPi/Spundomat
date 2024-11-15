void readAblaufplan(int value)
{
    File planfile;
    if (value == PLAN1)
        planfile = LittleFS.open(ABLAUFPLAN1, "r");
    else if (value == PLAN2)
        planfile = LittleFS.open(ABLAUFPLAN2, "r");
    else if (value == PLAN3)
        planfile = LittleFS.open(ABLAUFPLAN3, "r");
    else
        return;

    initAblaufplan();
    if (planfile)
    {
        JsonDocument doc;
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
