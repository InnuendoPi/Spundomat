/*
Name:		Spundomat.ino
Erstellt:	2022
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

Zum Anschließen, Umkonfigurieren, Carbonisieren und leichten Reinigen der CO2-Schläuche eignen 
sich pneumatische Schnellkupplungen hervorragend.
*/

#include <EEPROM.h>
#include <RotaryEncoder.h>
#include <OneButton.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "LittleFS.h"     // LittleFS Zugriff -> ESP 2.7.4
#include <ESP8266WebServer.h>
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
#include <InfluxDbClient.h>
#include "edit_htm.h"
#include <FS.h>

// CO2 Sensor
// #include <MHZ19.h>
// #include <SoftwareSerial.h> // Serielle Kommunikation mit CO2 Sensor

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf("%s ", timeClient.getFormattedTime().c_str());DEBUG_ESP_PORT.printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif

extern "C"
{
#include "user_interface.h"
}

// Definiere Konstanten
const char Version[7] = "2.64";

#define PAUSE1SEC 1000
#define PAUSE2SEC 2000
#define PAUSE5SEC 5000
#define PAUSE10SEC 10000
#define PAUSE200MS 200
#define PAUSE100MS 100
#define PAUSE10MS 10
#define PAUSE50MS 50
#define PAUSE2MIN 120000
#define PAUSE5MIN 300000
#define DELTA 0.05F
#define MVMODE1 1
#define TEMPERATUR_UPDATE 30000
#define PRESSURE_UPDATE 1000
#define DISPLAY_UPDATE 2000
#define DB_UPDATE 60000
#define WLAN_UPDATE 30000
#define CO2_UPDATE 60000
#define TARGET_UPDATE 60000
#define CON_UPDATE 60000

#define AUS 0
#define SPUNDOMAT 1
#define SPUNDEN_CO2 2
#define SPUNDEN_DRUCK 3
#define KARBONISIEREN_CO2 4
#define KARBONISIEREN_DRUCK 5
#define PLAN1 6
#define PLAN2 7
#define PLAN3 8
#define DICHTHEIT 9
#define DEFAULT_OPEN 200
#define DEFAULT_CLOSE 10000
#define ALARM_ON 1
#define ALARM_OFF 2
#define ALARM_OK 3
#define ALARM_ERROR 4
#define ALARM_WARNING 5
#define DEF_PRESSURE 2.0
#define DEF_CARB 4.5
#define PRESSURE_OFFSET0 0.0
#define PRESSURE_OFFSET2 2.0
#define ABWEICHUNG 0.50
#define ZUSATZALARM 1200000        // 20min

// Steuerung
// #define STEUERUNG 10
// #define CON1 11
// #define CON2 12
// #define DEF_TARGET_TEMP 18.0


// Definiere Pinbelegung
const int PIN_PRESSURE = A0;       // Drucksensor
const int PIN_BUZZER = D7;         // Buzzer od. Venitalor
const int PIN_TEMP = D3;           // DS18B20
const int PIN_ENCODER_A = D6;      // CLK Out A
const int PIN_ENCODER_B = D5;      // DT Out B
const int PIN_ENCODER_BUTTON = D4; // SW Button
const int PIN_MV1 = D8;            // Magnetventil ausgehend MV1 (Spunder)
const int PIN_MV2 = D0;            // Magnetventi2 eingehend MV2 (Karbonisieren)

// Eulersche Zahl
const double E = exp(1);

// Voreinstellungen
float setPressure = DEF_PRESSURE;   // Vorgabe bei Neustart von 2,0 bar
float setCarbonation = DEF_CARB;    // Vorgabe bei Neustart von 5,0 gr/L
float dichtPressure = 0.0;          // Überprüfung Dichtheit
int setMode = AUS;                  // Startposition 0 = AUS
int newMode = AUS;
bool startMDNS = true;    // mDNS Dienst
bool testModus = false;   // testModus - ignorieren!
bool startMV1 = false;    // Aktiviere MV1 an D8
bool startMV2 = false;    // Aktiviere MV2 an D0
bool alertState = false;
float ergDichtheit = -127.0;
bool wlanState = true;
// CO2 Sensor
// bool startCO2 = false;
// int wertCO2 = 0;                // CO2 Wert in ppm
// Klassen Initialisierungen
// MHZ19 myMHZ19;                      // CO2 Sensor MH-Z19b
// SoftwareSerial co2Serial(D2, D1);   // RX, TX Pins festlegen

LiquidCrystal_PCF8574 lcd(0x27); // LCD Display
#define RESOLUTION 12 // 12bit resolution == 750ms update rate
OneWire oneWire(PIN_TEMP);       // DS18B20
DallasTemperature sensors(&oneWire);
OneButton button(PIN_ENCODER_BUTTON, true);          // Encoder drücken
RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B); // Encoder drehen
WiFiManager wifiManager;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdate; // Version mit LittleFS Update
HTTPClient http;

