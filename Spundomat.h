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
#define ZUSATZALARM 1200000 // 20min
#define TIMEOUTDISPLAY 10000        // Display schaltet sich nach der Zeit dunkel
#define sizeOfModes 10
#define maxHostSign 16
#define anzAuswahl 3
#define maxSchritte 20
#define RESOLUTION 12      // 12bit resolution == 750ms update rate
#define EEPROM_SIZE 8

// Zeitserver Einstellungen
#define TIME_UPDATE 30000
#define maxNTPSign 30
#define NTP_ADDRESS "europe.pool.ntp.org"
#define NTP_ZONE "CET-1CEST,M3.5.0,M10.5.0/3"
#define SCHALTJAHR(Y) (((1970 + (Y)) > 0) && !((1970 + (Y)) % 4) && (((1970 + (Y)) % 100) || !((1970 + (Y)) % 400)))

#define TEXT_PLAIN "text/plain"
#define FS_INIT_ERROR "FS INIT ERROR"
#define FILE_NOT_FOUND "FileNotFound"
#define TRENNLINIE "-----------------------"


#define UPDATESYS "/updateSys.txt"
#define LOGUPDATESYS "/updateSys.log"
#define UPDATETOOLS "/updateTools.txt"
#define LOGUPDATETOOLS "/updateTools.log"
#define UPDATELOG "/webUpdateLog.txt"
#define DEVBRANCH "/dev.txt"
#define CERT "/ce.rts"
#define CONFIG "/config.txt"
// #define CONFIGSIZE 2048
#define ABLAUFPLAN1 "/ablaufplan1.json"
#define ABLAUFPLAN2 "/ablaufplan2.json"
#define ABLAUFPLAN3 "/ablaufplan3.json"

// Definiere Pinbelegung
#define PIN_PRESSURE A0       // Drucksensor
#define PIN_BUZZER D7         // Buzzer od. Venitalor
#define PIN_TEMP D3           // DS18B20
#define PIN_ENCODER_A D6      // CLK Out A
#define PIN_ENCODER_B D5      // DT Out B
#define PIN_ENCODER_BUTTON D4 // SW Button
#define PIN_MV1 D8            // Magnetventil ausgehend MV1 (Spunder)
#define PIN_MV2 D0            // Magnetventi2 eingehend MV2 (Karbonisieren)

// Eulersche Zahl
#define E exp(1)
// const uint8_t PIN_PRESSURE = A0;       // Drucksensor
// const uint8_t PIN_BUZZER = D7;         // Buzzer od. Venitalor
// const uint8_t PIN_TEMP = D3;           // DS18B20
// const uint8_t PIN_ENCODER_A = D6;      // CLK Out A
// const uint8_t PIN_ENCODER_B = D5;      // DT Out B
// const uint8_t PIN_ENCODER_BUTTON = D4; // SW Button
// const uint8_t PIN_MV1 = D8;            // Magnetventil ausgehend MV1 (Spunder)
// const uint8_t PIN_MV2 = D0;            // Magnetventi2 eingehend MV2 (Karbonisieren)

// const double E = exp(1);
