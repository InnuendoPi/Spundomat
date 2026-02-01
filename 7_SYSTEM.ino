void millis2wait(const int &value) // Warteschleife ohne delay (nonblocking)
{
  unsigned long pause = millis();
  while (millis() < pause + value)
  {
    yield();
  }
}
/*
// Prüfe WebIf Eingaben - ersetze Komma durch Punkt
// entfernt: WebIf client side validation

float formatDOT(String str)
{
  str.replace(',', '.');
  if (isValidDigit(str))
    return str.toFloat();
  else
    return 0;
}

float formatFLOAT(String str)
{
  str.replace(',', '.');
  if (isValidFloat(str))
    return str.toFloat();
  else
    return 0;
}

// Prüfe WebIf Eingaben
bool isValidFloat(const String &str)
{
  for (int i = 0; i < str.length(); i++)
  {
    if (i == 0)
    {
      if (str.charAt(i) == '-')
        continue;
    }
    if (str.charAt(i) == '.')
      continue;
    if (isdigit(str.charAt(i)))
      continue;
    return false;
  }
  return true;
}

bool isValidDigit(const String &str)
{
  for (int i = 0; i < str.length(); i++)
  {
    if (str.charAt(i) == '.')
      continue;
    if (isdigit(str.charAt(i)))
      continue;
    return false;
  }
  return true;
}

bool isValidInt(const String &str)
{
  for (int i = 0; i < str.length(); i++)
  {
    if (isdigit(str.charAt(i)))
      continue;
    return false;
  }
  return true;
}

bool checkRange(const String &str)
{
  int check = str.toInt();
  if (check >= 10 && check <= 7200000) //10ms bis 2 Stunden
    return true;
  else
    return false;
}

bool checkRange2(const String &str)
{
  int check = str.toInt();
  if (check >= 50 && check <= 7200000) //50ms bis 2 Stunden
    return true;
  else
    return false;
}

bool checkRangeDruck(const String &str)
{
  int check = str.toInt();
  if (check >= 0.0 && check <= 3.5)
    return true;
  else
    return false;
}
bool checkRangeCO2(const String &str)
{
  float check = str.toFloat();
  if (check >= 0.0 && check <= 8.0) //0gr/l bis 8gr/l
    return true;
  else
    return false;
}

char *checkChars(char *input)
{
  char *output = input;
  int j = 0;
  for (int i = 0; i < strlen(input); i++)
  {
    if (input[i] != ' ' && input[i] != '\n' && input[i] != '\r') // Suche nach Leerzeichen und CR LF
      output[j] = input[i];
    else
      j--;

    j++;
  }
  output[j] = '\0';
  return output;
}

void checkChars2(char *input)
{
  char *output = input;
  int j = 0;
  for (int i = 0; i < strlen(input); i++)
  {
    if (input[i] != ' ' && input[i] != '\n' && input[i] != '\r') // Suche nach Leerzeichen und CR LF
      output[j] = input[i];
    else
      j--;

    j++;
  }
  output[j] = '\0';
  *input = *output;
  return;
}

bool checkBool(const String &value)
{
  if (value == "true")
    return true;
  else
    return false;
}
*/

String PinToString(unsigned char &pinbyte)
{
  const unsigned char numberOfPins = 9;
  const unsigned char pins[numberOfPins] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};
  const String pin_names[numberOfPins] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8"};

  for (int i = 0; i < numberOfPins; i++)
  {
    if (pins[i] == pinbyte)
    {
      return pin_names[i];
    }
  }
  return "NaN";
}

