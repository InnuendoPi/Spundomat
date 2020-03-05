// Lese Temperatur
void readTemparature()
{
	sensors.requestTemperatures();
	temperature = sensors.getTempCByIndex(0);
	if (temperature == 85.0)
	{
		millis2wait(750);
		sensors.requestTemperatures();
		temperature = sensors.getTempCByIndex(0);
	}
	if (testModus)
		temperature = 20.0;

	// Kommastelle für Ausgabe entfernen (Konvertiere float in char array)
	//dtostrf(temperature, 5, 1, sTemperature);

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
	if (!testModus)
	{
		int sensorValue = analogRead(A0);
		// Skaliere Analogwert auf 5V
		voltage = (sensorValue * 5.0) / 1023.0;

		if (offset0 == 0.0 && offset2 == 0.0) // keine Kalibrierung
		{
			pressure = 0.0;
			oldPressDisp = pressure;
			reflashLCD = false;
			DEBUG_MSG("Keine Kalibrierung Sensor: %d P: %f\n", sensorValue, pressure);
			return;
		}
		//else if (offset2 > 0 && offset0 > 0 && sensorValue > offset0) // 2-Punkte-Kalibrierung
		else if (offset2 > 0 && offset0 > 0) // 2-Punkte-Kalibrierung
		{
			float m = (PRESSURE_OFFSET2 - PRESSURE_OFFSET0) / (offset2 - offset0);
			float b = PRESSURE_OFFSET2 - m * offset2;

			//pressure = m * sensorValue * 5.0 / 1023.0 + b;
			pressure = m * sensorValue + b;
			// DEBUG_MSG("2-Punkte Kalibrierung Sensor: %d P: %f offset0: %d offset2: %d m: %f b: %f\n", sensorValue, pressure, offset0, offset2, m, b);
		}
		else if (offset0 != 0.0 && offset2 == 0.0) // 1-Punkt-Kalibrierung
		{
			pressure = (sensorValue * 0.004889 - offset0 * 5.0 / 1023.0) * 1.724;
			// DEBUG_MSG("1-Punkt Kalibrierung Senor: %d P: %f offset0: %d \n", sensorValue, pressure, offset0);
		}

		// if (isnan(pressure) || pressure < 0)
		// 	pressure = 0.0;

		if (pressure < 0)
			pressure = 0.0;

		if (fabs(pressure - oldPressDisp) > DELTA) // Absolute Änderung
		{
			reflashLCD = true;
			oldPressDisp = pressure;
		}
	}
	else // Testmodus - Ignorieren!
	{
		oldPressDisp = pressure;
		checkTestMode();
		reflashLCD = true;
	}
}

// !!!Testcode!!! Ignorieren!
void checkTestMode()
{
	switch (setMode)
	{
	case AUS:
		break;
	case SPUNDEN_CO2: // CO2 Spunden
		if (pressure < calcPressure(setCarbonation, temperature))
		{
			pressure = oldPressDisp + 0.1;
		}
		break;
	case SPUNDEN_DRUCK: // Druck Spunden
		if (pressure < setPressure)
		{
			pressure = oldPressDisp - 0.1;
		}
		break;
	case KARBONISIEREN_CO2: // Karbonisieren
		if (pressure > calcPressure(setCarbonation, temperature))
		{
			pressure = oldPressDisp + 0.1;
		}
		break;
	case KARBONISIEREN_DRUCK: // Karbonisieren
		if (pressure > setPressure)
		{
			pressure = oldPressDisp + 0.1;
		}
		break;
	case SPUNDOMAT: // CO2 Spunden & Karbonisieren
		if (!stepA)
		{
			pressure = oldPressDisp + 0.1;
			DEBUG_MSG("Plan !StepA %f\n", pressure);
		}
		else if (!stepB && stepA)
		{
			pressure = oldPressDisp - 0.1;
			DEBUG_MSG("Plan !StepB %f\n", pressure);
		}
		break;
	case PLAN1:
	case PLAN2:
	case PLAN3:
		if (!stepA)
		{
			pressure = oldPressDisp - 0.1;
			DEBUG_MSG("Plan !StepA P %f O %f\n", pressure, oldPressDisp);
		}
		else if (!stepB && stepA)
		{
			pressure = oldPressDisp + 0.1;
			DEBUG_MSG("Plan !StepB P %f O %f\n", pressure, oldPressDisp);
		}
		break;
	}
}
