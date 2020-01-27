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
  if (check >= 20 && check <= 7200000) //20ms bis 2 Stunden
    return true;
  else
    return false;
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
    delay(50);
    tone(PIN_BUZZER, 880, 50);
    delay(50);
    break;
  case ALARM_OFF:
    tone(PIN_BUZZER, 880, 50);
    delay(50);
    tone(PIN_BUZZER, 440, 50);
    delay(50);
    break;
  case ALARM_OK:
    tone(PIN_BUZZER, 880, 50);
    delay(50);
    break;
  case ALARM_ERROR:
    tone(PIN_BUZZER, 220, 500);
    delay(500);
    break;
  case ALARM_PANIC:
    for (byte count2 = 0; count2 != 3; count2++)
    {
      for (byte count = 0; count != 250; count++)
      {
        tone(PIN_BUZZER, 200 + 10 * count, 10);
        delay(1);
      }
    }
    break;
  default:
    break;
  }
}

void setTicker() // Ticker Objekte deklarieren
{
  TickerTemp.config(tickerTempCallback, upTemp, 0);
  TickerPressure.config(tickerPressureCallback, upPressure, 0);
  TickerEncoder.config(tickerEncoderCallback, ENCODER_UPDATE, 0);
  TickerButton.config(tickerButtonCallback, BUTTON_UPDATE, 0);
}
