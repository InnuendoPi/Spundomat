// Öffne Ventil
// Das kleinste Schaltintervall ist 10ms. Kleiner macht das Magnetventil nicht mit.


void releasePressure()
{
	if (statusMV1)
	{
		digitalWrite(PIN_MV1, HIGH);
		DEBUG_MSG("releasePressure Status HIGH: %d Intervall: %d\n", statusMV1, mv1Open);
	}
	else
	{
		digitalWrite(PIN_MV1, LOW);
		DEBUG_MSG("releasePressure Status LOW: %d Intervall %d\n", statusMV1, mv1Close);
	}
}

void buildPressure() // Karbonisieren
{
	if (statusMV2)
	{
		digitalWrite(PIN_MV2, HIGH);
		DEBUG_MSG("buildPressure Status HIGH: %d Intervall: %d\n", statusMV2, mv2Open);
	}
	else
	{
		digitalWrite(PIN_MV2, LOW);
		DEBUG_MSG("buildPressure Status LOW: %d Intervall: %d\n", statusMV2, mv2Close);
	}
}
