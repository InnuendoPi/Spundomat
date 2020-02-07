# Spundomat

![ov1](/Info/Spundomat01.jpg)

Projekt im Beta Status

## Changelog

Version 2.0b3

- Added:    Ablaufpläne.txt mit Testwerten gefüllt
- Added:    Alarmfunktion
- Added:    Überprüfung Eingabe Web Interface Kalibrierung (0.0 bis 3.5)
- Added:    Überprüfung Eingabe Web Interface CO2 Gehalt (0.0 bis 8.0)
- Fixed:    LCD Anzeige Karbonisieren und Spunden CO2 Gehalt statt Druck

Version 2.0b2

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

Version 2.0b1

- Add:      mDNS Name in der LCD Anzeige
- Add:      Eingabe mDNS Name: Leerzeichen werden automatisch entfernt
- Reworked: Kombi-Modus als serielle Abfolge MVV1, MV2
- Add:      Verzögerung Kombi-Modus (Minuten/Stunden)

Version 2.0

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

Dokumentation unter: <https://innuendopi.github.io/Spundomat/>
