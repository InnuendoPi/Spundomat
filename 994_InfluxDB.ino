void setInfluxDB()
{
    // Setze Parameter 
    client.setConnectionParamsV1(dbServer, dbDatabase, dbUser, dbPass);
    sensor.addTag("Temperatur", "0");
    sensor.addTag("Pressure", "0");
    sensor.addTag("ZielCO2", "0");
    sensor.addTag("IstCO2", "0");
}

void checkDBConnect()
{
    if (client.validateConnection())
    {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    }
    else
    {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
    }
}

void sendDBData()
{
    sensor.clearFields();
    sensor.addField("Temperatur", temperature);
    sensor.addField("Pressure", pressure);
    sensor.addField("ZielCO2", setCarbonation);
    sensor.addField("IstCO2", calcCarbonation(pressure, temperature));
    // Serial.println(sensor.toLineProtocol());
    if (!client.writePoint(sensor))
    {
        DEBUG_MSG("InfluxDB Schreibfehler: %s\n", client.getLastErrorMessage().c_str());
    }
}