# Spundomat

![ov1](/docs/img/Spundomat01.jpg)

Dokumentation unter: <https://innuendopi.github.io/Spundomat/>

## Infos zum Quellcode

Bibliotheken: (Stand ab Version 2.77, 05.2023)

- ESP8266 3.1.2 (LittleFS)
- Arduino IDE 1.8.19
- Visual Code + modified ESP8266FS Plugin (VSCode 1.78 Arduino 0.6)
- ArduinoJSON 6.21
- InfluxDB 3.13
- WiFiManager 2.0
- LiquidCrystal_PCF8574 2.2
- RotaryEncoder 1.5.3
- InnuTicker

Flash size 4MB (FS:2MB OTA:~1019kB)

## Changelog

Version 2.77

- Update:   ESP8266 cores, Arduino Bibliotheken
- Update:   JQUery 3.7.0
- Fixed:    cache handling
- Fixed:    page load unknown length
- Fixed:    Fehler gpio
- Changed:  GPIO Modus (Piezo, Ventilator) ersetzt durch Pizeo Buzzer ein/aus

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
