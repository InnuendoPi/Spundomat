// Öffne Ventil
// Das kleinste Schaltintervall ist 10ms. Kleiner macht das Magnetventil nicht mit.

class Magnetventil
{
	// Klassenvariablen
	int mvPin;			// PIN Magnetventil
	long openInterval;  // Intervall MV öffnen in ms
	long closeInterval; // Intervall MV Schließen in ms

	// Status Magnetventil
	int mvState;				  // Aktueller MV Status
	int prevMVState;			  // vorheriger MV Status
	unsigned long previousMillis; // letztes Update MV
	bool enabled;

public:
	Magnetventil(int pin, long newOpen, long newClose, bool newEnabled) // Konstruktor
	{
		mvPin = pin;			  // GPIO
		pinMode(mvPin, OUTPUT);   // Setze GPIO auf OUTPUT
		openInterval = newOpen;   // Intervall aus config.txt
		closeInterval = newClose; // Intervall aus config.txt
		mvState = LOW;			  // Magnetventil ist geschlossen
		prevMVState = LOW;		  // Default Status
		previousMillis = 0;		  // Default Zeitstempel
		enabled = newEnabled;	 // Aus der config.txt wird startMV1/2 auf die Klassenvar enabled übertragen
	}

	Magnetventil(int pin) // Konstrukttor
	{
		mvPin = pin;
		pinMode(mvPin, OUTPUT);
		openInterval = DEFAULT_OPEN;
		closeInterval = DEFAULT_CLOSE;
		mvState = LOW;
		prevMVState = LOW;
		previousMillis = 0;
		enabled = false;
	}

