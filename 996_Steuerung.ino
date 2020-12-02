void steuerung()
{
	// Minimale Gärsteuerung für glassart
    // MV1 Kühlung
	// MV2 Heizung
    // diff ist der absolute Abstand zw IST und Zieltemperatur
    // Erreichen der Zieltemperatur +/- 0.5 Grad - in diesem Bereich sind MV1/MV2 aus (low)

	int diff = targetTemp - temperature;
    
    if (abs(diff) < ABWEICHUNG) // Temperaturdifferenz kleiner 0,5?
        diff = 0;

    if (diff > 0)
	{
        // Heizen
        controller = 2;
        digitalWrite(PIN_MV1, LOW);
        digitalWrite(PIN_MV2, HIGH);
	}
	else if (diff < 0)
	{
		// Kühlen
        controller = 1;
        digitalWrite(PIN_MV1, HIGH);
        digitalWrite(PIN_MV2, LOW);
	}
	else
	{
		// do nothing
        controller = 0;
        digitalWrite(PIN_MV1, LOW);
        digitalWrite(PIN_MV2, LOW);
	}
}
