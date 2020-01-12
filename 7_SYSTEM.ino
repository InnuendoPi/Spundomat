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
/*
void DBGPRINT(const char *str, ...)
{
  if (setDEBUG)
  {
    int i, count = 0, j = 0, flag = 0;
    char temp[DBGBUFFER + 1];
    va_list argv;
    va_start(argv, str);
    for (i = 0, j = 0; str[i] != '\0'; i++) // Iteriere über die Zeichen des Strings
    {
      if (str[i] == '%')
      {
        // Lösche buffer
        temp[j] = '\0';
        if (Telnet.connected())
          Telnet.print(temp);
        else
          Serial.print(temp);
        j = 0;
        temp[0] = '\0';

        switch (str[++i])
        {
        case 'd': // Integer
          if (Telnet.connected())
            Telnet.print(va_arg(argv, int));
          else
            Serial.print(va_arg(argv, int));
          break;
        case 'l': // Long
          if (Telnet.connected())
            Telnet.print(va_arg(argv, long));
          else
            Serial.print(va_arg(argv, long));
          break;
        case 'f': // Float
          if (Telnet.connected())
            Telnet.print(va_arg(argv, double));
          else
            Serial.print(va_arg(argv, double));
          break;
        case 'c': // Char
          if (Telnet.connected())
            Telnet.print((char)va_arg(argv, int));
          else
            Serial.print((char)va_arg(argv, int));
          break;
        case 's': // String
          if (Telnet.connected())
            Telnet.print(va_arg(argv, char *));
          else
            Serial.print(va_arg(argv, char *));
          break;
        default:;
        };
      }
      else
      {
        //Zum buffer hinzufügen
        temp[j] = str[i];
        j = (j + 1) % DBGBUFFER;
        if (j == 0) // Ist der buffer voll dann leere den buffer
        {
          temp[DBGBUFFER] = '\0';
          if (Telnet.connected())
            Telnet.print(temp);
          else
            Serial.print(temp);
          temp[0] = '\0';
        }
      }
    };
    if (Telnet.connected())
      Telnet.println(); // Zeilenumbruch am Ende
    else
      Serial.println();
  }
}
*/