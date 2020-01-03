// Öffne Ventil
// Das kleinste Schaltintervall ist 10ms. Kleiner macht das Magnetventil nicht mit.
//
void releasePressure()
{
	digitalWrite(PIN_MV1, HIGH);
	millis2wait(PAUSE200MS);

	digitalWrite(PIN_MV1, LOW);
	millis2wait(PAUSE100MS);
}

void buildPressure() // Karbonisieren
{
	digitalWrite(PIN_MV2, HIGH);
	millis2wait(PAUSE200MS);

	digitalWrite(PIN_MV2, LOW);
	millis2wait(PAUSE200MS);
}
