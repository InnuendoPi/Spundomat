void millis2wait(const int &value)
{
  unsigned long pause = millis();
  while (millis() < pause + value)
  {
    yield();
  }
}

// Prüfe WebIf Eingaben - ersetze Komma durch Punkt
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
  if (check >= 10 && check <= 7200000) //20ms bis 2 Stunden
    return true;
  else
    return false;
}

bool checkRange2(const String &str)
{
  int check = str.toInt();
  if (check >= 50 && check <= 7200000) //20ms bis 2 Stunden
    return true;
  else
    return false;
}

bool checkRangeDruck(const String &str)
{
  int check = str.toInt();
  if (check >= 0.0 && check <= 3.5) //20ms bis 2 Stunden
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
  DEBUG_MSG("SYS: setGPIO: %d\n", setGPIO);
  if (setGPIO != 1)
    return;
  switch (setAlarm)
  {
  case ALARM_ON:
    DEBUG_MSG("SYS: sendAlarm ON: %d\n", setAlarm);
    tone(PIN_BUZZER, 440, 50);
    delay(150);
    tone(PIN_BUZZER, 660, 50);
    delay(150);
    tone(PIN_BUZZER, 880, 50);
    break;
  case ALARM_OFF:
    DEBUG_MSG("SYS: sendAlarm OFF: %d\n", setAlarm);
    tone(PIN_BUZZER, 880, 50);
    delay(150);
    tone(PIN_BUZZER, 660, 50);
    delay(150);
    tone(PIN_BUZZER, 440, 50);
    break;
  case ALARM_OK:
    DEBUG_MSG("SYS: sendAlarm OK: %d\n", setAlarm);
    digitalWrite(PIN_BUZZER, HIGH);
    delay(200);
    digitalWrite(PIN_BUZZER, LOW);
    break;
  case ALARM_WARNING:
    DEBUG_MSG("SYS: sendAlarm WARN: %d\n", setAlarm);
    for (int i = 0; i < 3; i++)
    {
      tone(PIN_BUZZER, 880, 50);
      delay(150);
      tone(PIN_BUZZER, 440, 50);
      delay(150);
      millis2wait(PAUSE1SEC);
    }
    break;
  case ALARM_ERROR:
    DEBUG_MSG("SYS: sendAlarm ERR: %d\n", setAlarm);
    for (int i = 0; i < 20; i++)
    {
      tone(PIN_BUZZER, 880, 50);
      delay(150);
      tone(PIN_BUZZER, 440, 50);
      delay(150);
    }
    millis2wait(PAUSE1SEC);
    break;
  default:
    DEBUG_MSG("SYS: sendAlarm DEF: %d\n", setAlarm);
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

bool checkBool(const String &value)
{
  if (value == "true")
    return true;
  else
    return false;
}

void setTicker() // Ticker Objekte deklarieren
{
  TickerTemp.config(tickerTempCallback, upTemp, 0);
  TickerPressure.config(tickerPressureCallback, upPressure, 0);
  TickerInfluxDB.config(tickerInfluxDBCallback, upInflux, 0);
  TickerDisplay.config(tickerDisplayCallback, DISPLAY_UPDATE, 0);
  TickerWLAN.config(tickerWLANCallback, WLAN_UPDATE, 0);
  
  // TickerCO2.config(tickerCO2Callback, CO2_UPDATE, 0);
  // TickerSteuerung.config(tickerSteuerungCallback, TARGET_UPDATE, 0);
  // TickerAlarmierung.config(tickerAlarmierungCallback, ZUSATZALARM, 0);
  // TickerCon.config(tickerConCallback, CON_UPDATE, 0);
}

void checkSummerTime()
{
  time_t rawtime = timeClient.getEpochTime();
  struct tm *ti;
  ti = localtime(&rawtime);
  int year = ti->tm_year + 1900;
  int month = ti->tm_mon + 1;
  int day = ti->tm_mday;
  int hour = ti->tm_hour;
  int tzHours = 1; // UTC: 0 MEZ: 1
  int x1, x2, x3, lastyear;
  int lasttzHours;
  if (month < 3 || month > 10)
  {
    timeClient.setTimeOffset(3600);
    return;
  }
  if (month > 3 && month < 10)
  {
    timeClient.setTimeOffset(7200);
    return;
  }
  if (year != lastyear || tzHours != lasttzHours)
  {
    x1 = 1 + tzHours + 24 * (31 - (5 * year / 4 + 4) % 7);
    x2 = 1 + tzHours + 24 * (31 - (5 * year / 4 + 1) % 7);
    lastyear = year;
    lasttzHours = tzHours;
  }
  x3 = hour + 24 * day;
  if (month == 3 && x3 >= x1 || month == 10 && x3 < x2)
  {
    timeClient.setTimeOffset(7200);
    return;
  }
  else
  {
    timeClient.setTimeOffset(3600);
    return;
  }
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
