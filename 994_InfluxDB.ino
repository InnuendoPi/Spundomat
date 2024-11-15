void setInfluxDB()
{
    // Setze Parameter
    if (startDB == 1)
        dbClient.setConnectionParamsV1(dbServer, dbDatabase, dbUser, dbPass);
    else if (startDB == 2)
        dbClient.setConnectionParams(dbServer, dbDatabase, dbUser, dbPass);
    else
        return;
}

bool checkDBConnect()
{
    if (dbClient.validateConnection())
    {
        if (!connected2DB)
        {
            // DEBUG_MSG("Verbinde mit InfluxDB: %s\n", dbClient.getServerUrl().c_str());
            connected2DB = true;
        }
        visState = "0";
        return true;
    }
    else
    {
        // DEBUG_MSG("Verbindung zu InfluxDB Datenbank fehlgeschlagen: %s\n", dbClient.getLastErrorMessage().c_str());
        visState = dbClient.getLastErrorMessage().c_str();
        connected2DB = false;
        return false;
    }
}

void sendDBData()
{
    // sensor.clearFields();
    Point dbData("spundomat_status");
    dbData.addTag("Device", nameMDNS);
    if (dbVisTag[0] != '\0')
        dbData.addTag("Sud-ID", dbVisTag);
    dbData.addField("Temperatur", temperature);
    dbData.addField("Pressure", pressure);
    dbData.addField("ZielCO2", setCarbonation);
    dbData.addField("IstCO2", calcCarbonation(pressure, temperature));
    //DEBUG_MSG("Sende an InfluxDB: %s\n", dbData.toLineProtocol().c_str());
    visState = "0";
    if (!dbClient.writePoint(dbData))
    {
        // DEBUG_MSG("InfluxDB Schreibfehler1: %s\n", dbClient.getLastErrorMessage().c_str());
        // millis2wait(PAUSE100MS);
        if (!dbClient.writePoint(dbData)) // 2. Versuch in Datenbank zu schreiben
        {
            visState = dbClient.getLastErrorMessage().c_str(); // setze Schreibstatus auf Fehler
            // DEBUG_MSG("InfluxDB Schreibfehler2: %s\n", dbClient.getLastErrorMessage().c_str());
        }
    }
}
