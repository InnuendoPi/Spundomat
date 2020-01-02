// Öffne Ventil
// Das kleinste Schaltintervall ist 10ms. Kleiner macht das Magnetventil nicht mit.
//
void releasePressure() 
{
	digitalWrite(PIN_VALVE, HIGH);
	//delay(200);
	millis2wait(200);
	
	digitalWrite(PIN_VALVE, LOW);
	//delay(100);
	millis2wait(100);
}

void buildPressure() // Zwangskarbonisierung
{
	digitalWrite(PIN_VALVE, HIGH);
	//delay(200);
	millis2wait(200);
	
	digitalWrite(PIN_VALVE, LOW);
	//delay(100);
	millis2wait(100);
}

void openValve()
{
	if (!oldValve)
	{
		digitalWrite(PIN_VALVE, HIGH);
		millis2wait(200);
	}
	oldValve = true;
}

void closeValve()
{
	if (oldValve)
	{
		digitalWrite(PIN_VALVE, LOW);
		millis2wait(100);
	}
	oldValve = false;
}