	void releasePressureCO2() // MV1 Modus Spunden CO2
	{
		if (!enabled)
			return;
		unsigned long currentMillis = millis();
		if ((pressure > calcPressure(setCarbonation, temperature)) && (fabs(pressure - calcPressure(setCarbonation, temperature)) > DELTA))
		{
			// DEBUG_MSG("Delta Release: %f\n", fabs(pressure - calcPressure(setCarbonation, temperature)));
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV1 offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Modus Spunden-CO2 close P: %f prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				previousMillis = currentMillis;
				stepA = true; // Spundomat Modus
				reflashLCD = true;
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Modus Spunden-CO2 open P: %f prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, prevMVState, mvState, currentMillis, previousMillis, openInterval);
				previousMillis = currentMillis;
				stepA = false; // Spundomat Modus
				reflashLCD = true;
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(100); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Modus Spunden-CO2 Delta P: %f Status: %d \n", pressure, mvState);
			if (currentMillis - previousMillis >= openInterval)
			{
				readPressure();
				previousMillis = currentMillis;
			}
			stepA = true; // Spundomat Modus
		}
	}

	void releasePressureDruck() // MV1 Modus Spunden Druck
	{
		if (!enabled) // MV1 aktiviert?
			return;
		unsigned long currentMillis = millis();
		if (pressure > setPressure)
		{
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Modus Spunden-Druck close P: %f prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				previousMillis = currentMillis;
				reflashLCD = true;
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Modus Spunden-Druck open P: %f prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, prevMVState, mvState, currentMillis, previousMillis, openInterval);
				previousMillis = currentMillis;
				reflashLCD = true;
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(100); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Modus Spunden-Druck delta P: %f Status: %d \n", pressure, mvState);
			if (currentMillis - previousMillis >= openInterval)
			{
				readPressure();
				previousMillis = currentMillis;
			}
		}
	}

	void buildPressureDruck() // MV2 Modus Karbonisieren
	{
		if (!enabled) // MV2 aktiviert?
			return;
		unsigned long currentMillis = millis();
		if (pressure < setPressure)
		{
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Modus Karbonisieren close P: %f prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				previousMillis = currentMillis;
				stepB = true; // Kombi-Modus
				reflashLCD = true;
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Magnetventil
				DEBUG_MSG("Magnetventil Modus Karbonisieren open P: %f prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, prevMVState, mvState, currentMillis, previousMillis, openInterval);
				previousMillis = currentMillis;
				stepB = false; // Kombi-Modus
				reflashLCD = true;
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(100); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Modus Karbonisieren delta Status: %d P: %f calcP: %f\n", mvState, pressure, calcPressure(setCarbonation, temperature));
			if (currentMillis - previousMillis >= openInterval)
			{
				readPressure();
				previousMillis = currentMillis;
			}
			stepB = true;
		}
	}

	void buildPressureCO2() // MV2 Modus Karbonisieren
	{
		if (!enabled) // MV2 aktiviert?
			return;
		unsigned long currentMillis = millis();
		if ((pressure < calcPressure(setCarbonation, temperature)) && (fabs(pressure - calcPressure(setCarbonation, temperature)) > DELTA))
		{
			// DEBUG_MSG("Delta Build: %f\n", fabs(pressure - calcPressure(setCarbonation, temperature)));
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Modus Karbonisieren close P: %f prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				previousMillis = currentMillis;
				stepB = true; // Spundomat Modus
				reflashLCD = true;
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Magnetventil
				DEBUG_MSG("Magnetventil Modus Karbonisieren open P: %f prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", pressure, prevMVState, mvState, currentMillis, previousMillis, openInterval);
				previousMillis = currentMillis;
				stepB = false; // Spundomat Modus
				reflashLCD = true;
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(100); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Modus Karbonisieren delta Status: %d P: %f calcP: %f\n", mvState, pressure, calcPressure(setCarbonation, temperature));
			if (currentMillis - previousMillis >= openInterval)
			{
				readPressure();
				previousMillis = currentMillis;
			}
			stepB = true;
		}
	}

	bool planBuildPress(float newPressure) // MV2 Modus Karbonisieren
	{
		if (!enabled) // MV2 aktiviert?
		{
			setMode = 0;
			return true;
		}
		unsigned long currentMillis = millis();
		if (pressure < newPressure && newPressure > 0.0)
		{
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Pin: %d Modus planBuildPress1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", mvPin, prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				previousMillis = currentMillis;
				reflashLCD = true;
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Magnetventil
				DEBUG_MSG("Magnetventil Pin: %d Modus planBuildPress2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", mvPin, prevMVState, mvState, currentMillis, previousMillis, openInterval);
				previousMillis = currentMillis;
				reflashLCD = true;
			}
			return false;
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(100); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Pin: %d Modus planBuildPress3 Status: %d \n", mvPin, mvState);
			return true;
		}
	}

	bool planRelPress(float newPressure) // MV1 Modus Spunden Druck
	{
		if (!enabled) // MV1 aktiviert?
		{
			setMode = 0;
			return true;
		}
		unsigned long currentMillis = millis();
		if (pressure > newPressure)
		{
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Pin: %d Modus planRelPress1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld newPressure: %f\n", mvPin, prevMVState, mvState, currentMillis, previousMillis, closeInterval, newPressure);
				previousMillis = currentMillis;
				reflashLCD = true;
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure(); // Lese Drucksensor, bevor MV geöffnet wird
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				DEBUG_MSG("Magnetventil Pin: %d Modus planRelPress2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld newPressure: %f\n", mvPin, prevMVState, mvState, currentMillis, previousMillis, openInterval, newPressure);
				previousMillis = currentMillis;
				reflashLCD = true;
			}
			return false;
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(200); // Schutz Magnetventil
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Pin: %d Modus planRelPress3 Status: %d newPressure: %f Pressure: %f\n", mvPin, mvState, newPressure, pressure);
			return true;
		}
	}

	void change(long newOpen, long newClose, bool newEnabled) // Aufruf aus configfile
	{
		openInterval = newOpen;
		closeInterval = newClose;
		enabled = newEnabled;
		previousMillis = 0;
		DEBUG_MSG("Change Pin: %d open: %ld close: %ld\n", mvPin, openInterval, closeInterval);
	}
	void switchOff() // Zusätzlicher Ausschalter
	{
		mvState = LOW;
		digitalWrite(mvPin, mvState);
		//DEBUG_MSG("Switched off %d status: %d\n", mvPin, mvState);
	}
	bool getState() // Gibt den aktuellen Status HIGH/LOW von MV zurück
	{
		return mvState;
	}
};

