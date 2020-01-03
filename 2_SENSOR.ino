/**
 * Der Spundomat misst den aktuellen Druck am T-Stück
 * und vergleicht dem Messwert mit dem auf Basis der Temperatur 
 * berechneten Spundungsdruck und lässt für eine Zeit Druck ab. 
 * Die Zeit wird nach größe der Druckdifferent berechnet. 
 * 
 * Sensor DS18B20 Temperatur
 * Sensor Druck
 **/

// Lese Temperatur
void readTemparature()
{
	sensors.requestTemperatures();
	temperature = sensors.getTempCByIndex(0);

	// Kommastelle für Ausgabe entfernen (Konvertiere float in char array)
	dtostrf(temperature, 5, 1, sTemperature);

	// Aktualisiere LCD wenn Temperatur geändert hat
	if (temperature != oldTemperature)
	{
		reflashLCD = true;
		oldTemperature = temperature;
	}
}

// Lese Druck
void readPressure()
{
	int sensorValue = analogRead(A0);

	// Skaliere Analogwert auf 5V
	voltage = (sensorValue * 5.0) / 1023.0;

	// Berechne Druck offsetVoltage = 0.42;
	pressure = (voltage - offsetVoltage) * 1.36;

	if (isnan(pressure))
		pressure = -1;

	// Aktualisiere LCD wenn Druck geändert hat
	if (fabs(pressure - oldPressure) > 0.01)
	{
		reflashLCD = true;
		oldPressure = pressure;
	}
}

// Timer Objekte
void timerTempCallback(void *pArg) // Timer Objekt Temperatur mit Pointer
{
	TickTempOccured = true; // Bei true wird im nächsten loop readTemperature ausgeführt
}

void timerPressureCallback(void *pArg) // Timer Objekt Temperatur mit Pointer
{
	TickPressureOccured = true; // Bei true wird im nächsten loop readPressure ausgeführt
}
