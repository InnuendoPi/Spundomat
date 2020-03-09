// Lese Temperatur
void readTemparature()
{
	// sensors.requestTemperatures();
	//temperature = sensors.getTempCByIndex(0);

	sensors.requestTemperatures();
	bool sens_isConnected = sensors.isConnected(0);
	sens_isConnected ? temperature = sensors.getTempCByIndex(0) : temperature = -127.0;

	if (temperature == 85.0 || temperature == -127.00)
	{
		millis2wait(PAUSE1SEC);
		sensors.requestTemperatures();
		temperature = sensors.getTempCByIndex(0);
		if (temperature == 85.0 || temperature == -127.00) // DS18B20 im Fehlerstatus
		{
			setMode = AUS;
			if (startBuzzer)
    			sendAlarm(ALARM_ERROR);
			
			saveConfig();
			return;
		}
	}
	else

		if (testModus)
		temperature = 20.0;

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
		else if (offset2 > 0 && offset0 > 0) // 2-Punkte-Kalibrierung
		{
			float m = (PRESSURE_OFFSET2 - PRESSURE_OFFSET0) / (offset2 - offset0);
			float b = PRESSURE_OFFSET2 - m * offset2;
			pressure = m * sensorValue + b;
		}
		else if (offset0 != 0.0 && offset2 == 0.0) // 1-Punkt-Kalibrierung
		{
			pressure = (sensorValue * 0.004889 - offset0 * 5.0 / 1023.0) * 1.724;
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
		if (pressure > calcPressure(setCarbonation, temperature))
		{
			pressure = oldPressDisp - 0.15;
		}
		else
			pressure = 3.0;
		break;
	case SPUNDEN_DRUCK: // Druck Spunden
		if (pressure < setPressure)
		{
			pressure = oldPressDisp - 0.15;
		}
		else
			pressure = 3.0;
		break;
	case KARBONISIEREN_CO2: // Karbonisieren
		if (pressure < calcPressure(setCarbonation, temperature))
		{
			pressure = oldPressDisp + 0.15;
		}
		else
			pressure = 0.2;
		break;
	case KARBONISIEREN_DRUCK: // Karbonisieren
		if (pressure < setPressure)
		{
			pressure = oldPressDisp + 0.15;
		}
		else
			pressure = 0.2;
		break;
	case SPUNDOMAT: // CO2 Spunden & Karbonisieren

		if (pressure > calcPressure(setCarbonation, temperature))
		{
			DEBUG_MSG("Größer P: %f O: %f A: %d B: %d\n", pressure, oldPressDisp, stepA, stepB);
			pressure = oldPressDisp - 0.15;
		}
		else if (pressure < calcPressure(setCarbonation, temperature))
		{
			DEBUG_MSG("Kleiner P: %f O: %f A: %d B: %d\n", pressure, oldPressDisp, stepA, stepB);
			pressure = oldPressDisp + 0.15;
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
