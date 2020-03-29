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

// Prüfe WebIf Eingaben
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
  if (!startBuzzer)
    return;
  switch (setAlarm)
  {
  case ALARM_ON:
    tone(PIN_BUZZER, 440, 50);
    delay(150);
    tone(PIN_BUZZER, 660, 50);
    delay(150);
    tone(PIN_BUZZER, 880, 50);
    break;
  case ALARM_OFF:
    tone(PIN_BUZZER, 880, 50);
    delay(150);
    tone(PIN_BUZZER, 660, 50);
    delay(150);
    tone(PIN_BUZZER, 440, 50);
    break;
  case ALARM_OK:
    digitalWrite(PIN_BUZZER, HIGH);
    delay(200);
    digitalWrite(PIN_BUZZER, LOW);
    break;
  case ALARM_ERROR:
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