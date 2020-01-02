void loop()
{
    // Webserver (80)
    server.handleClient();
    // Telnet
    if (startTEL)
        checkTELNET();
    if (startMDNS)
        mdns.update();

    button.tick();
    readEncoder();

    if (TickPressureOccured)
    {
        readPressure();
        TickPressureOccured = false;
    }

    if (TickTempOccured)
    {
        readTemparature();
        TickTempOccured = false;
    }

    if (reflashLCD)
    {
        showLCD();
    }

    //Betriebsmodi
    switch (setMode)
    {
    case 0: // aus
        break;
    case 1: // CO2 Spunden
        if (pressure > calcPressure(setCarbonation, temperature))
        {
            releasePressure();
            readPressure();
        }
        break;
    case 2: // Druck Spunden
        if (pressure > setPressure)
        {
            releasePressure();
            readPressure();
        }
        break;
    case 3: // CO2 Zwangskarbonisierung
        if (pressure < calcPressure(setCarbonation, temperature))
        {
            //buildPressure();
            openValve();
            readPressure();
        }
        else
        {
            closeValve();
            readPressure();
        }
        break;
    }

    up = false;
    down = false;
}
