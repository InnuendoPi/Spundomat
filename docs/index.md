![Startseite](img/Spundomat01.jpg)

# Spundomat

**Was ist Spundomat?**

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

**Was bietet diese Firmware?**

Die Firmware bietet zwei Betriebsmodis:
1. Spunden (Druck ausgehend vom Keg)
2. Karbonisieren (CO2 eingehend in das Keg)

Zur Konfiguration und Verwendung kann ein Display mit Encoder (Dreh-Drück-Knopf) oder das Web Interface verwendet werden.

* Web Interface (WebIf) für die Konfiguration
* 2004A Display Integration
* Encoder Unterstützung
* Sensoren
  * Temperatursensoren DS18B20
  * Drucksensor
* Magnetventil ausgehend (Spunden)
* Magnetventil eingehend (Karbonisieren)
* Kalibrierung (offsetVolt)
* mDNS Support
* Backup / Restore der Konfiguration (config.json) über Dateiexplorer
* Firmware Update über WebIf (Firmware und SPIFFS)

![Einstellungen](img/Spundomat02.jpg)

**Voraussetzungen: (2020.01)**

* Arduino IDE 1.8.10
* Optional Microsoft VSCode + Arduino + ESP8266FS
* ESP8266 by ESP8266 Community version 2.6.3
* Folgende Bibliotheken müssen über die Arduino IDE hinzugefügt werden:
    * ArduinoJSON by Benoit Blanchon Version 6.13.0 
    * DallasTemperature by Miles Burton Version 3.8.0
    * OneWire By Jim Studt Version 2.3.5
    * WiFiManager by tzapu Version 0.15.0
    * EEPROM.h
    * RotaryEncoder.h
    * OneButton.h
    * Wire.h
    * LiquidCrystal_PCF8574.h
    * FS.h

# Hardware: die Platine zum Projekt

![Platine-vo](img/Layout01.png)

![Platine-hi](img/Layout02.png)

Beschreibung:

Stückliste:

Einrichtung:
- Trimmer auf 70.5kOhm einstellen
- MP1584En ausgehend auf 5V einstellen

**Hardware: Anschlussmöglichkeiten Drucksensor und Magnetventil

1. Möglichkeit: Spunder

![Spunder](img/Spunder.jpg)

2. Möglichkeit: Zwangskarbonisierung

![Karbonisieren](img/Zwangskarbonisieren.jpg)


# Installation

**Installation ohne den Quellcode zu compilieren**

Mit Hilfe von esptool.exe (https://github.com/igrr/esptool-ck/releases ) aus dem Ordner tools kann die Firmware auf das ESP Modul geladen werden. Das ESPTool ist für verschiedene Betriebssysteme verfügbar.
ESPtool-ck Copyright (C) 2014 Christian Klippel ck@atelier-klippel.de. This code is licensed under GPL v2.

Unter Win10 wird der USB Treiber CH341SER benötigt: http://www.wch.cn/download/CH341SER_ZIP.html

Beispiel für ein ESP8266 Modul vom Typ Wemos D1 mini mit 4MB Flash verbunden mit COM3

	* Download von github entpacken (komplett)

    * Im Ordner tools das Archiv tools.zip entpacken. Enthalten sind das esptool und das Skript Flashen.cmd

    * Eingabeaufforderung öffnen

	* in den Order .../Spundomat/tools wechseln und das Skript Flashen.cmd ausführen
    Das Skript löscht alle Daten aus dem Speicher und spielt die Firmware und das Filesystem SPIFFS auf.

    alternativ manuell mit esptool:

		* Wemos D1 mini löschen:
        esptool.exe -cp COM3 -cd nodemcu -ce 
        * Flashen:
        esptool.exe -cp COM3 -cd nodemcu -ca 0x000000 -cf ..\build\Spundomat.ino.bin -ca 0x200000 -cf ..\build\Spundomat.spiffs.bin

	    * Das ESP8266 Modul resetten

	    * Das ESP8266 Modul startet anschließend im Access Point Modus mit der IP Adresse 192.168.4.1

    	* Das ESP8266 Modul über einen Webbrowser mit dem WLAN verbinden

        * Anschließend ist der Spundomat erreichbar über http://Spundomat

* Updates
	Updates (firmware und SPIFFS) können über das WebIf geladen werden: http://Spundomat/update

* Backup and restore der Konfiguration
    Der FileBrowser ist erreichbar über <IP Adresse ESP8266>/edit download oder upload config.json 

* mDNS: Ein mDNS Name kann anstelle der IP Adresse vom ESP8266 im Webbrowser verwendet werden (http://<mDNSname>). Der Name ist frei wählbar.

Der Quellcode basiert ursprünglich auf einem snipet von Janick 2017
