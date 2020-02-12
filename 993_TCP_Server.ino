void publishTCP()
{
    tcpClient.connect(tcpHost, tcpPort);
    if (tcpClient.connect(tcpHost, tcpPort))
    {
        StaticJsonDocument<128> doc;
        doc["name"] = nameMDNS;
        doc["name"] = "1";
        doc["temperature"] = temperature;
        doc["temp_units"] = "C";
        doc["RSSI"] = WiFi.RSSI();
        doc["interval"] = TCP_UPDATE;
        doc["battery"] = pressure;
        doc["angle"] = calcCarbonation(pressure, temperature);
        doc["gravity"] = setCarbonation;

        // Send additional but sensless data to act as an iSpindle device
        // json from iSpindle:
        // Input Str is now:{"name":"iSpindle","ID":1234567,"angle":22.21945,"temperature":15.6875,
        // "temp_units":"C","battery":4.207508,"gravity":1.019531,"interval":900,"RSSI":-59}

        char jsonMessage[128];
        serializeJson(doc, jsonMessage);
        tcpClient.write(jsonMessage);
    }
}
