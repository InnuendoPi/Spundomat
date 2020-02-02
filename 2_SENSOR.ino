// Lese Temperatur
void readTemparature()
{
	sensors.requestTemperatures();
	temperature = sensors.getTempCByIndex(0);

	if (testModus)
		temperature = 20.0;

	// Kommastelle für Ausgabe entfernen (Konvertiere float in char array)
	dtostrf(temperature, 5, 1, sTemperature);

	// Aktualisiere LCD wenn Temperatur geändert hat
	if (temperature != oldTemperature)
	{
		reflashLCD = true;
		oldTemperature = temperature;
	}
}

// Lese Daten vom Sensor A0
int readSensor()
{
	return analogRead(A0);
}

// Skaliere Analogwert auf 5V
float readVolt()
{
	return (readSensor() * 5.0 / 1023.0);
}

// Lese Druck
void readPressure()
{
	int sensorValue = analogRead(A0);
	// Skaliere Analogwert auf 5V
	voltage = (sensorValue * 5.0) / 1023.0;
	if (offset2 > 0 && offset0 > 0 && sensorValue > offset0)
	{
		float m = (pressureOffset2 - 0.0) / (offset2 - offset0);
		float b = pressureOffset2 - m * offset2;

		//pressure = m * sensorValue * 5.0 / 1023.0 + b;
		pressure = m * sensorValue + b;
		DEBUG_MSG("Senor: %d P: %f offset0: %d offset2: %d m: %f b: %f\n", sensorValue, pressure, offset0, offset2, m, b);
	}
	else
	{
		pressure = (sensorValue * 0.004889 - offset0 * 5.0 / 1023.0) * 1.724;
		DEBUG_MSG("Senor: %d P: %f offset0: %d \n", sensorValue, pressure, offset0);
	}

	// if (isnan(pressure) || pressure < 0)
	// 	pressure = 0.0;

	// Testmodus - Ignorieren!
	if (testModus)
	{
		oldPressDisp = pressure;
		checkTestMode();
		return;
	}

	// Aktualisiere LCD wenn Druck geändert hat
	// if (setMode != PLAN1) // AUS
	// {
	if (fabs(pressure - oldPressDisp) > DELTA) // Absolute Änderung
	{
		reflashLCD = true;
		oldPressDisp = pressure;
	}
	// Negativer Druck
	// if (pressure < 0.0)
	// 	pressure = 0.0;
}

// !!!Testcode!!! Ignorieren!
void checkTestMode()
{
	switch (setMode)
	{
	case AUS:
		break;
	case SPUNDEN_CO2: // CO2 Spunden
		if (pressure > calcPressure(setCarbonation, temperature))
		{
			pressure = oldPressDisp - 0.015;
		}
		break;
	case SPUNDEN_DRUCK: // Druck Spunden
		if (pressure > setPressure)
		{
			pressure = oldPressDisp - 0.015;
		}
		break;
	case KARBONISIEREN_CO2: // Karbonisieren
		if (pressure < calcPressure(setCarbonation, temperature))
		{
			pressure = oldPressDisp + 0.015;
		}
		break;
	case KARBONISIEREN_DRUCK: // Karbonisieren
		if (pressure < setPressure)
		{
			pressure = oldPressDisp + 0.015;
		}
		break;
	case SPUNDOMAT_UPDATE: // CO2 Spunden & Karbonisieren
		if (!stepA)
		{
			pressure = oldPressDisp - 0.015;
			DEBUG_MSG("%s", "minus\n");
		}
		if (!stepB)
		{
			pressure = oldPressDisp + 0.015;
			DEBUG_MSG("%s", "plus\n");
		}
		break;
	case PLAN1:
	case PLAN2:
	case PLAN3:
		if (!stepA)
			pressure = oldPressDisp - 0.015;
		else
			pressure = oldPressDisp + 0.015;
		break;
	}
}
