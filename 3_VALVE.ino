// Öffne Ventil
// Das kleinste Schaltintervall ist 10ms. Kleiner macht das Magnetventil nicht mit.

class Magnetventil
{
	// Klassenvariablen
	int mvPin;			// PIN Magnetventil
	long openInterval;  // Intervall MV öffnen in ms
	long closeInterval; // Intervall MV Schließen in ms

	// Status Magnetventil
	int mvState;			  // Aktueller MV Status
	unsigned long lastTimeMV; // letztes Update MV
	bool enabled;
	int mvMode = 0;			// 0: Normal (Std open/close), 1: Kurz (50ms open und Std close)

public:
	Magnetventil(int pin, long newOpen, long newClose, bool newEnabled) // Konstruktor
	{
		mvPin = pin;			  // GPIO
		pinMode(mvPin, OUTPUT);   // Setze GPIO auf OUTPUT
		openInterval = newOpen;   // Intervall aus config.txt
		closeInterval = newClose; // Intervall aus config.txt
		mvState = LOW;			  // Magnetventil ist geschlossen
		lastTimeMV = 0;			  // Default Zeitstempel
		enabled = newEnabled;	 // Aus der config.txt wird startMV1/2 auf die Klassenvar enabled übertragen
		mvMode = 0;
	}

	Magnetventil(int pin) // Konstrukttor
	{
		mvPin = pin;
		pinMode(mvPin, OUTPUT);
		openInterval = DEFAULT_OPEN;
		closeInterval = DEFAULT_CLOSE;
		mvState = LOW;
		lastTimeMV = 0;
		enabled = false;
		mvMode = 0;
	}

	void change(long newOpen, long newClose, bool newEnabled) // Aufruf aus configfile
	{
		openInterval = newOpen;
		closeInterval = newClose;
		enabled = newEnabled;
		lastTimeMV = 0;
		DEBUG_MSG("Change Pin: %d open: %ld close: %ld\n", mvPin, openInterval, closeInterval);
	}

	void switchOff()
	{
		mvState = LOW;
		digitalWrite(mvPin, mvState);
		lastTimeMV = millis();
		reflashLCD = true;
	}

	void switchOn()
	{
		mvState = HIGH;
		digitalWrite(mvPin, mvState);
		lastTimeMV = millis();
		reflashLCD = true;
	}

	bool getState() // Gibt den aktuellen Status HIGH/LOW von MV zurück
	{
		return mvState;
	}

	unsigned long getElapsed()
	{
		return millis() - lastTimeMV;
	}

