void millis2wait(const int &value)
{
  unsigned long pause = millis();
  while (millis() < pause + value)
  {
    //wait approx. [period] ms
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

// format bytes
String formatBytes(size_t bytes)
{
  if (bytes < 1024)
  {
    return String(bytes) + "B";
  }
  else if (bytes < (1024 * 1024))
  {
    return String(bytes / 1024.0) + "KB";
  }
  else if (bytes < (1024 * 1024 * 1024))
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
  else
  {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

void DBG_PRINT(const String &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.print(value);
    else
      Serial.print(value);
  }
}
void DBG_PRINT(const int &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.print(value);
    else
      Serial.print(value);
  }
}
void DBG_PRINT(const unsigned int &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.print(value);
    else
      Serial.print(value);
  }
}
void DBG_PRINT(const long unsigned int &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.print(value);
    else
      Serial.print(value);
  }
}
void DBG_PRINT(const long &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.print(value);
    else
      Serial.print(value);
  }
}
void DBG_PRINTLN(const long &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.println(value);
    else
      Serial.println(value);
  }
}
void DBG_PRINT(const float &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.print(value);
    else
      Serial.print(value);
  }
}
void DBG_PRINT(const double &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.print(value);
    else
      Serial.print(value);
  }
}
void DBG_PRINTHEX(const int &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.print(value, HEX);
    else
      Serial.print(value, HEX);
  }
}
void DBG_PRINTLN(const String &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.println(value);
    else
      Serial.println(value);
  }
}
void DBG_PRINTLN(const int &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.println(value);
    else
      Serial.println(value);
  }
}
void DBG_PRINTLN(const unsigned int &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.println(value);
    else
      Serial.println(value);
  }
}
void DBG_PRINTLN(const long unsigned int &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.println(value);
    else
      Serial.println(value);
  }
}
void DBG_PRINTLN(const float &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.println(value);
    else
      Serial.println(value);
  }
}
void DBG_PRINTLN(const double &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.println(value);
    else
      Serial.println(value);
  }
}
void DBG_PRINTLNHEX(const int &value)
{
  if (setDEBUG)
  {
    if (Telnet.connected())
      Telnet.println(value, HEX);
    else
      Serial.println(value, HEX);
  }
}
void DBG_PRINTLNTS(unsigned long value) // Timestamp
{
  value = value / 1000;
  if (setDEBUG)
  {
    if (Telnet.connected())
    {
      Telnet.print((value / 3600) % 24); // Stunden
      Telnet.print(":");
      Telnet.print((value / 60) % 60); // Minuten
      Telnet.print(":");
      Telnet.println(value % 60); // Sekunden
    }
    else
    {
      Serial.print((value / 3600) % 24); // Stunden
      Serial.print(":");
      Serial.print((value / 60) % 60); // Minuten
      Serial.print(":");
      Serial.println(value % 60); // Sekunden
    }
  }
}
