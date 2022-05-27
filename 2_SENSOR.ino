// Lese Temperatur
void readTemparature()
{
	sensors.requestTemperatures();
	sensors.setResolution(RESOLUTION);
	temperature = sensors.getTempCByIndex(0);
	if (testModus)
		checkTestMode();
	// temperature = 20.0;

	if (temperature == 85.0 || temperature == -127.00)
	{
		millis2wait(PAUSE1SEC);
		sensors.requestTemperatures();
		temperature = sensors.getTempCByIndex(0);
		if (temperature == 85.0 || temperature == -127.00) // DS18B20 im Fehlerstatus
		{
			if (setMode != AUS)
			{
				setMode = AUS;
				newMode = setMode;
				reflashLCD = true;
				if (setGPIO == 1)
					sendAlarm(ALARM_ERROR);
				saveConfig();
				lastTimeSpundomat = millis();
				return;
			}
			else
			{
				if (millis() > (lastTimeSpundomat + PAUSE2MIN) && setGPIO == 1)
				{
					sendAlarm(ALARM_ERROR);
					lastTimeSpundomat = millis();
				}
			}
		}
	}

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
			//reflashLCD = false;
			//DEBUG_MSG("Keine Kalibrierung Sensor: %d P: %f\n", sensorValue, pressure);
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

void checkDichtheit()
{

	if (pressure < setPressure && lastTimeSpundomat == 0.0) // Step 1 Befülle Keg
	{
		updateMV2();
		return;
	}
	else if (pressure >= setPressure && lastTimeSpundomat == 0.0) // Step 2 Zieldruck im Keg erreicht
	{
		dichtPressure = 0.0;
		lastTimeSpundomat = millis();
		DEBUG_MSG("Überprüfung Dichtheit Step 2 Zeitstempel: %lu \n", lastTimeSpundomat);
		sendAlarm(ALARM_OK);
		// TickerDisplay.start();
		TickerTemp.interval(PAUSE2SEC);
		TickerPressure.interval(PAUSE2SEC);
		return;
	}
	else if (lastTimeSpundomat > 0.0 && dichtPressure == 0.0 && millis() > (lastTimeSpundomat + PAUSE2MIN)) // Step 3 Warte 2min (Gleichgewicht)
	{
		readPressure();
		dichtPressure = pressure;
		ergDichtheit = 0.0;
		DEBUG_MSG("Überprüfung Dichtheit Step 3 dichtP: %f Elapsed %lu \n", dichtPressure, (millis() - lastTimeSpundomat));
		sendAlarm(ALARM_OK);
		return;
	}
	else if (lastTimeSpundomat > 0.0 && millis() > (lastTimeSpundomat + PAUSE5MIN)) // Step 4 nach 5min ermittle Delta
	{
		readPressure();
		ergDichtheit = pressure - dichtPressure;
		DEBUG_MSG("Überprüfung Dichtheit Step 4 Delta %f Elapsed %lu \n", (pressure - dichtPressure), (millis() - lastTimeSpundomat));
		// TickerDisplay.stop();
		TickerTemp.interval(upTemp);
		TickerPressure.interval(upPressure);
		setMode = AUS;
		saveConfig();
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
	case DICHTHEIT:
		if (pressure < setPressure)
			pressure = oldPressDisp + 0.1;
		break;
	// case STEUERUNG:
	// case CON1:
	// case CON2:
	// 	if (targetTemp <= oldTemp)
	// 		oldTemp -= 0.2;
	// 	else
	// 		oldTemp += 0.2;
	// 	temperature = oldTemp;
	// 	DEBUG_MSG("TestMode Temp %3.2f Target %3.2f\n", temperature, targetTemp);
	// 	break;
	}
}
