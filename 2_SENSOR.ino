/**
 * Der Spundomat misst den aktuellen Druck am T-Stück
 * und vergleicht den Messwert mit dem auf Basis der Temperatur 
 * berechneten Spundungsdruck und lässt für eine Zeit Druck ab. 
 * 
 * Sensor DS18B20 Temperatur
 * Sensor Druck
 **/

// Lese Temperatur
void readTemparature()
{
	sensors.requestTemperatures();
	temperature = sensors.getTempCByIndex(0);

	// !!!Testcode!!! Ignorieren!
	// if (testModus)
	// temperature = 20;

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

	// !!!Testcode!!! Ignorieren!
	// if (testModus)
	// 	sensorvalue = checkTestMode();

	// Skaliere Analogwert auf 5V
	voltage = (sensorValue * 5.0) / 1023.0;

	// Berechne Druck offsetVoltage = 0.42;
	pressure = (voltage - offsetVoltage) * 1.36;

	// if (isnan(pressure))
	// 	pressure = -1;

	// Aktualisiere LCD wenn Druck geändert hat
	if (fabs(pressure - oldPressure) > 0.01)
	{
		reflashLCD = true;
		oldPressure = pressure;
	}
}

// !!!Testcode!!! Ignorieren!
// int checkTestMode()
// {
// 	switch (setMode)
// 	{
// 	case 0: // aus
// 		break;
// 	case 1: // CO2 Spunden
// 		if (pressure > calcPressure(setCarbonation, temperature))
// 		{
// 			sensorValueTest--;
// 		}
// 		break;
// 	case 2: // Druck Spunden
// 		if (pressure > setPressure)
// 		{
// 			sensorValueTest--;
// 		}
// 		break;
// 	case 3: // Karbonisieren
// 		if (pressure < calcPressure(setCarbonation, temperature))
// 		{
// 			sensorValueTest++;
// 		}
// 		break;
// 	}
// 	return sensorValueTest;
// // sensorValue = 500;
// }

// int setSensorValueTest(int value)
// {
// 	if (value)
// 	{
// 		switch (setMode)
// 		{
// 		case 0: // aus
// 			return 0;
// 			break;
// 		case 1: // CO2 Spunden
// 		case 2: // Druck Spunden
// 			return 350;
// 			break;
// 		case 3: // Karbonisieren
// 			return 100;
// 			break;
// 		}
// 	}
// }