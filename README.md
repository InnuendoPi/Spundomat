# Spundomat

![ov1](/Info/Spundomat01.jpg)

Dokumentation unter: <https://innuendopi.github.io/Spundomat/>

## Update Hinweis Version 2.50

Beim Update auf Version 2.5 von 2.30 oder älter muss das Dateisystem ersetzt werden.
Die Firmware und das Filesystem LittleFS müssen neu geflasht werden. Ein WebUpdate von 2.3x auf 2.50 oder neuer ist nicht möglich.

## Changelog

Version 2.50

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
- Cleanup:  2.0
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

Bibliotheken: (Stand ab Version 2.50, 02.2021)

- ESP8266 2.7.4 (LittleFS)
- Arduino IDE 1.8.13
- Visual Code 1.51.2 mit modifiziertem ESP8266FS Plugin für LittleFS
- ArduinoJSON 6.17
- InfluxDB 3.6
- WiFiManager 2.0
- MH-Z19 1.5

Flash size 4MB (FS:2MB OTA:~1019kB)
SSL support all SSL ciphers (most comp)
Exceptions Lagacy
IwIP variant v2 lower mem

Für Debug Ausgaben muss der Debug Port auf Serial eingestellt werden. Für spezielle Debug Ausgaben entsprechend den Debug Level einstellen (default none).
