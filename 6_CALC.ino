/**
 * Carbonation tables show the CO2 content of the beer based on the beer 
 * temperature and CO2 head pressure once an equilibrium has been reached. 
 * Using this formula [HOBBYBRAUER]:
 * 
 * Cbeer = (Phead+1.013)*(2.71828182845904^(-10.73797+(2617.25/(Tbeer+273.15))))*10
 * Cbeer - carbonation of the beer in g/l
 * Phead - head pressure in bar
 * Tbeer - temperature of the beer in C
 * 
 * http://braukaiser.com/wiki/index.php/Carbonation_Tables
**/

// Berechne CO2 gehalt
float calcCarbonation(float Pressure, float Temperature)
{
	if (temperature == 85.0 || temperature == -127.00)
		return 0.0;
	float localCarbonation;
	localCarbonation = (Pressure + 1.013) * pow(E, (-10.73797 + (2617.25 / (Temperature + 273.15)))) * 10;
	return localCarbonation;
}

// Berechne Solldruck
float calcPressure(float Carbonation, float Temperature)
{
	if (temperature == 85.0 || temperature == -127.00)
		return 0.0;
	float localPressure;
	localPressure = (Carbonation / 10) / pow(E, (-10.73797 + (2617.25 / (Temperature + 273.15)))) - 1.013;
	return localPressure;
}

// Lese EEPROM - Kalibrierung
float readFloat(unsigned int addr)
{
	union {
		byte b[4];
		float f;
	} data;
	for (int i = 0; i < 4; i++)
	{
		data.b[i] = EEPROM.read(addr + i);
	}
	if (isnan(data.f)) // Keine Kalibrierung
		data.f = 0;
	
	return data.f;
}

// Schreibe EEPROM - Kalibrierung
void writeFloat(unsigned int addr, float x)
{
	union {
		byte b[4];
		float f;
	} data;
	data.f = x;
	for (int i = 0; i < 4; i++)
	{
		EEPROM.write(addr + i, data.b[i]);
		EEPROM.commit();
	}
}
