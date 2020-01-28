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
		if (pressure > calcPressure(setCarbonation, temperature))
		{
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV1 offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Modus Spunden-CO2-1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				// readPressure();
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Modus Spunden-CO2-2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", prevMVState, mvState, currentMillis, previousMillis, openInterval);
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(200); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Modus Spunden-CO2-3 Status: %d \n", mvState);
			if (currentMillis - previousMillis >= openInterval)
			{
				readPressure();
				previousMillis = currentMillis;
			}
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
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Modus Spunden-Druck1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", prevMVState, mvState, currentMillis, previousMillis, closeInterval);
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Modus Spunden-Druck2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", prevMVState, mvState, currentMillis, previousMillis, openInterval);
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(200); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Modus Spunden-Druck3 Status: %d \n", mvState);
			if (currentMillis - previousMillis >= openInterval)
			{
				readPressure();
				previousMillis = currentMillis;
			}
		}
	}

	void buildPressure() // MV2 Modus Karbonisieren
	{
		if (!enabled) // MV2 aktiviert?
			return;
		unsigned long currentMillis = millis();
		if (pressure < calcPressure(setCarbonation, temperature))
		{
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Modus Karbonisieren1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", prevMVState, mvState, currentMillis, previousMillis, closeInterval);
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Magnetventil
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Modus Karbonisieren2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", prevMVState, mvState, currentMillis, previousMillis, openInterval);
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(200); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil Modus Karbonisieren3 Status: %d \n", mvState);
			if (currentMillis - previousMillis >= openInterval)
			{
				readPressure();
				previousMillis = currentMillis;
			}
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
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Pin: %d Modus planBuildPress1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", mvPin, prevMVState, mvState, currentMillis, previousMillis, closeInterval);
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure();
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Magnetventil
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Pin: %d Modus planBuildPress2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", mvPin, prevMVState, mvState, currentMillis, previousMillis, openInterval);
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
		// if (!enabled) // MV2 aktiviert?
		// {
		// 	setMode = 0;
		// 	return true;
		// }
		//readPressure();
		unsigned long currentMillis = millis();
		if (pressure > newPressure)
		{
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				prevMVState = mvState;
				mvState = LOW;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Pin: %d Modus planRelPress1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld newPressure: %f\n", mvPin, prevMVState, mvState, currentMillis, previousMillis, closeInterval, newPressure);
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				readPressure(); // Lese Drucksensor, bevor MV geöffnet wird
				prevMVState = mvState;
				mvState = HIGH;
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
				previousMillis = currentMillis;
				DEBUG_MSG("Magnetventil Pin: %d Modus planRelPress2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld newPressure: %f\n", mvPin, prevMVState, mvState, currentMillis, previousMillis, openInterval, newPressure);
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
		DEBUG_MSG("Switched off %d status: %d\n", mvPin, mvState);
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
	if (setMode != KARBONISIEREN)
		mv2.switchOff();
	else
		mv2.buildPressure();
}

void setPlanPause()
{
	DEBUG_MSG("setPlanPause: %d\n", PAUSE1SEC); 
	millis2wait(PAUSE1SEC);
}

void startPlan(int count, Ablaufplan *newPlan) // Modus Ablaufplan
{
	if (counterPlan == -1) // Start Ablaufplan (-1 ist der Startindikator)
	{
		counterPlan = 0; // Setze auf erstes Element im Struct Array Ablaufplan
		readPressure();
		DEBUG_MSG("Counterplan 0: #%d/%d\n", counterPlan, count);
		mv2.change(newPlan[counterPlan].intervallMV2Open, newPlan[counterPlan].intervallMV2Close, true);
		mv1.change(newPlan[counterPlan].intervallMV1Open, newPlan[counterPlan].intervallMV1Close, true);
	}
	if (!stepA) // true: MV1 aktiv | false: MV1 inaktiv
	{
		// DEBUG_MSG("startPlan StepA #%d/%d Zieldruck: %f Ist-Druck: %f\n", counterPlan, count, newPlan[counterPlan].zieldruckMV1, pressure);
		stepA = mv1.planRelPress(newPlan[counterPlan].zieldruckMV1);
		displayPressure = newPlan[counterPlan].zieldruckMV1;
		if (stepA)
			millis2wait(newPlan[counterPlan].intervallMV1Close);
		return;
	}
	else if (!stepB && stepA) // true: MV2 aktiv | false: MV2 inaktiv
	{
		// DEBUG_MSG("startPlan StepB #%d/%d Zieldruck: %f Ist-Druck: %f\n", counterPlan, count, newPlan[counterPlan].zieldruckMV2, pressure);
		stepB = mv2.planBuildPress(newPlan[counterPlan].zieldruckMV2);
		displayPressure = newPlan[counterPlan].zieldruckMV2;
		if (stepB)
			millis2wait(newPlan[counterPlan].intervallMV2Close);
		return;
	}
	else if (stepA && stepB)
	{
		counterPlan++; // Nächster Schritt im Plan
		if (counterPlan == count)
		{
			setMode = 0;
			DEBUG_MSG("Counterplan Ende %d: #%d/%d\n", count, counterPlan, count);
			return;
		}
		stepA = false; // Setze StepA aktiv
		stepB = false; // Setze StepB aktiv
		setPlanPause(); // StepA und StepB abgeschlossen -> kurze Pause
		DEBUG_MSG("Counterplan: #%d/%d\n", counterPlan, count);
		// Setze Intervalle für den nächsten Schritt
		mv1.change(newPlan[counterPlan].intervallMV1Open, newPlan[counterPlan].intervallMV1Close, true);
		mv2.change(newPlan[counterPlan].intervallMV2Open, newPlan[counterPlan].intervallMV2Close, true);
	}
}
