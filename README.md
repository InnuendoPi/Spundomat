# Spundomat

![ov1](/docs/img/Spundomat01.jpg)

Dokumentation unter: <https://innuendopi.github.io/Spundomat/>

## Infos zum Quellcode

Bibliotheken: (Stand ab Version 2.83, 12.2023)

- ESP8266 3.1.2 (LittleFS)
- Arduino IDE 1.8.19
- Visual Code + modified ESP8266FS Plugin (VSCode 1.85 Arduino 0.6)
- ArduinoJSON 6.21
- InfluxDB 3.13
- WiFiManager 2.0
- LiquidCrystal_PCF8574 2.2
- RotaryEncoder 1.5.3
- InnuTicker
- InnuFramework

Flash size 4MB (FS:2MB OTA:~1019kB)

## Changelog

Version 2.83e

- Neu:          Display Timeout (User Bierdrohne)
- Fix:          JSON Größe misc
- Fix:          JSON Influx User Password an WebIf fehlerhaft
- Geändert:     Parametertyp Influx User/Bucket und Password/Token
- Fix:          Visualisierung

Version 2.82

- Fix:          WebUpdate

Version 2.81

- Geändert:     WebUpdate Standardverbindung SSL
- Geändert:     Speicherverbrauch WebUpdate optimiert
- Geändert:     DateiUpdate
- Neu:          Log Datei WebUpdate (webUpdateLog.txt)

Version 2.80

- Fix:          github MFLN Verprobung WebUpdate
- Fix:          WebUpdate
- Erneuert:     Zertifikate SSL

Version 2.79 Release

- Update:     InnuFramework

Version 2.78

- Fixed:      Arduino ESP8266 core: Fix for occasional timeout issues #8944 (net error timeout - WebIf Fehler)
- Fixed:      Arduino ESP8266 core: Fix for dangerous relocation: j: cannot encode #8925
- Changed:    Arduino ESP8266 core: Add support WiFiClientSecure TCP KeepAlive #8940
- Changed:    Cache control WebIf (no-cache, expires 0)
- Update:     InnuFramework

Version 2.77

- Update:   ESP8266 cores, Arduino Bibliotheken
- Update:   JQuery 3.7.0
- Fixed:    cache handling
- Fixed:    page load unknown length
- Fixed:    Fehler gpio
- Changed:  GPIO Modus (Piezo, Ventilator) ersetzt durch Pizeo Buzzer ein/aus
- Removed:  alte goto sub statements

Version 2.76

- Optimize: CleanUp, Framework
- Fixed:    Fehler speichern Ablaufplan

Version 2.75

- Optimize: Framework: WLAN reconnect, readConfig

Version 2.74

- Fixed:    Fehler WebIf Visualisierung behoben
- Update:   ESP8266 cores
- Update:   bootstrap 4.6.2
- Optimize: Konfiguration Backup und Restore
- Added:    DevBranch
- Update:   ESP8266 Arduino 3.1.2
- Optimize: Web Interface
- Optimize: Framework
- Update:   VSCode 1.76
