# Spundomat

![ov1](/Info/Spundomat01.jpg)

Projekt Spundomat:

Firmware:   Beta

Platine:    Entwicklungsphase

Dokumentation unter: <https://innuendopi.github.io/Spundomat/>

Hinweis zum Quellcode:

- das Modul WebUpdate ist nicht im repository enthalten
- Debug Ausgaben werden über Debug Port aktiviert

## Changelog

Version 2.0F6

- Update:   Gerber Dateien
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

Version 2.0beta13

- Added:    Status (HIGH/LOW) Magnetventile hinzugefügt
- Changed:  Handling Ticker Drucksensor angepasst
- Fixed:    Fehler Modus Spundomat behoben. closeInterval wurde nicht korrekt verarbeitet
- Fixed:    Verarbeitung Ablaufpläne
- Changed:  2 Dezimalstellen für aktueller CO2-Gehalt und Druck
- Changed:  Hysterese auf 0.05bar reduziert
- Fixed:    TestModus überarbeitet (Debug)
- Fixed:    Fehler ABlauf Modus Spundomat behoben
- Fixed:    Fehler bei der Berechnung Verzögerung min CO2 behoben
- Fixed:    Modus Einstellung Encoder
- Fixed:    Fehler Ticker Spundomat behoben (ersetzt)
- Fixed:    Fehler Start Ablaufpläne über Encoder behoben
- Fixed:    Fehler Start Ticker Drucksensor behoben
- Removed:  Ticker Encoder
- Removed:  Ticker Button
- Cleanup:  Web Interface
- Reworked: Neuer Button Visualisierung Start/Stop
- Added:    Datenbank Tag Sud-ID für Visualisierung hinzugefügt
- Reworked: Updates
- Fixed:    LCD Anzeige -0.0bar
- Fixed:    Temperatur 85°C bei Start
- Changed:  2. Kalibrierung Speicherung im Eeprom
- Fixed:    Nachkommastellen aktueller CO2 Gehalt
- Fixed:    Speichern Modus
- Fixed:    Speichern Verzögerung
- Changed:  2. Kalibrierung nun fest auf 2bar (Konfig entfernt)
- Fixed:    NaN wenn 1. Kalibrierung nicht ausgeführt wurde
- Reworked: WebIf
- Fixed:    Korrektur Standardwerte wenn keine config.txt vorhanden ist
- Changed:  Pinbelegung
- Changed:  Alarme
- Fixed:    Update Log
- Fixed:    loadConfig korrigiert (JSON Hardware Block versehentlich entfernt)
- Fixed:    JSON doc size
- Changed:  Dashboard Datenbank Verbindung hinzugefügt
- Added:    Ticker Objekt für den Datenbankzugriff
- Fixed:    Reset to defaults beinhaltet nun auch lösche Kalibrierung aus Eeprom
- Removed:  TCPServer Unterstützung gelöscht. Das Projekt Tozzi Server wird vorerst nicht weiter verfolgt
- Changed:  PIN Mapping (Herberts Enocder li/re)
- Added:    InfluxDB für Grafana implementiert
- Changed:  DT und CLK getauscht
- Changed:  Der Betriebsmodus wird nun beim Neustart wieder hergestellt
- Fixed:    Kalibrierung Datentyp
- Changed:  Pinmap angepasst (Encoder, MV2 Piezo)
- Removed:  I/O Expander I2C PCF8574
- Test:     I/O Expander I2C PCF8574
- Added:    Ablaufpläne.txt mit Testwerten gefüllt
- Added:    Alarmfunktion
- Added:    Überprüfung Eingabe Web Interface Kalibrierung (0.0 bis 3.5)
- Added:    Überprüfung Eingabe Web Interface CO2 Gehalt (0.0 bis 8.0)
- Fixed:    LCD Anzeige Karbonisieren und Spunden CO2 Gehalt statt Druck
- Fixed:    Reflash LCD in Menüs Einstellungen
- Renamed:  Kombi-Modus in Spundomat Modus umbenannt
- Renamed:  Spundomat CO2 in Spunden CO2 umbenannt
- Renamed:  Spundomat Druck in Spunden Druck umbenannt
- Add:      Betriebsmodus Karbonisieren Druck hinzugefügrt
- Reorg:    Betriebmodis neu angeordnet
- Add:      Verzögerung Mindest-CO2-Gehalt im Spundomat Modus
- Add:      mDNS in StartDisplay LCD
- Add:      2-Punkte Kalibrierung - 0bar und 2bar (einstellbar)
- Fixed:    Überprüfung der Eingaben im Web Interface (int, float, char)
- Removed:  setPause in Ablaufpläne (millis2wait)
- Fixed:    Ticker ReadPressure wurde nicht autmatisch gestartet
- Fixed:    Überprüfung Eingabe mDNS Name war fehlerhaft
- Add:      mDNS Name in der LCD Anzeige
- Add:      Eingabe mDNS Name: Leerzeichen werden automatisch entfernt
- Reworked: Kombi-Modus als serielle Abfolge MVV1, MV2
- Add:      Verzögerung Kombi-Modus (Minuten/Stunden)
- Add:      Kombi-Modus
- Reworked  Ablaufpläne Speicherhandling verbessert
- Fixed:    Fehlerhafte Sensorauswertung in Ablaufplänen
- Reworked: LCD Display
- Fixed:    Kalibrierung
- Add:      Lösche Kalibrierung
- Add:      Ablaufpläne Plan1: Entlüften, Plan2: QuickCarbonation, Plan3: CO2-Wäsche (Test!)
- Fixed:    Abruf Drucksensor (Lese Daten nach Schließen)
- Fixed:    Anzeige LCD
- Fixed:    Fehler in der Überprüfung der Zeitintervalle 20ms bis 7200000 (2h) behoben
- Fixed:    Lese Temperatur bei Neustart war verzögert
- Changed:  Update Intervall Drucksensor
- Add:      Bibliothek InnuTicker erstellt
- Add:      Klasse Magnetventil erstellt
- Reworked: NTP, readTemperature, readPressure und Encoder als Ticker Objekte eingebunden
- Reworked: Magnetventile MV1 und MV2 als Instanzen mit variablen Intervallen open/close erstellt
- Fixed:    Magnetventile schließen zuverlässig bei Erreichung der Zielwerte
- Fixed:    Langsame/träge Encoder und WebIf Bedienung durch Ticker Objekte behoben
- Add:      Intervalle Öffne und Schließe Ventil konfigurierbar
- Add:      Intervall Lese Drucksensor und lese Temperatursensor konfigurierbar
- Removed:  Ursprüngliches handling releasePressure mit delays (Spunder) durch Ticker Objekte ersetzt
- New:      der Startmodus ist nun immer Aus. Der Modus wird gespeichert, aber bei Neustart nicht gesetzt
- New:      Überprüfung der Eingabe auf Int Werte (Intervalle Sensoren)
- New:      Überprüfe Bereich Eingabe (min 20ms bis max 7200000ms (2h))
- Add:      WebUpdate Firmware
- Add:      NTP Zeit
- Update:   ESP8266 V2.6.3
- Add:      Ersetze bei Zahleneingabe Komma durch Punkt(4,5 -> 4.5)
- Add:      Hardware kann konfiguriert werden (MV1, MV2 und Buzzer)
- Fixed:    Webserver Standard index.html
- Add:      Testmodus
- Fixed:    Typo index.html (offset)
- Add:      Display auf 4 Zeilen je 20 Zeichen
- Add:      Abfrage not a number bei Berechnung setCarbonation
- Add:      Abfrage not a number bei Berechnung Pressure
- Add:      Im AP Mode die Mindestsignalstärke eingefügt
- Add:      Timeout von 5 Minuten für den AP Mode gesetzt
- Fixed:    Fehler mDNS behoben
- Fixed:    Absturz im Startmenü behoben
- Reworked: Setup und loop überarbeitet
- Reworked: WiFiManager
- Reworked: Startdisplay LCD
- Cleanup:  Testcode entfernt
- Update:   Bibliotheken aktualisiert
- Add:      Dateisystem SPIFFS
- Add:      Konfigurationsdatei im JSON Format
- Add:      Webserver
- Add:      mDNS, Telnet
- Add:      ESP WebUpdate
- Add:      Modus Zwangskarbonisieren
- Add:      Anzeige IP und WLAN im Display
- Add:      Menü Speichern der Konfiguration im Display
- Removed:  Delay Befehle
