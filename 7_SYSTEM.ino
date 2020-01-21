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
  if (check > 100 && check < 7200000 ) //20ms bis 2 Stunden
    return true;
  else
    return false;
}

void setTicker() // Ticker Objekte deklarieren
{
  TickerTemp.config(tickerTempCallback, upTemp, 0);
  TickerPressure.config(tickerPressureCallback, upPressure, 0);
  TickerMV1.config(tickerMV1Callback, mv1Open, 0);
  TickerMV2.config(tickerMV2Callback, mv2Open, 0);
  TickerNTP.config(tickerNTPCallback, NTP_INTERVAL, 0);
  TickerEncoder.config(tickerEncoderCallback, ENCODER_UPDATE, 0);
  TickerButton.config(tickerButtonCallback, BUTTON_UPDATE, 0);
}
