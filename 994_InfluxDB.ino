void setInfluxDB()
{
    // Setze Parameter 
    client.setConnectionParamsV1(dbServer, dbDatabase, dbUser, dbPass);
    // sensor.addTag("Temperatur", "0");
    // sensor.addTag("Pressure", "0");
    // sensor.addTag("ZielCO2", "0");
    // sensor.addTag("IstCO2", "0");
}

void checkDBConnect()
{
    if (client.validateConnection())
        DEBUG_MSG("Verbinde mit InfluxDB: %s\n", client.getServerUrl().c_str());
    else
        DEBUG_MSG("Verbindung zu InfluxDB Datenbank fehlgeschlagen: ", client.getLastErrorMessage().c_str());
}

void sendDBData()
{
    // sensor.clearFields();
    Point sensor("spundomat_status");
    sensor.addTag("Device", nameMDNS);
    sensor.addField("Temperatur", temperature);
    sensor.addField("Pressure", pressure);
    sensor.addField("ZielCO2", setCarbonation);
    sensor.addField("IstCO2", calcCarbonation(pressure, temperature));
    DEBUG_MSG("Sende an InfluxDB: ", sensor.toLineProtocol().c_str());
    if (!client.writePoint(sensor))
    {
        DEBUG_MSG("InfluxDB Schreibfehler: %s\n", client.getLastErrorMessage().c_str());
    }
}