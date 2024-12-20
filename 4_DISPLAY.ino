// LCD Ausgabe
void showLCD()
{
  reflashLCD = false;
  switch (page)
  {
  case 1: // Startseite
    Menu1[0] = "CO2:   ";
    Menu1[0] += ((int)(calcCarbonation(pressure, temperature) * 100)) / 100.0;
    Menu1[0] += "g/l ";
        
    if (setMode == AUS)
    {
      Menu1[3] = getDayOfWeek(timeinfo.tm_wday);
      Menu1[3] += " ";
      Menu1[3] += zeit;
      Menu1[3] += " ";
    }
    else if (setMode == SPUNDOMAT)
    {
      Menu1[3] = "Spundomat: ";
      Menu1[3] += setCarbonation;
      Menu1[3] += "g/l";
    }
    else if (setMode == SPUNDEN_CO2)
    {
      Menu1[3] = "Spunden: ";
      Menu1[3] += setCarbonation;
      Menu1[3] += "g/l";
    }
    else if (setMode == SPUNDEN_DRUCK)
    {
      Menu1[3] = "Spunden: ";
      Menu1[3] += setPressure;
      Menu1[3] += "bar";
    }
    else if (setMode == KARBONISIEREN_CO2)
    {
      Menu1[3] = "Karbonisiere:";
      Menu1[3] += setCarbonation;
      Menu1[3] += "g/l";
    }
    else if (setMode == KARBONISIEREN_DRUCK)
    {
      Menu1[3] = "Karbonisiere: ";
      Menu1[3] += setPressure;
      Menu1[3] += "b";
    }
    else if (setMode == PLAN1 || setMode == PLAN2 || setMode == PLAN3)
    {
      Menu1[3] = "Plan #";
      Menu1[3] += counterPlan + 1;
      if (!stepA)
      {
        Menu1[3] += " MV1 ";
      }
      else
      {
        Menu1[3] += " MV2 ";
      }
      Menu1[3] += displayPressure;
      Menu1[3] += "bar";
    }
    else if (setMode == DICHTHEIT)
    {
      Menu1[3] = "Pruefe Dicht: ";
      Menu1[3] += setPressure;
      Menu1[3] += "b";
    }
    // else if (setMode == STEUERUNG)
    // {
    //   Menu1[3] = "Zieltemp: ";
    //   Menu1[3] += targetTemp;
    //   Menu1[3] += "\337C";
    // }

    Menu1[1] = "Druck: ";
    // if (pressure == -1)
    //   Menu1[1] += "-       ";
    // else
    // {
    Menu1[1] += pressure;
    Menu1[1] += "bar ";
    // }

    Menu1[2] = "Temp:  ";
    Menu1[2] += ((int)(temperature * 10)) / 10.0;
    Menu1[2] += "\337C"; // °C
    // Navigation
    if (up)
    {
      menuitem++;
      if (menuitem > 1)
      {
        menuitem = 1;
      }
    }
    else if (down)
    {
      menuitem--;
      if (menuitem < 0)
      {
        menuitem = 0;
      }
    }

    if (buttonPressed)
    {
      menuitem = 0;
      page = 2;
      reflashLCD = true;
    }

    // Ausgabe
    if (menuitem == 0)
    {
      lcd.home();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(Menu1[menuitem]);
      lcd.setCursor(0, 1);
      lcd.print(Menu1[(menuitem + 1)]);
      lcd.setCursor(0, 2);
      lcd.print(Menu1[(menuitem + 2)]);
      lcd.setCursor(0, 3);
      lcd.print(Menu1[(menuitem + 3)]);
    }
    else
    {
      lcd.home();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Spundomat V ");
      lcd.setCursor(12, 0);
      lcd.print(Version);
      lcd.setCursor(0, 1);
      if (startMDNS)
      {
        lcd.print("mDNS");
        lcd.setCursor(5, 1);
        lcd.print(nameMDNS);
      }
      else
      {
        lcd.print("IP");
        lcd.setCursor(5, 1);
        lcd.print(WiFi.localIP().toString());
      }
      lcd.setCursor(0, 2);
      lcd.print("SSID");
      lcd.setCursor(5, 2);
      lcd.print(WiFi.SSID());
      lcd.setCursor(0, 3);
      lcd.print("RSSI");
      lcd.setCursor(5, 3);
      lcd.print(WiFi.RSSI());
      lcd.setCursor(0, 4);
    }
    break;

  case 2:                   // switch(page) Einstellungen
    Menu2[0] = "Zur\365ck"; // ü
    Menu2[1] = "Modus:";
    Menu2[1] += modes[newMode];
    Menu2[2] = "Soll: ";

    if (setMode == AUS || setMode == SPUNDOMAT || setMode == SPUNDEN_CO2 || setMode == KARBONISIEREN_CO2) // CO2
    {
      Menu2[2] += setCarbonation;
      Menu2[2] += " g/l";
    }
    else if (setMode == SPUNDEN_DRUCK || setMode == KARBONISIEREN_DRUCK) // Druck
    {
      Menu2[2] += setPressure;
      Menu2[2] += " bar";
    }
    // else if (setMode == STEUERUNG) // Druck
    // {
    //   Menu2[2] += targetTemp;
    //   Menu2[2] += "\337C";
    // }

    Menu2[3] = "Kalibrierung";
    Menu2[4] = "Speichern";

    // Navigation
    if (up && edititem == 0)
    {
      menuitem++;
      if (menuitem > 4)
      {
        menuitem = 4;
      }
    }
    else if (down && edititem == 0)
    {
      menuitem--;
      if (menuitem < 0)
      {
        menuitem = 0;
      }
    }

    if (buttonPressed)
    {
      switch (menuitem)
      {
      case 0: // Zurück
        page = 1;
        menuitem = 0;
        reflashLCD = true;
        lcd.noBlink();
        break;
      case 1:
      case 2:
        if (edititem == 0)
        {
          edititem = menuitem;
        }
        else
        {
          edititem = 0;
        }
        break;
      case 3: // Kalibriere (speichere Offset)
        page = 3;
        menuitem = 0;
        reflashLCD = true;
        lcd.noBlink();
        break;
      case 4: // Einstellungen speichern
        page = 4;
        menuitem = 0;
        reflashLCD = true;
        lcd.noBlink();
        break;
      }
    }
    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(">");
    lcd.print(Menu2[menuitem]);
    if ((menuitem + 1) <= 4)
    {
      lcd.setCursor(1, 1);
      lcd.print(Menu2[(menuitem + 1)]);
    }
    if (buttonPressed)
    {
      if (setMode != newMode)
      {
        setMode = newMode;
        saveConfig();
      }
    }

    // Ändere Einstellungen
    switch (edititem)
    {
    case 1: // Modus einstellen
      lcd.setCursor(7, 0);
      lcd.blink();
      if (up && newMode < sizeOfModes - 1)
      {
        newMode++;
        reflashLCD = true;
      }
      else if (down && newMode > 0)
      {
        newMode--;
        reflashLCD = true;
      }
      break;

    case 2: // Sollwert
      lcd.setCursor(7, 0);
      lcd.blink();
      if (up && (setMode == AUS || setMode == SPUNDOMAT || setMode == SPUNDEN_CO2 || setMode == KARBONISIEREN_CO2)) // Aus oder CO2
      {
        setCarbonation += 0.05;
        reflashLCD = true;
      }
      else if (down && (setMode == AUS || setMode == SPUNDOMAT || setMode == SPUNDEN_CO2 || setMode == KARBONISIEREN_CO2)) // Aus oder CO2
      {
        setCarbonation -= 0.05;
        reflashLCD = true;
      }
      else if (up && (setMode == SPUNDEN_DRUCK || setMode == KARBONISIEREN_DRUCK)) // Druck
      {
        setPressure += 0.05;
        reflashLCD = true;
      }
      else if (down && (setMode == SPUNDEN_DRUCK || setMode == KARBONISIEREN_DRUCK)) // Druck
      {
        setPressure -= 0.05;
        reflashLCD = true;
      }
      // else if (up && (setMode == STEUERUNG)) // Druck
      // {
      //   targetTemp += 0.1;
      //   reflashLCD = true;
      // }
      // else if (down && (setMode == STEUERUNG)) // Druck
      // {
      //   targetTemp -= 0.1;
      //   reflashLCD = true;
      // }
      break;

    default:
      lcd.noBlink();
      break;
    }
    break;

  case 3: // Switch(page) Kalibrierung
    reflashLCD = false;
    Menu3[0] = "Kalibrieren?";
    Menu3[1] = "Nein          Ja";

    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(Menu3[0]);
    lcd.setCursor(0, 1);
    lcd.print(Menu3[1]);
    lcd.setCursor(0, 1);
    lcd.blink();

    // Navigation
    if (up)
    {
      menuitem++;
      lcd.setCursor(14, 1);
      if (menuitem > 1)
      {
        menuitem = 1;
      }
    }
    else if (down)
    {
      menuitem--;
      lcd.setCursor(0, 1);
      if (menuitem < 0)
      {
        menuitem = 0;
      }
    }

    if (buttonPressed)
    {
      switch (menuitem)
      {
      default: // Auswahl Nein - Zurück
        page = 2;
        menuitem = 0;
        reflashLCD = true;
        lcd.noBlink();
        break;
      case 1: // Auswahl Ja - Kalibrierung
        readPressure();
        if (psettings.offset0 == 0) // Keine Kalibrierung bei 0 bar
        {
          psettings.offset0 = readSensor();
          writeFlash();
          // offset0 = readSensor();          
          // writeFloat(0, offset0);
        }
        else if (psettings.offset0 > 0 && readSensor() < 200)
        {
          psettings.offset0 = readSensor();
          writeFlash();
          // offset0 = readSensor();
          // writeFloat(0, offset0);
        }
        else if (psettings.offset0 > 0 && readSensor() > 200) // 2bar ca. 330
        {
          psettings.offset2 = readSensor();
          writeFlash();
          // offset2 = readSensor();
          // writeFloat(4, offset2);
        }

        readPressure();
        saveConfig();
        page = 2;
        menuitem = 0;
        reflashLCD = true;
        lcd.noBlink();
        break;
      }
    }
    break;
  case 4: // Switch(page) Speichern
    reflashLCD = false;
    Menu4[0] = "Speichern?";
    Menu4[1] = "Nein          Ja";

    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(Menu4[0]);
    lcd.setCursor(0, 1);
    lcd.print(Menu4[1]);
    lcd.setCursor(0, 1);
    lcd.blink();

    // Navigation
    if (up)
    {
      menuitem++;
      lcd.setCursor(14, 1);
      if (menuitem > 1)
      {
        menuitem = 1;
      }
    }
    else if (down)
    {
      menuitem--;
      lcd.setCursor(0, 1);
      if (menuitem < 0)
      {
        menuitem = 0;
      }
    }

    if (buttonPressed)
    {
      switch (menuitem)
      {
      case 1: // Auswahl Ja - Speichern
        saveConfig();
        page = 2;
        menuitem = 0;
        reflashLCD = true;
        lcd.noBlink();
        break;
      default: // Auswahl Nein - Zurück
        page = 2;
        menuitem = 0;
        reflashLCD = true;
        lcd.noBlink();
        break;
      }
    }
    break;
  } // Switch(page)
  buttonPressed = false;
}