void sendAlarm(const uint8_t &setAlarm)
{
  // DEBUG_MSG("SYS: setGPIO: %d\n", setGPIO);
  if (!buzzer)
    return;
  switch (setAlarm)
  {
  case ALARM_ON:
    // DEBUG_MSG("SYS: sendAlarm ON: %d\n", setAlarm);
    tone(PIN_BUZZER, 440, 50);
    millis2wait(150);
    tone(PIN_BUZZER, 660, 50);
    millis2wait(150);
    tone(PIN_BUZZER, 880, 50);
    break;
  case ALARM_OFF:
    // DEBUG_MSG("SYS: sendAlarm OFF: %d\n", setAlarm);
    tone(PIN_BUZZER, 880, 50);
    millis2wait(150);
    tone(PIN_BUZZER, 660, 50);
    millis2wait(150);
    tone(PIN_BUZZER, 440, 50);
    break;
  case ALARM_OK:
    // DEBUG_MSG("SYS: sendAlarm OK: %d\n", setAlarm);
    digitalWrite(PIN_BUZZER, HIGH);
    millis2wait(200);
    digitalWrite(PIN_BUZZER, LOW);
    break;
  case ALARM_WARNING:
    // DEBUG_MSG("SYS: sendAlarm WARN: %d\n", setAlarm);
    for (int i = 0; i < 3; i++)
    {
      tone(PIN_BUZZER, 880, 50);
      millis2wait(150);
      tone(PIN_BUZZER, 440, 50);
      millis2wait(150);
      millis2wait(PAUSE1SEC);
    }
    break;
  case ALARM_ERROR:
    // DEBUG_MSG("SYS: sendAlarm ERR: %d\n", setAlarm);
    for (int i = 0; i < 20; i++)
    {
      tone(PIN_BUZZER, 880, 50);
      millis2wait(150);
      tone(PIN_BUZZER, 440, 50);
      millis2wait(150);
    }
    millis2wait(PAUSE1SEC);
    break;
  default:
    // DEBUG_MSG("SYS: sendAlarm DEF: %d\n", setAlarm);
    break;
  }
}

void calcVerzSpundomat()
{
  if (setEinheit == 0) // Verzögerung in Minuten
  {
    verzKarbonisierung = verzKombi * 1000 * 60;
    minKarbonisierung = 0.0;
  }
  else if (setEinheit == 1) // Verzögerung in Stunden
  {
    verzKarbonisierung = verzKombi * 1000 * 60 * 60;
    minKarbonisierung = 0.0;
  }
  else if (setEinheit == 2) // Verzögerung min CO2-Gehalt
  {
    verzKarbonisierung = 0;
    minKarbonisierung = verzKombi;
  }
}

void setTicker() // Ticker Objekte deklarieren
{
  TickerTemp.config(tickerTempCallback, upTemp, 0);
  TickerPressure.config(tickerPressureCallback, upPressure, 0);
  TickerInfluxDB.config(tickerInfluxDBCallback, upInflux, 0);
  TickerDisplay.config(tickerDisplayCallback, DISPLAY_UPDATE, 0);
  TickerWLAN.config(tickerWLANCallback, WLAN_UPDATE, 0);
  TickerDisplayTimer.config(tickerDisplayTimerCallback, DisplayTimerTime, 0);
  TickerTime.config(tickerTimeCallback, TIME_UPDATE, 0);
  TickerCon.config(tickerConCallback, CON_UPDATE, 0);
}

void EM_WLAN() // check WLAN and reconnect on error
{
  if (WiFi.status() == WL_CONNECTED)
  {
    if (TickerWLAN.state() == RUNNING)
      TickerWLAN.stop();
  }
  else
  {
    if (TickerWLAN.state() != RUNNING)
    {
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);
      TickerWLAN.resume();
    }
    TickerWLAN.update();
  }
}

void setupTime()
{
  configTzTime(ntpZone, ntpServer);
  getLocalTime(&timeinfo);

  // Test Zeitumstellung
  // 1 Minute vor Winterzeit 27.10.2024 2:59:00 1729994340
  // 1 Minute vor Sommerzeit 31.03.2024 1:59:00 1711846740
  // timeval epoch = {1711846740, 0};
  // settimeofday((const timeval*)&epoch, 0);

  strftime(zeit, sizeof(zeit), "%H:%M:%S", &timeinfo);
  TickerTime.start();
}

String getDayOfWeek(int value)
{
  switch (value)
  {
  case 0:
    return "Sonntag";
    break;
  case 1:
    return "Montag";
    break;
  case 2:
    return "Dienstag";
    break;
  case 3:
    return "Mittwoch";
    break;
  case 4:
    return "Donnerstag";
    break;
  case 5:
    return "Freitag";
    break;
  case 6:
    return "Samstag";
    break;
  default:
    return "";
  }
}


void readFlash()
{
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, psettings);
  EEPROM.end();
  // DEBUG_MSG("readFlash offset0: %g\toffset2: %g\n", psettings.offset0, psettings.offset2);
}

void writeFlash()
{
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(0, psettings); // Schreiben einer zweiten Structur ab Adresse 0
  EEPROM.commit();
  EEPROM.end();
  // DEBUG_MSG("writeFlash offset0: %g\toffset2: %g\n", psettings.offset0, psettings.offset2);
}

void eraseFlash()
{
  psettings.offset0 = 0;
  psettings.offset2 = 0;

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.put(0, psettings);
  EEPROM.commit();
  EEPROM.end();

  // DEBUG_MSG("eraseFlash size of psettings: %d\n", sizeof(psettings)); // 8 byte
}