	void releasePressureCO2() // MV1 Modus Spunden CO2
	{
		if (!enabled)
		{
			setMode = 0;
			saveConfig();
			return;
		}
		if ((pressure > calcPressure(setCarbonation, temperature)) && (fabs(pressure - calcPressure(setCarbonation, temperature)) > DELTA))
		{
			if ((mvState == HIGH) && (getElapsed() >= openInterval)) // wenn MV1 offen ist, dann schließe es nach openInterval ms
			{
				DEBUG_MSG("MV Spunden-CO2 close P: %f Status: %d current: %lu prevOpen: %lu Intervall: %ld\n", pressure, mvState, millis(), lastTimeMV, openInterval);
				switchOff();
				stepA = true;
			}
			else if ((mvState == LOW) && (getElapsed() >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				DEBUG_MSG("MV Spunden-CO2 open P: %f Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, mvState, millis(), lastTimeMV, openInterval);
				switchOn();
				stepA = false;
			}
			// Test 20200321
			if (setMode == SPUNDOMAT && mvMode == 0) // Reduziere mvOpen
			{
				mvMode = 1;
				change(PAUSE50MS, closeInterval, true);
			}
		}
		else
		{
			if (mvState == HIGH)
				switchOff();
			if (getElapsed() >= closeInterval)
			{
				DEBUG_MSG("MV Spunden-CO2 Delta P: %f Status: %d\n", pressure, mvState);
				lastTimeMV = millis();
				stepA = true;
				return;
			}
			stepA = false;
		}
	}

	void releasePressureDruck() // MV1 Modus Spunden Druck
	{
		if (!enabled)
		{
			setMode = 0;
			saveConfig();
			return;
		}
		if (pressure > setPressure)
		{
			if ((mvState == HIGH) && (getElapsed() >= openInterval))
			{
				switchOff();
				DEBUG_MSG("MV Spunden-Druck close P: %f Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, mvState, millis(), lastTimeMV, openInterval);
			}
			else if ((mvState == LOW) && (getElapsed() >= closeInterval))
			{
				switchOn();
				DEBUG_MSG("MV Spunden-Druck open P: %f Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, mvState, millis(), lastTimeMV, closeInterval);
			}
		}
		else
		{
			if (mvState == HIGH)
				switchOff();
			if (getElapsed() >= openInterval)
			{
				DEBUG_MSG("MV Spunden-Druck delta P: %f Status: %d \n", pressure, mvState);
				lastTimeMV = millis();
				stepA = true;
				return;
			}
			stepA = false;
		}
	}

	void buildPressureDruck() // MV2 Modus Karbonisieren Druck
	{
		if (!enabled)
		{
			setMode = 0;
			saveConfig();
			return;
		}
		if (pressure < setPressure)
		{
			if ((mvState == HIGH) && (getElapsed() >= openInterval))
			{
				switchOff();
				DEBUG_MSG("MV Karb-Druck close P: %f Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, mvState, millis(), lastTimeMV, openInterval);
				stepB = true;
			}
			else if ((mvState == LOW) && (getElapsed() >= closeInterval))
			{
				switchOn();
				DEBUG_MSG("MV Karb-Druck open P: %f Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, mvState, millis(), lastTimeMV, closeInterval);
				stepB = false;
			}
		}
		else
		{
			if (mvState == HIGH)
				switchOff();
			if (getElapsed() >= closeInterval)
			{
				DEBUG_MSG("MV Karb-Druck Delta Status: %d P: %f calcP: %f\n", mvState, pressure, calcPressure(setCarbonation, temperature));
				stepB = true;
				return;
			}
			stepB = false;
		}
	}

	void buildPressureCO2() // MV2 Modus Karbonisieren CO2
	{
		if (!enabled)
		{
			setMode = 0;
			saveConfig();
			return;
		}
		if ((pressure < calcPressure(setCarbonation, temperature)) && (fabs(pressure - calcPressure(setCarbonation, temperature)) > DELTA))
		{
			if ((mvState == HIGH) && (getElapsed() >= openInterval))
			{
				switchOff();
				DEBUG_MSG("MV Karb-CO2 close P: %f Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, mvState, millis(), lastTimeMV, openInterval);
				stepB = true;
			}
			else if ((mvState == LOW) && (getElapsed() >= closeInterval))
			{
				switchOn();
				DEBUG_MSG("MV Karb-CO2 open P: %f Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, mvState, millis(), lastTimeMV, closeInterval);
				stepB = false;
			}
		}
		else
		{
			if (mvState == HIGH)
				switchOff();
			if (getElapsed() >= closeInterval)
			{
				DEBUG_MSG("MV Karb-CO2 Delta Status: %d P: %f calcP: %f\n", mvState, pressure, calcPressure(setCarbonation, temperature));
				lastTimeMV = millis();
				stepB = true;
				return;
			}
			stepB = false;
			// Test 20200321
			if (setMode == SPUNDOMAT && mvMode == 0) // Reduziere mvOpen
			{
				mvMode = 1;
				change(PAUSE50MS, closeInterval, true);
			}
		}
	}

	bool planBuildPress(float newPressure) // Ablaufplan MV2 Karbonisieren
	{
		if (!enabled)
		{
			setMode = 0;
			saveConfig();
			return true;
		}
		if (pressure < newPressure && newPressure > 0.0)
		{
			if ((mvState == HIGH) && (getElapsed() >= openInterval))
			{
				switchOff();
				DEBUG_MSG("MV Plan BuildPress close Pin: %d Pressure: %f Status: %d currentMillis: %lu prevOpen: %lu Intervall: %ld \n", mvPin, pressure, mvState, millis(), lastTimeMV, closeInterval);
			}
			else if ((mvState == LOW) && (getElapsed() >= closeInterval))
			{
				switchOn();
				DEBUG_MSG("MV Plan BuildPress open Pin: %d Pressure: %f Status: %d currentMillis: %lu prevOpen: %lu Intervall: %ld \n", mvPin, pressure, mvState, millis(), lastTimeMV, openInterval);
			}
			return false;
		}
		else
		{
			if (mvState == HIGH)
				switchOff();
			if (getElapsed() >= closeInterval)
			{
				DEBUG_MSG("MV Plan BuildPress Delta Pin: %d Status: %d \n", mvPin, mvState);
				lastTimeMV = millis();
				return true;
			}
			return false;
		}
	}

	bool planRelPress(float newPressure) // Ablaufplan MV1 Spunden
	{
		if (!enabled)
		{
			setMode = 0;
			saveConfig();
			return true;
		}
		if (pressure > newPressure)
		{
			if ((mvState == HIGH) && (getElapsed() >= openInterval))
			{
				switchOff();
				DEBUG_MSG("MV Plan RelPress close Pin: %d Pressure: %f Status: %d currentMillis: %lu prevOpen: %lu Intervall: %ld newPressure: %f\n", mvPin, pressure, mvState, millis(), lastTimeMV, closeInterval, newPressure);
			}
			else if ((mvState == LOW) && (getElapsed() >= closeInterval)) // getElapsed() - closeIntervall
			{
				switchOn();
				DEBUG_MSG("MV Plan RelPress open Pin: %d Pressure: %f Status: %d currentMillis: %lu prevOpen: %lu Intervall: %ld newPressure: %f\n", mvPin, pressure, mvState, millis(), lastTimeMV, openInterval, newPressure);
			}
			return false;
		}
		else
		{
			if (mvState == HIGH)
				switchOff();
			if (getElapsed() >= closeInterval)
			{
				DEBUG_MSG("MV Plan RelPress Delta Pin: %d Status: %d newPressure: %f Pressure: %f\n", mvPin, mvState, newPressure, pressure);
				lastTimeMV = millis();
				return true;
			}
			return false;
		}
	}
};

// Erstelle Objekte MV1 und MV2
Magnetventil mv1 = Magnetventil(PIN_MV1);
Magnetventil mv2 = Magnetventil(PIN_MV2);

void updateMV1() // Modus Spunden
{
	if ((mv1.getState() == LOW && mv2.getState() == LOW))
		TickerPressure.update();

	if (setMode == SPUNDEN_CO2)
		mv1.releasePressureCO2();
	else if (setMode == SPUNDEN_DRUCK)
		mv1.releasePressureDruck();
	else
		mv1.switchOff();
}

void updateMV2() // Modus Karbonisieren
{
	if ((mv1.getState() == LOW && mv2.getState() == LOW))
		TickerPressure.update();

	if (setMode == KARBONISIEREN_CO2)
		mv2.buildPressureCO2();
	else if (setMode == KARBONISIEREN_DRUCK)
		mv2.buildPressureDruck();
	else if (setMode == DICHTHEIT)
		mv2.buildPressureDruck();
	else
		mv2.switchOff();
}

void updateSpundomat() // Spundomat Modus
{
	if ((mv1.getState() == LOW && mv2.getState() == LOW))
		TickerPressure.update();

	//float localCalcPress = calcPressure(setCarbonation, temperature);
	if (!stepA) // true: MV1 aktiv | false: MV1 inaktiv
	{
		// DEBUG_MSG("Spundomat Modus MV1-1 Zieldruck: %f Ist-Druck: %f\n", localCalcPress, pressure);
		mv1.releasePressureCO2();
		return;
	}
	else if (!stepB && stepA) // true: MV2 aktiv | false: MV2 inaktiv
	{
		//DEBUG_MSG("Spundomat Modus Verzögerung: %ld minCarb: %f\n", verzKarbonisierung, minKarbonisierung);
		if (verzKarbonisierung == 0 && minKarbonisierung == 0.0) // Keine Verzögerung Karbonisieren
		{
			// DEBUG_MSG("Spundomat Modus MV2-3 Zieldruck: %f Ist-Druck: %f\n", localCalcPress, pressure);
			mv2.buildPressureCO2();
			return;
		}
		else if (verzKarbonisierung > 0 && millis() > (lastTimeSpundomat + verzKarbonisierung)) // Verzögerung Karbonisieren Zeit
		{
			lastTimeSpundomat = millis();
			verzKarbonisierung = 0;
			// DEBUG_MSG("Spundomat Modus MV2-2 Zieldruck: %f Ist-Druck: %f\n", localCalcPress, pressure);
			mv2.buildPressureCO2();
			return;
		}
		else if (minKarbonisierung > 0.0 && minKarbonisierung < calcCarbonation(pressure, temperature)) // Verzögerung min CO2-Gehalt
		{
			// DEBUG_MSG("Spundomat Modus MV2-3 Zieldruck: %f Ist-Druck: %f\n", localCalcPress, pressure);
			mv2.buildPressureCO2();
			return;
		}
		else
			stepB = true;

		return;
	}
	else if (stepA && stepB)
	{
		stepA = false; // Setze StepA aktiv
		if (millis() > (lastTimeSpundomat + verzKarbonisierung))
			stepB = false; // Setze StepB aktiv
		else
			stepB = true;
	}
}

void startPlan() // Modus Ablaufplan
{
	if ((mv1.getState() == LOW && mv2.getState() == LOW))
		TickerPressure.update();

	if (counterPlan == -1) // Start Ablaufplan (-1 ist der Startindikator)
	{
		counterPlan = 0; // Setze auf erstes Element im Struct Array Ablaufplan
		DEBUG_MSG("Counterplan 0: #%d\n", counterPlan);
		mv2.change(structPlan[counterPlan].intervallMV2Open, structPlan[counterPlan].intervallMV2Close, true);
		mv1.change(structPlan[counterPlan].intervallMV1Open, structPlan[counterPlan].intervallMV1Close, true);
		readPressure();
	}

	if (!stepA) // true: MV1 aktiv | false: MV1 inaktiv
	{
		// DEBUG_MSG("startPlan StepA #%d Zieldruck: %f Ist-Druck: %f\n", counterPlan, structPlan[counterPlan].zieldruckMV1, pressure);
		if (structPlan[counterPlan].zieldruckMV1 == 0.0)
		{
			stepA = true;
			return;
		}
		stepA = mv1.planRelPress(structPlan[counterPlan].zieldruckMV1);
		displayPressure = structPlan[counterPlan].zieldruckMV1;
		setPressure = structPlan[counterPlan].zieldruckMV1;
		if (stepA)
			millis2wait(PAUSE100MS);
		return;
	}
	else if (!stepB && stepA) // true: MV2 aktiv | false: MV2 inaktiv
	{
		// DEBUG_MSG("startPlan StepB #%d Zieldruck: %f Ist-Druck: %f\n", counterPlan, structPlan[counterPlan].zieldruckMV2, pressure);
		if (structPlan[counterPlan].zieldruckMV2 == 0.0)
		{
			stepB = true;
			return;
		}
		stepB = mv2.planBuildPress(structPlan[counterPlan].zieldruckMV2);
		displayPressure = structPlan[counterPlan].zieldruckMV2;
		setPressure = structPlan[counterPlan].zieldruckMV2;
		if (stepB)
			millis2wait(PAUSE100MS);
		return;
	}
	else if (stepA && stepB)
	{
		counterPlan++; // Nächster Schritt im Plan
		if (counterPlan == maxSchritte)
		{
			setMode = 0;
			saveConfig();
			DEBUG_MSG("Counterplan Ende %d\n", counterPlan);
			return;
		}
		stepA = false;			// Setze StepA aktiv
		stepB = false;			// Setze StepB aktiv
		millis2wait(PAUSE2SEC); // StepA und StepB abgeschlossen -> kurze Pause
		DEBUG_MSG("Counterplan: #%d\n", counterPlan);
		// Setze Intervalle für den nächsten Schritt
		mv1.change(structPlan[counterPlan].intervallMV1Open, structPlan[counterPlan].intervallMV1Close, true);
		mv2.change(structPlan[counterPlan].intervallMV2Open, structPlan[counterPlan].intervallMV2Close, true);
	}
}