void startLCD()
{
  lcd.home();
  lcd.noAutoscroll();
  showDisplay = true;
  handleDisplayTimer();
  // lcd.setBacklight(LedBackLight_On);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Spundomat V");
  lcd.setCursor(12, 0);
  lcd.print(Version);
  lcd.setCursor(0, 1);
  lcd.print("IP");
  lcd.setCursor(5, 1);
  lcd.print(WiFi.localIP().toString());
  lcd.setCursor(0, 2);
  lcd.print("WLAN");
  lcd.setCursor(5, 2);
  lcd.print(WiFi.SSID());
  if (startMDNS)
  {
    lcd.setCursor(0, 3);
    lcd.print("mDNS");
    lcd.setCursor(5, 3);
    lcd.print(nameMDNS);
  }
  millis2wait(PAUSE5SEC);
}

void updateLCD()
{
  // lcd.setBacklight(LedBackLight_On);
  handleDisplayTimer();
  lcd.home();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Spundomat V");
  lcd.setCursor(12, 0);
  lcd.print(Version);
  lcd.setCursor(0, 2);
  lcd.print(">Update gestartet");
}

void handleDisplayTimer()
{
  // Serial.printf("useBacklightTimer: %d showDisplay: %d DisplayIsOn: %d\n", useBacklightTimer, showDisplay, DisplayIsOn);
  if (useBacklightTimer) // DisplayTimer aktiviert
  {
    if (showDisplay) // Display an
    {
      lcd.display();                     // show display (with the previous content)
      lcd.setBacklight(LedBackLight_On); // turn on display background
      DisplayIsOn = true;
    }
    else
    {
      if (DisplayIsOn)
      {
        lcd.setBacklight(LedBackLight_Off); // turn off display background
        lcd.noDisplay();                    // turn off display content
        DisplayIsOn = false;
      }
    }
  }
  else
  {
    lcd.display();                     // show display (with the previous content)
    lcd.setBacklight(LedBackLight_On); // turn on display background
    DisplayIsOn = true;
  }
}
