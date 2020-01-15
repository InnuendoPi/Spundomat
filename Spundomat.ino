/*
Name:		Spundomat.ino
Erstellt:	2020
Author:	    Innuendo

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

Dieser Quellcode baut auf einem Snippet von Janick 2017 auf.
*/

#include <EEPROM.h>
#include <RotaryEncoder.h>
#include <OneButton.h>
#include <OneWire.h>           // OneWire Bus Kommunikation
#include <DallasTemperature.h> // Vereinfachte Benutzung der DS18B20 Sensoren
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>      // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>      // Lesen und schreiben von JSON Dateien
#include <FS.h>               // SPIFFS Zugriff
#include <ESP8266WebServer.h> // Unterstützung Webserver
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Math.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <NTPClient.h>
#include <stdarg.h>
#include <time.h>
#include <CertStoreBearSSL.h>
// BearSSL::CertStore certStore;

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif

extern "C"
{
#include "user_interface.h"
}

// Definiere Konstanten
const char Version[6] = "2.0";

#define PAUSE1SEC 1000
#define PAUSE2SEC 2000
#define PAUSE5SEC 5000
#define PAUSE200MS 200
#define PAUSE100MS 100
#define PAUSE10MS 10

// Definiere Pinbelegung
const int PIN_PRESSURE = A0;       // Drucksensor
const int PIN_BUZZER = D4;         // Buzzer (wird nicht verwendet)
const int PIN_TEMP = D3;           // DS18B20
const int PIN_ENCODER_A = D5;      // CLK
const int PIN_ENCODER_B = D6;      // SW
const int PIN_ENCODER_BUTTON = D7; // DT
const int PIN_MV1 = D8;            // Magnetventil ausgehend MV1 (Spunder)
const int PIN_MV2 = D8;            //D0    // Magnetventil eingehend MV2 (Karbonisierer)

// Eulersche Zahl
const double E = exp(1);

// Voreinstellungen
float setPressure = 2.0;    //  Vorgabe bei Neustart von 2,0 bar
float setCarbonation = 5.0; //  Vorgabe bei Neustart von 5,0 gr/L
int setMode = 0;            //  Startposition 0 = AUS , 1 = CO² , 2 = Druck, 3 = Karbonisieren

bool startMDNS = false;   // mDNS Dienst
bool testModus = false;   // testModus
bool startMV1 = false;    // Aktiviere MV1
bool startMV2 = false;    // Aktiviere MV2
bool startBuzzer = false; // Aktiviere Buzzer

// Klassen Initialisierungen
LiquidCrystal_PCF8574 lcd(0x27); // LCD Display
OneWire oneWire(PIN_TEMP);       // DS18B20
DallasTemperature sensors(&oneWire);
OneButton button(PIN_ENCODER_BUTTON, true);          // Encoder drücken
RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B); // Encoder drehen
WiFiManager wifiManager;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdate; // Version mit SPIFFS Update
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

// Definiere Timer
os_timer_t TimerTemp;               // Timer Objekt Temperatur: Wiederhole Aufgaben und lasse den Wemos das Zeitintervall überwachen
os_timer_t TimerPressure;           // Timer Objekt Druck

bool TickTempOccured = false;       // Prüfe Zeitintervall Temperatur
bool TickPressureOccured = false;   // Prüfe Zeitintervall Druck

// Deklariere Variablen
float temperature;
float oldTemperature;
char sTemperature[5];
float voltage;
float offsetVoltage = 0.42;
double pressure;
double oldPressure;
int encoderOldPos;
int sensorValueTest;

boolean up = false;
boolean down = false;
boolean buttonPressed;

int menuitem = 0;       // Display
int edititem = 0;       // Display
int page = 1;           // Display
boolean reflashLCD = true;
String Menu1[4]; // Startseite
String Menu2[5]; // Modus
String Menu3[2]; // Kalibrierung
String Menu4[2]; // Einstellunen speichern

File fsUploadFile; // Datei Object
#define sizeOfModes 4
String modes[sizeOfModes] = {"Aus", "CO2 Spund", "Druck Spund", "Karb"};                            // ModusNamen im Display
String modesWeb[sizeOfModes] = {"Aus", "Spundomat CO2 Gehalt", "Spundomat Druck", "Karbonisieren"}; // Modus-Namen für WebIf

char nameMDNS[16] = "spundomat"; // http://spundomat/index.html
IPAddress aktIP;    // Aktuelle IP Adresse
String aktWLAN;     // SSID WLAN im STA Modus

// Callback für Wemos im Access Point Modus
void configModeCallback(WiFiManager *myWiFiManager)
{
    Serial.print("*** SYSINFO: Spundomat im AP Mode ");
    Serial.println(WiFi.softAPIP());
    Serial.print("*** SYSINFO: Erstelle Konfigurationsportal ");
    Serial.println(myWiFiManager->getConfigPortalSSID());
    Wire.begin();
    Wire.beginTransmission(0x27);

    lcd.begin(16, 2);
    lcd.setBacklight(255);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" Spundomat V");
    lcd.print(Version);
    lcd.setCursor(0, 2);
    lcd.print("AP-Mode: ");
    lcd.print(WiFi.softAPIP());
}
