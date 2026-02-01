/*
Name:		Spundomat.ino
Erstellt:	2022
Author:	    Innuendo


Spunden Zitat aus https://hobbybrauer.de/forum/wiki/doku.php/lagern:spunden
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
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include <FS.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Math.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>
#include <stdarg.h>
#include "InnuTicker.h"
#include <InfluxDbClient.h>
#include "edit_htm.h"
#include "index_htm.h"
#include "ablaufplan_htm.h"
#include "splan_htm.h"
#include "Spundomat.h"

#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif

// Definiere Konstanten
#define Version "2.85"

LiquidCrystal_PCF8574 lcd(0x27); // LCD Display

OneWire oneWire(PIN_TEMP); // DS18B20
DallasTemperature sensors(&oneWire);
OneButton button(PIN_ENCODER_BUTTON, true);          // Encoder drücken
RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B); // Encoder drehen
WiFiManager wifiManager;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdate; // DateiUpdate

// Zeitserver Einstellungen
WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

// Zeitserver Einstellungen
char ntpServer[maxNTPSign] = NTP_ADDRESS;
char ntpZone[maxNTPSign] = NTP_ZONE;
struct tm timeinfo;
char zeit[9] = "00:00:00";

// Influx Server (optional)
InfluxDBClient dbClient;
uint8_t startDB = 0;      // Visualisierung aktiviert 0:= Aus 1:= InfluxDB1 2:= InfluxDB2
bool startVis = false;     // Visualisierung gestartet
bool connected2DB = false; // Datenbankverbindung hergestellt
String visState = "0";     // Status Schreiben in Datenbank

String dbServer; // "http://192.168.100.31:8086"; // InfluxDB Server IP
String dbUser;    // User und Bucket
String dbPass;    // Pass und Token
String dbDatabase = "spundomat";  // Database und Org
String dbVisTag;
unsigned long upInflux = DB_UPDATE;

// Definiere Ticker Objekte
InnuTicker TickerTemp;
InnuTicker TickerPressure;
InnuTicker TickerInfluxDB;
InnuTicker TickerDisplay;
InnuTicker TickerWLAN;
InnuTicker TickerDisplayTimer;
InnuTicker TickerTime;
InnuTicker TickerCon;

// Voreinstellungen
float setPressure = DEF_PRESSURE; // Vorgabe bei Neustart von 2,0 bar
float setCarbonation = DEF_CARB;  // Vorgabe bei Neustart von 5,0 gr/L
float dichtPressure = 0.0;        // Überprüfung Dichtheit
uint8_t setMode = AUS;                // Startposition 0 = AUS
uint8_t newMode = AUS;
bool buzzer = false;    // Piezo Buzzer
bool startMDNS = true;  // mDNS Dienst
bool testModus = false; // testModus - ignorieren!
bool devBranch = false; // Check out development branch
bool startMV1 = false;  // Aktiviere MV1 an D8
bool startMV2 = false;  // Aktiviere MV2 an D0
bool alertState = false;
float ergDichtheit = -127.0;

// Deklariere Variablen
float temperature;
float oldTemperature = 0.0;
float alarmTemperature = 0.0;
float voltage;
float pressure = 0.0;
float oldPressDisp = 0.0;
float displayPressure = 0.0;
int encoderOldPos;
float targetTemp;
float DEF_TARGET_TEMP;
bool checkTemp;
int controller;
unsigned long lastTimeSteuerung;

typedef struct
{
    float offset0 = 0.0;  // Standard Vadc bei 0bar an A0
    float offset2 = 0.0;  // Vadc bei 2bar an A0
} PSettings;
PSettings psettings;

boolean up = false;
boolean down = false;
boolean buttonPressed;
long mv1Open = DEFAULT_OPEN;   // Default Öffne Magnetventil 1 in ms (min 20ms)
long mv1Close = DEFAULT_CLOSE; // Default Schließe Magnetventil 1 in ms (min 20ms)
long mv2Open = DEFAULT_OPEN;   // Default Öffne Magnetventil 2 in ms (min 20ms)
long mv2Close = DEFAULT_CLOSE; // Default Schließe Magnetventil 2 in ms (min 20ms)
float senOffset = 0.0;
int upTemp = TEMPERATUR_UPDATE;   // Default Update temperatur
int upPressure = PRESSURE_UPDATE; // Default Update Drucksensor

int menuitem = 0; // Display
int edititem = 0; // Display
int page = 1;     // Display
boolean reflashLCD = true; // LCD Update erforderlich nach Änderung durch Encoder 
String Menu1[4]; // Startseite
String Menu2[5]; // Modus
String Menu3[2]; // Kalibrierung
String Menu4[2]; // Einstellunen speichern

// Display Timer
bool useBacklightTimer = true;
boolean DisplayIsOn = true;
boolean showDisplay = true;
int DisplayTimerTime = TIMEOUTDISPLAY;
const int LedBackLight_On = 255;
const int LedBackLight_Off = 0;

// FSBrowser
File fsUploadFile; // a File object to temporarily store the received file
enum
{
    MSG_OK,
    CUSTOM,
    NOT_FOUND,
    BAD_REQUEST,
    ERROR
};

String modes[sizeOfModes] = {"Aus", "Spundomat", "CO2 Spund", "Druck Spund", "CO2 Karb", "Druck Karb", "Plan 1", "Plan 2", "Plan 3", "Dichtheit"};                                                  // ModusNamen im Display
String modesWeb[sizeOfModes] = {"Aus", "Spundomat", "Spunden CO2 Gehalt", "Spunden Druck", "Karbonisieren CO2 Gehalt", "Karbonisieren Druck", "Plan 1", "Plan 2", "Plan 3", "Überprüfe Dichtheit"}; // Modus-Namen für WebIf

char nameMDNS[maxHostSign] = "spundomat"; // http://spundomat/index.html
char esp_clientid[maxHostSign]; // AP-Mode und Gerätename

// Verzögerung Spundomat
String einheit[anzAuswahl] = {"Minuten", "Stunden", "CO2-Gehalt gr/l"};
int setEinheit = 0;
float verzKombi = 0.0;
float minKarbonisierung = 0.0;
unsigned long verzKarbonisierung = 0;
unsigned long lastTimeSpundomat;

// Ablaufplan
File planfile;
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

struct Steuerplan 
{
    String Name;
    float Stemp;
    float Etemp;
    bool Ramp;
    unsigned long Dauer;
};
uint8_t counterCon = 0;
struct Steuerplan structOG[MAXFERM];
struct Steuerplan structUG[MAXFERM];

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
