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
#include "InnuTicker.h"
#include <CertStoreBearSSL.h>

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif

extern "C"
{
#include "user_interface.h"
}

// Definiere Konstanten
const char Version[6] = "2.0b5";

#define PAUSE1SEC 1000
#define PAUSE2SEC 2000
#define PAUSE5SEC 5000
#define PAUSE200MS 200
#define PAUSE100MS 100
#define PAUSE10MS 10
#define DELTA 0.01
#define ENCODER_UPDATE 100
#define BUTTON_UPDATE 100
#define TEMPERATUR_UPDATE 30000
#define PRESSURE_UPDATE 1000
#define SPUNDOMAT_UPDATE 1000
#define TCP_UPDATE 60000
#define AUS 0
#define SPUNDOMAT 1
#define SPUNDEN_CO2 2
#define SPUNDEN_DRUCK 3
#define KARBONISIEREN_CO2 4
#define KARBONISIEREN_DRUCK 5
#define PLAN1 6
#define PLAN2 7
#define PLAN3 8
#define DEFAULT_OPEN 300
#define DEFAULT_CLOSE 2000
#define ALARM_ON 1
#define ALARM_OFF 2
#define ALARM_OK 3
#define ALARM_ERROR 4
#define ALARM_PANIC 5


// Definiere Pinbelegung
const int PIN_PRESSURE = A0;       // Drucksensor

// Neue PIN Belegung 20200212
const int PIN_BUZZER = D6;         // Buzzer
const int PIN_TEMP = D3;           // DS18B20
const int PIN_ENCODER_A = D4;      // CLK Out A
const int PIN_ENCODER_B = D5;      // DT Out B
const int PIN_ENCODER_BUTTON = D0; // SW Button
const int PIN_MV1 = D8;            // Magnetventil ausgehend MV1 (Spunder)
const int PIN_MV2 = D7;            // Magnetventi2 eingehend MV2 (Karbonisieren)

// Alte PIN Belegung
// const int PIN_BUZZER = D4;         // Buzzer
// const int PIN_TEMP = D3;           // DS18B20
// const int PIN_ENCODER_A = D6;      // CLK Out A
// const int PIN_ENCODER_B = D5;      // DT Out B
// const int PIN_ENCODER_BUTTON = D7; // SW Button
// const int PIN_MV1 = D8;            // Magnetventil ausgehend MV1 (Spunder)
// const int PIN_MV2 = D0;            // Magnetventil eingehend MV2 (Karbonisierer)

// Eulersche Zahl
const double E = exp(1);

// Voreinstellungen
float setPressure = 2.0;    //  Vorgabe bei Neustart von 2,0 bar
float setCarbonation = 5.0; //  Vorgabe bei Neustart von 5,0 gr/L
int setMode = 0;            //  Startposition 0 = AUS , 1 = CO² , 2 = Druck, 3 = Karbonisieren

bool startMDNS = true;    // mDNS Dienst
bool testModus = false;   // testModus - ignorieren!
bool startMV1 = false;    // Aktiviere MV1 an D8
bool startMV2 = false;    // Aktiviere MV2 an D0
bool startBuzzer = false; // Aktiviere Buzzer an D4

// Klassen Initialisierungen
LiquidCrystal_PCF8574 lcd(0x27); // LCD Display
OneWire oneWire(PIN_TEMP);       // DS18B20
DallasTemperature sensors(&oneWire);
OneButton button(PIN_ENCODER_BUTTON, true);          // Encoder drücken
RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B); // Encoder drehen
WiFiManager wifiManager;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdate; // Version mit SPIFFS Update
// Zeitserver Einstellungen
#define NTP_OFFSET 60 * 60                // NTP in Sekunden
#define NTP_INTERVAL 60 * 60 * 1000       // NTP in ms
#define NTP_ADDRESS "europe.pool.ntp.org" // NTP change this to whatever pool is closest (see ntp.org)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);
WiFiClient tcpClient;

// Definiere Ticker Objekte
InnuTicker TickerTemp;
InnuTicker TickerPressure;
InnuTicker TickerEncoder;
InnuTicker TickerButton;
InnuTicker TickerSpundomat;

// Deklariere Variablen
float temperature;
float oldTemperature;
char sTemperature[5];
float voltage;
//float offsetVoltage = 0.42;
float offset0 = 0.0; // Standard Vadc bei 0bar an A0
float offset2 = 0.0; // Vadc bei 2bar an A0
float pressureOffset2 = 2.0;
float pressure = 0.0;
float oldPressDisp = 0.0;
float displayPressure = 0.0;
int encoderOldPos;

boolean up = false;
boolean down = false;
boolean buttonPressed;
long mv1Open = DEFAULT_OPEN;      // Default Öffne Magnetventil 1 in ms (min 20ms)
long mv1Close = DEFAULT_CLOSE;    // Default Schließe Magnetventil 1 in ms (min 20ms)
long mv2Open = DEFAULT_OPEN;      // Default Öffne Magnetventil 2 in ms (min 20ms)
long mv2Close = DEFAULT_CLOSE;    // Default Schließe Magnetventil 2 in ms (min 20ms)
int upTemp = TEMPERATUR_UPDATE;   // Default Update temperatur
int upPressure = PRESSURE_UPDATE; // Default Update Drucksensor

int menuitem = 0; // Display
int edititem = 0; // Display
int page = 1;     // Display
boolean reflashLCD = true;
String Menu1[4]; // Startseite
String Menu2[5]; // Modus
String Menu3[2]; // Kalibrierung
String Menu4[2]; // Einstellunen speichern

File fsUploadFile; // Datei Object
#define sizeOfModes 9
String modes[sizeOfModes] = {"Aus", "Spundomat", "CO2 Spund", "Druck Spund", "CO2 Karb", "Druck Karb", "PLAN 1", "Plan 2", "Plan 3"};                            // ModusNamen im Display
String modesWeb[sizeOfModes] = {"Aus", "Spundomat", "Spunden CO2 Gehalt", "Spunden Druck", "Karbonisieren CO2 Gehalt", "Karbonisieren Druck", "Plan 1", "Plan 2", "Plan 3"}; // Modus-Namen für WebIf
char nameMDNS[16] = "spundomat";                                                                                                  // http://spundomat/index.html

// TCP Server (optional)
int tcpPort = 9501; // TCP server Port
char tcpHost[16] = "192.168.100.30";   // TCP server IP

// Verzögerung Spundomat
#define anzAuswahl 3
String einheit[anzAuswahl] = {"Minuten", "Stunden", "CO2-Gehalt gr/l"};
int setEinheit = 0;
float verzKombi = 0.0;
float minKarbonisierung = 0.0;
unsigned long verzKarbonisierung = 0;
unsigned long prevMillis;

// Ablaufplan
#define maxSchritte 20
File file;      // Datei Objekt ablaufplan.txt
struct Ablaufplan
{
    float zieldruckMV1;
    long intervallMV1Open;
    long intervallMV1Close;
    float zieldruckMV2;
    long intervallMV2Open;
    long intervallMV2Close;
};
struct Ablaufplan structPlan[maxSchritte];
int counterPlan = 0; // Aktueller Schritt im Ablaufplan
bool stepA = false;  // Step MV1 je Schritt
bool stepB = false;  // Step MV2 je Schritt

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
