# Spundomat

![ov1](/Info/Spundomat01.jpg)

Dokumentation unter: <https://innuendopi.github.io/Spundomat/>

## Infos zum Quellcode

Bibliotheken: (Stand ab Version 2.66, 08.2022)

- ESP8266 3.0.2 (LittleFS)
- Arduino IDE 1.8.19
- Visual Code + modified ESP8266FS Plugin (VSCode 1.70 Arduino 4.12)
- ArduinoJSON 6.19
- InfluxDB 3.12
- WiFiManager 2.0
- LiquidCrystal_PCF8574 2.0
- RotaryEncoder 1.5.3

Flash size 4MB (FS:2MB OTA:~1019kB)
SSL support all SSL ciphers (most comp)
Exceptions disabled
IwIP variant v2 lower mem

Debug Ausgabe:
Für Debug Ausgaben muss der Debug Port auf Serial eingestellt werden. Für spezielle Debug Ausgaben entsprechend den Debug Level einstellen (default none).

## Update Hinweis Version 2.5x oder neuer

Ein WebUpdate von Version 2.3x auf 2.50 oder neuer ist nicht möglich. Die Firmware und das Filesystem LittleFS müssen neu geflasht werden.

## Changelog

Version 2.65

- Removed:  Quellcode CO2 Sensor
- Removed:  Quellcode Gärsteuerung
- Fixed:    Reset im Testmodus
- Update:   VSCode 1.69.2
- Update:   Bibliotheken aktualisiert
- Added:    Temperatursensor Offset
- Fixed:    WebUpdate erweitert
- Update:   ESP8266 3.0.2
- Update:   Bibliotheken aktualisiert
- Update:   VSCode 1.67.2
- Update:   Arduino plugin 0.4.12
- Removed:  CO2 Sensor
- Removed:  Gärsteuerung
- Update:   ESP8266 3.0.1
- Update:   ESP8266 3.0.0
- Update:   Bibliotheken aktualisiert
- Fixed:    Gärsteuerung
- Fixed:    Fehler im Parsing Ablaufplan und Steuerplan behoben
- Fixed:    Minor fixes WebIf
- Downgrade Lib LiquidCrystal_PCF8574 1.2
- Fixed:    LCD Anzeige (QuickFix Downgrade LiquidCrystal_PCF8574 1.3)
- Changed:  Dateisystem LittleFS
- Optimize: Web Interface
- Optimize: Speicherverbrauch
- Update:   Bootstrap und JQuery
- Update:   Bibliotheken
- Fixed:    Start Grafana/InfluxDB (validateConnection)
- Fixed:    Update Grafana/InfluxDB (validateConnection)
- Added:    Steuerpläne für eine Gärführung
- Added:    einfache Gärsteuerung (Kühlung/Heizung) implementiert
- Changed:  Konfiguration Visualisierung wird nun gespeichert (bierdrohne)
- Changed:  Visualisierung persistent (bierdrohne)
- Added:    MH-Z19b CO2 Sensor
- Changed:  GPIO D7 Optionen: Aus, Piezo Buzzer oder Ventilator (an Relais/SSR)
- Fixed:    WLAN reconnect
- Update:   ESP8266 2.7.4 (weiterhin SPIFFS)
- Update:   Bibliotheken aktualisiert
- Fixed:    Umstellung Sommer/Winterzeit
- Added:    WebIf, Abfrage Kalibrieren Start-Abbruch
- Reworked: WebIf, Menü Einstellungen überarbeitet, Anzeige Volt entfernt
- Update:   Libs InfluxDB, ArduinoJson
- Update:   Influx CheckDBConnect: bei einem Verbindungsfehler wird der Task Vis gestoppt
- Update:   LCD Display
- Added:    Automatische Reduzierung der Intervalle Öffne MVs im Modus Spundomat
- Reworked: Update LCD Display
- Reworked: LCD Display 2 Nachkommastellen CO2 Gehalt
- Added:    Erweiterung der Ticker lib updateNow
- Fixed:    Typo Ticker InfluxDB
- Update:   Gerber Dateien aktualisiert
- Update:   Gehäusedateien 3D aktualisiert
- Fixed:    Encoder reagierte im Modus Dichtheit nicht während der Wartezeit
- Fixed:    Anzeigefehler LCD behoben, wenn keine Kalibrierung durchgeführt wurde
- Fixed:    Anzeigefehler behoben
- Added:    Timer Information im Modus Spundomat und Dichtheit
- Fixed:    kleine Fehlerkorrektur
- Reworked: Modus Überprüfe Dichtheit
- Update:   Gerber Dateien aktualisiert
- Added:    Neuer Modus Überprüfe Dichtheit
- Added:    nach einem WebUpdate erscheint eine Info
- Added:    Bei einem Fehler Visualisierung wird im Dashboard nun der Fehler angezeigt
- Fixed:    Fehler Anzeige Ziel CO2-Gehalt mit 5 Nachkommastellen behoben
- Added:    bei einem Sensorfehler Temperatur wird nun ein Alarm ausgegeben und der Spundomat ausgeschaltet
- Fixed:    Fehler Anzeige Spundomat Verzögerung im Dashboard behoben
- Optimze:  Handling Magnetventile
- Reworked: WebIf (Ablaufpläne Deaktiviert, Pausiert, Aktiviert)
- Reworked: WebIf (Ablaufpläne Anzeige aktueller Schritt)
- Reworked: WebIf Dashboard Informationen
- Fixed:    Fehler Anzeige Druck WebIf während Ablaufpläne behoben
- Cleanup
