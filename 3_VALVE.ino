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
		if (pressure > calcPressure(setCarbonation, temperature))
		{
			unsigned long currentMillis = millis();
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV1 offen ist, dann schließe es nach openInterval ms
			{
				previousMillis = currentMillis;
				prevMVState = mvState;
				mvState = LOW;
				DEBUG_MSG("Magnetventil an %s Modus Spunden-CO2-1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", PinToString(mvPin).c_str(), prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				previousMillis = currentMillis;
				prevMVState = mvState;
				mvState = HIGH;
				DEBUG_MSG("Magnetventil an %s Modus Spunden-CO2-2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", PinToString(mvPin).c_str(), prevMVState, mvState, currentMillis, previousMillis, openInterval);
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(20); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil an %s Modus Spunden-CO2-3 Status: %d \n", PinToString(mvPin).c_str(), mvState);
		}
	}

	void releasePressureDruck() // MV1 Modus Spunden Druck
	{
		if (!enabled) // MV1 aktiviert?
			return;
		if (pressure > setPressure)
		{
			unsigned long currentMillis = millis();
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				previousMillis = currentMillis;
				prevMVState = mvState;
				mvState = LOW;
				DEBUG_MSG("Magnetventil an %s Modus Spunden-Druck1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", PinToString(mvPin).c_str(), prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				previousMillis = currentMillis;
				prevMVState = mvState;
				mvState = HIGH;
				DEBUG_MSG("Magnetventil an %s Modus Spunden-Druck2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", PinToString(mvPin).c_str(), prevMVState, mvState, currentMillis, previousMillis, openInterval);
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(20); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil an %s Modus Spunden-Druck3 Status: %d \n", PinToString(mvPin).c_str(), mvState);
		}
	}

	void buildPressure() // MV2 Modus Karbonisieren
	{
		if (!enabled) // MV2 aktiviert?
			return;
		if (pressure < calcPressure(setCarbonation, temperature))
		{
			unsigned long currentMillis = millis();
			if ((mvState == HIGH) && (currentMillis - previousMillis >= openInterval)) // wenn MV offen ist, dann schließe es nach openInterval ms
			{
				previousMillis = currentMillis;
				prevMVState = mvState;
				mvState = LOW;
				DEBUG_MSG("Magnetventil an %s Modus Karbonisieren1 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", PinToString(mvPin).c_str(), prevMVState, mvState, currentMillis, previousMillis, closeInterval);
				digitalWrite(mvPin, mvState); // Update Status Magnetventil
			}
			else if ((mvState == LOW) && (currentMillis - previousMillis >= closeInterval)) // wenn MV geschlossen ist, dann öffne es nach closeInterval ms
			{
				previousMillis = currentMillis;
				prevMVState = mvState;
				mvState = HIGH;
				DEBUG_MSG("Magnetventil an %s Modus Karbonisieren2 prevStatus: %d Status: %d current: %lu prevOpen: %lu Intervall: %ld \n", PinToString(mvPin).c_str(), prevMVState, mvState, currentMillis, previousMillis, openInterval);
				digitalWrite(mvPin, mvState); // Update Magnetventil
			}
		}
		else // hier muss das Magnetventil immer geschlossen werden
		{
			mvState = LOW;
			millis2wait(20); // Verzögerung um 2 Schaltvorgänge unter 10ms zu vermeiden
			digitalWrite(mvPin, mvState);
			DEBUG_MSG("Magnetventil an %s Modus Karbonisieren3 Status: %d \n", PinToString(mvPin).c_str(), mvState);
		}
	}

	void change(long newOpen, long newClose, bool newEnabled) // Aufruf aus configfile
	{
		openInterval = newOpen;
		closeInterval = newClose;
		enabled = newEnabled;
		previousMillis = 0;
		DEBUG_MSG("Change PIN: %s open: %ld close: %ld\n", PinToString(mvPin).c_str(), openInterval, closeInterval);
	}
	void switchOff() // Zusätzlicher Ausschalter
	{
		mvState = LOW;
		digitalWrite(mvPin, mvState);
		DEBUG_MSG("Switched off PIN: %s status: %d\n", PinToString(mvPin).c_str(), mvState);
	}
	bool getState() // Gibt den aktuellen Status HIGH/LOW von MV zurück
	{
		return mvState;
	}
};

// Erstelle Objekte MV1 und MV2
Magnetventil mv1 = Magnetventil(PIN_MV1);
Magnetventil mv2 = Magnetventil(PIN_MV2);

void updateMV1()
{
	if (setMode == SPUNDEN_CO2)
		mv1.releasePressureCO2();
	else if (setMode == SPUNDEN_DRUCK)
		mv1.releasePressureDruck();
	else
		mv1.switchOff();
}
void updateMV2()
{
	if (setMode != KARBONISIEREN)
		mv2.switchOff();
	else
		mv2.buildPressure();
}