// Erstelle Objekte MV1 und MV2
Magnetventil mv1 = Magnetventil(PIN_MV1);
Magnetventil mv2 = Magnetventil(PIN_MV2);

void updateMV1() // Modus Spunden
{
	if (setMode == SPUNDEN_CO2)
		mv1.releasePressureCO2();
	else if (setMode == SPUNDEN_DRUCK)
		mv1.releasePressureDruck();
	else
		mv1.switchOff();
}

void updateMV2() // Modus Karbonisieren
{
	if (setMode == KARBONISIEREN_CO2)
		mv2.buildPressureCO2();
	else if (setMode == KARBONISIEREN_DRUCK)
		mv2.buildPressureDruck();
	else
		mv2.switchOff();
}

void updateSpundomat() // Spundomat Modus
{
	float localCalcPress = calcPressure(setCarbonation, temperature);
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
		else if (verzKarbonisierung > 0 && millis() > (prevMillis + verzKarbonisierung)) // Verzögerung Karbonisieren Zeit
		{
			prevMillis = millis();
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
		if (millis() > (prevMillis + verzKarbonisierung))
			stepB = false; // Setze StepB aktiv
		else
			stepB = true;
	}
}

void startPlan() // Modus Ablaufplan
{
	if (counterPlan == -1) // Start Ablaufplan (-1 ist der Startindikator)
	{
		counterPlan = 0; // Setze auf erstes Element im Struct Array Ablaufplan
		readPressure();
		DEBUG_MSG("Counterplan 0: #%d\n", counterPlan);
		mv2.change(structPlan[counterPlan].intervallMV2Open, structPlan[counterPlan].intervallMV2Close, true);
		mv1.change(structPlan[counterPlan].intervallMV1Open, structPlan[counterPlan].intervallMV1Close, true);
	}
	if (!stepA) // true: MV1 aktiv | false: MV1 inaktiv
	{
		// DEBUG_MSG("startPlan StepA #%d/%d Zieldruck: %f Ist-Druck: %f\n", counterPlan, count, structPlan[counterPlan].zieldruckMV1, pressure);
		if (structPlan[counterPlan].zieldruckMV1 == 0.0)
		{
			stepA = true;
			return;
		}
		stepA = mv1.planRelPress(structPlan[counterPlan].zieldruckMV1);
		displayPressure = structPlan[counterPlan].zieldruckMV1;
		if (stepA)
			millis2wait(PAUSE100MS);
		return;
	}
	else if (!stepB && stepA) // true: MV2 aktiv | false: MV2 inaktiv
	{
		// DEBUG_MSG("startPlan StepB #%d/%d Zieldruck: %f Ist-Druck: %f\n", counterPlan, count, structPlan[counterPlan].zieldruckMV2, pressure);
		if (structPlan[counterPlan].zieldruckMV2 == 0.0)
		{
			stepB = true;
			return;
		}
		stepB = mv2.planBuildPress(structPlan[counterPlan].zieldruckMV2);
		displayPressure = structPlan[counterPlan].zieldruckMV2;
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
			DEBUG_MSG("Counterplan Ende %d\n", counterPlan);
			return;
		}
		stepA = false;		 // Setze StepA aktiv
		stepB = false;		 // Setze StepB aktiv
		millis2wait(PAUSE2SEC); // StepA und StepB abgeschlossen -> kurze Pause
		DEBUG_MSG("Counterplan: #%d\n", counterPlan);
		// Setze Intervalle für den nächsten Schritt
		mv1.change(structPlan[counterPlan].intervallMV1Open, structPlan[counterPlan].intervallMV1Close, true);
		mv2.change(structPlan[counterPlan].intervallMV2Open, structPlan[counterPlan].intervallMV2Close, true);
	}
}
