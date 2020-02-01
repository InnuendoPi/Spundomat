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

// Lese Druck
void readPressure()
{
	int sensorValue = analogRead(A0);
	// Skaliere Analogwert auf 5V
	voltage = (sensorValue * 5.0) / 1023.0;

	// Berechne Druck offsetVoltage = 0.42;
	// pressure = (voltage - offsetVoltage) * 1.36;

	// if (isnan(pressure))
	// 	pressure = -1;

	//Pbar=(VALadc*1/(1023*D)-Offset)*Vbar
	//(%value%*0.004889-0.5)*1.724

	pressure = (sensorValue * 0.004889 - offsetVoltage) * 1.724;
	//pressure = (sensorValue * 0.004889 - 0.5 + offsetVoltage) * 1.724;
	//pressure = (sensorValue * 0.004889 - 0.5) * 1.724;

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
	// }
	// else
	// 	reflashLCD = true;

	// Negativer Druck
	if (pressure < 0.0)
		pressure = 0.0;
}

// Lese Druck

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
	case KARBONISIEREN: // Karbonisieren
		if (pressure < calcPressure(setCarbonation, temperature))
		{
			pressure = oldPressDisp + 0.015;
		}
		break;
	case KOMBIMODUS: // CO2 Spunden & Karbonisieren
		//if (pressure > calcPressure(setCarbonation, temperature))
		if (!stepA)
		{
			pressure = oldPressDisp - 0.015;
			DEBUG_MSG("%s", "minus\n");
		}
		//if (pressure < calcPressure(setCarbonation, temperature))
		if (!stepB)
		{
			pressure = oldPressDisp + 0.015;
			DEBUG_MSG("%s", "plus\n");
		}
		break;
	// Nicht getestet!
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
