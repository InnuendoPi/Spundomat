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

String PinToString(unsigned char pinbyte)
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

// void sendAlarm()
// {
//   DEBUG_MSG("Starte Alarm %d\n", PIEZZO_UPDATE);
//   analogWrite(PIN_BUZZER, 20); // 0-255
//   delay(50);
//   analogWrite(PIN_BUZZER, 0); // 0-255
//   //tone(PIN_BUZZER, 440, 50);
//   delay(50);
//   //noTone(PIN_BUZZER);
// }

void setTicker() // Ticker Objekte deklarieren
{
  TickerTemp.config(tickerTempCallback, upTemp, 0);
  TickerPressure.config(tickerPressureCallback, upPressure, 0);
  TickerEncoder.config(tickerEncoderCallback, ENCODER_UPDATE, 0);
  TickerButton.config(tickerButtonCallback, BUTTON_UPDATE, 0);
  // TickerPiezzo.config(tickerPiezzoCallback, PIEZZO_UPDATE, 3);
}
