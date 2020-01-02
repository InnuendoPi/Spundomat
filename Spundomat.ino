/*
 Basisquellcode
 Name:		Spundomat.ino
 Created:	18.11.2017 00:24:56
 Author:	Janick

 Erweiterung
 Created:	27.12.2019
 Author:	Innuendo

Spunden https://hobbybrauer.de/forum/wiki/doku.php/lagern:spunden

Unter dem Spunden versteht man Bier in einem druckdicht abgeschlossenen Faß oder Gärbehälter 
kontrolliert zu Carbonisieren damit Kohlendioxid im Bier gelöst wird. 
Um eine Explosion des Druckfasses zu vermeiden und einen erwünschten Kohlensäuregehalt des Bieres 
laut Spundungstabelle zu erreichen muss ein einstellbares Druckbegrenzungsventil, 
das Spundventil (Spunddruckregler) am Fass angebracht werden. 
Bei der Spundung muss keine genaue Menge an Zucker oder Würze zugegeben und berechnet werden, 
weil das Spundventil überschüssiges CO2 automatisch ablässt.
Am einfachsten schlaucht man nach 5-7 Tagen in die Drucktanks oder Kegs und hängt das nach 
Tanktemperatur (bzw. Raumtemperatur wenn nicht temperaturreguliert) richtig eingestellte Spundventil an.

Zum Anschließen, Umkonfigurieren, Carbonisieren und leichten Reinigen der CO2-Schäuche eignen 
sich Pneumatische Schnellkupplungen hervorragend.
*/

#include <EEPROM.h>
#include "RotaryEncoder.h"
#include "OneButton.h"
#include <OneWire.h>
#include "DallasTemperature.h"
#include <Wire.h>
#include "LiquidCrystal_PCF8574.h"
#include <ESP8266WiFi.h>        // https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>        // Lesen und schreiben von JSON Dateien
#include <FS.h>                 // SPIFFS Zugriff
#include <ESP8266WebServer.h>   // Unterstützung Webserver
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

// Definiere Konstanten
const char Version[6] = "2.02e";
#define PAUSE1SEC 1000
#define PAUSE2SEC 2000

extern "C"
{
#include "user_interface.h"
}

// Voreinstellungen
float setPressure = 2.0;    //  Vorgabe bei Neustart von 2,0 bar
float setCarbonation = 5.0; //  Vorgabe bei Neustart von 5,0 gr/L
int setMode = 0;            //  Startposition 0 = AUS , 1 = CO² , 2 = Druck, 3 = Zwangskarbonisierung

bool oldValve = false;      // Zwangskarbonisierung default Ventil geschlossen
bool setDEBUG = false;      // Debug Ausgaben serieller Monitor
bool startTEL = false;      // Debug Ausgaben an Telnet Client (putty)
bool startMDNS = false;     // mDNS Dienst
File fsUploadFile; // Datei Object

//Pinbelegung
const int PIN_PRESSURE = A0;        // Drucksensor
const int PIN_BUZZER = D0;          // Buzzer (wird nicht verwendet)
const int PIN_TEMP = D3;            // DS18B20
const int PIN_ENCODER_A = D5;       // CLK
const int PIN_ENCODER_B = D6;       // SW
const int PIN_ENCODER_BUTTON = D7;  // DT
const int PIN_VALVE = D8;           // Magnetventil

//Eulersche Zahl
const double E = exp(1);

//Klassen Initialisierungen
LiquidCrystal_PCF8574 lcd(0x27);    // LCD Display
OneWire oneWire(PIN_TEMP);          // DS18B20
DallasTemperature sensors(&oneWire);
OneButton button(PIN_ENCODER_BUTTON, true);          // Encoder drücken
RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B); // Encoder drehen
WiFiManager wifiManager;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdate; // Version mit SPIFFS Update
WiFiServer TelnetServer(23);        // Telnet Server (putty)
WiFiClient Telnet;
MDNSResponder mdns;

//Timer
os_timer_t TimerTemp;             // Timer Objekt Temperatur: Wiederhole Aufgaben und lasse den Wemos das Zeitintervall überwachen
os_timer_t TimerPressure;         // Zeitintervall Druck
bool TickTempOccured = false;     // Prüfe Zeitintervall Temperatur
bool TickPressureOccured = false; // Prüfe Zeitintervall Druck

//Variablen
float temperature;
float oldTemperature;
char sTemperature[5];
float voltage;
float offsetVoltage = 0.42;
double pressure;
double oldPressure;
int menuitem = 0;
int edititem = 0;
int page = 1;
int encoderOldPos;

boolean up = false;
boolean down = false;
boolean buttonPressed;

boolean reflashLCD = true;

String Menu1[4]; // Startseite
String Menu2[5]; // Modus
String Menu3[2]; // Kalibrierung
String Menu4[2]; // Einstellunen speichern

#define sizeOfModes 4
String modes[sizeOfModes] = {"Aus", "CO2 Spund", "Druck Spund", "CO2 Zwang"};
String modesWeb[sizeOfModes] = {"Aus", "Spunder CO2 Gehalt", "Spunder Druck", "CO2 Zwangskarbonisieren"};

char nameMDNS[16] = "spundomat"; // http://spundomat/index.html