// Zeitserver Einstellungen
#define NTP_OFFSET 60 * 60                // Offset Winterzeit in Sekunden
#define NTP_INTERVAL 60 * 60 * 1000       // Aktualisierung NTP in ms
#define NTP_ADDRESS "europe.pool.ntp.org" // NTP Server
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// Influx Server (optional)
InfluxDBClient dbClient;
bool startDB = false;       // Visualisierung aktiviert
bool startVis = false;      // Visualisierung gestartet
bool connected2DB = false;  // Datenbankverbindung hergestellt
String visState = "0";    // Status Schreiben in Datenbank
#define MAXHOSTSIGN 30
#define MAXDBSSIGN 15

char dbServer[MAXHOSTSIGN]; // "http://192.168.100.31:8086"; // InfluxDB Server IP
char dbUser[MAXDBSSIGN];
char dbPass[MAXDBSSIGN];
char dbDatabase[MAXDBSSIGN] = "spundomat";
char dbVisTag[MAXDBSSIGN];
unsigned long upInflux = DB_UPDATE;

// Definiere Ticker Objekte
InnuTicker TickerTemp;
InnuTicker TickerPressure;
InnuTicker TickerInfluxDB;
InnuTicker TickerDisplay;
InnuTicker TickerWLAN;

// InnuTicker TickerCO2;
// InnuTicker TickerSteuerung;
// InnuTicker TickerAlarmierung;
// InnuTicker TickerCon;

// Deklariere Variablen
float temperature;
float oldTemperature = 0.0;
float alarmTemperature = 0.0;
float voltage;
float offset0 = 0.0; // Standard Vadc bei 0bar an A0
float offset2 = 0.0; // Vadc bei 2bar an A0
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
float senOffset = 0.0;
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

// FSBrowser
File fsUploadFile; // a File object to temporarily store the received file
enum { MSG_OK, CUSTOM, NOT_FOUND, BAD_REQUEST, ERROR };
#define TEXT_PLAIN "text/plain"
#define FS_INIT_ERROR "FS INIT ERROR"
#define FILE_NOT_FOUND "FileNotFound"


// Steuerung
// #define sizeOfModes 13
// String modes[sizeOfModes] = {"Aus", "Spundomat", "CO2 Spund", "Druck Spund", "CO2 Karb", "Druck Karb", "PLAN_1", "Plan_2", "Plan_3", "Dichtheit", "Gaersteuerung", "Steuerung OG", "Steuerung UG"};                            // ModusNamen im Display
// String modesWeb[sizeOfModes] = {"Aus", "Spundomat", "Spunden CO2 Gehalt", "Spunden Druck", "Karbonisieren CO2 Gehalt", "Karbonisieren Druck", "Plan_1", "Plan_2", "Plan_3", "Überprüfe Dichtheit", "Gärsteuerung", "Steuerung OG", "Steuerung UG"}; // Modus-Namen für WebIf
#define sizeOfModes 10
String modes[sizeOfModes] = {"Aus", "Spundomat", "CO2 Spund", "Druck Spund", "CO2 Karb", "Druck Karb", "PLAN #1", "Plan #2", "Plan #3", "Dichtheit"};                            // ModusNamen im Display
String modesWeb[sizeOfModes] = {"Aus", "Spundomat", "Spunden CO2 Gehalt", "Spunden Druck", "Karbonisieren CO2 Gehalt", "Karbonisieren Druck", "Plan #1", "Plan #2", "Plan #3", "Überprüfe Dichtheit"}; // Modus-Namen für WebIf
#define sizeOfModes 10
#define maxHostSign 16
char nameMDNS[maxHostSign] = "spundomat";    // http://spundomat/index.html

// Verzögerung Spundomat
#define anzAuswahl 3
String einheit[anzAuswahl] = {"Minuten", "Stunden", "CO2-Gehalt gr/l"};
int setEinheit = 0;
float verzKombi = 0.0;
float minKarbonisierung = 0.0;
unsigned long verzKarbonisierung = 0;
unsigned long lastTimeSpundomat;

// Modus GPIO D7
#define sizeOfGPIO 3
String modesGPIO[sizeOfGPIO] = {"Aus", "Piezo Buzzer", "Ventilator"};
int setGPIO = 0;

// Gärsteuerung
// float targetTemp = DEF_TARGET_TEMP;
// int upTarget = TARGET_UPDATE;
// int upCon = CON_UPDATE;
// int controller = 0;

// Ablaufplan
#define maxSchritte 20
String planResponse;
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

// Gärsteuerung
// Datei Objekt steuerplan.txt
// #define maxCon 5
// struct Steuerplan
// {
//     float Temperatur;
//     unsigned long Dauer;
// };
// struct Steuerplan structOG[maxCon];
// struct Steuerplan structUG[maxCon];
// int counterCon = 0; // Aktueller Schritt im Steuerplan
// bool checkTemp = false;
// String ogResponse, ugResponse;
// unsigned long lastTimeSteuerung;
// unsigned long lastTimePause = 0;

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
