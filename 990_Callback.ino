void tickerTempCallback() // Timer Objekt Temperatur
{
  readTemparature();
}

void tickerInfluxDBCallback() // Timer Objekt Influx Datenbank
{
  if (checkDBConnect()) // if checkDBConnect is successful it sets visState to 0
    sendDBData();
}

void tickerPressureCallback() // Timer Objekt Druck
{
  readPressure();
}

void tickerDisplayCallback() // Timer Objekt Druck
{
  showLCD();
}

void tickerDisplayTimerCallback() // Timer Objekt Druck
{
  handleDisplayTimer();
  showDisplay = false;
}

void tickerWLANCallback() // Ticker helper function calling Event WLAN Error
{
  /* if (TickerWLAN.counter() == 1)
  {
    switch (WiFi.status())
    {
    case 0: // WL_IDLE_STATUS
      DEBUG_MSG("WiFi status: error rc: %d WL_IDLE_STATUS");
      break;
    case 1: // WL_NO_SSID_AVAIL
      DEBUG_MSG("WiFi status: error rc: %d WL_NO_SSID_AVAIL!");
      break;
    case 2: // WL_SCAN_COMPLETED
      DEBUG_MSG("WiFi status: error rc: %d WL_SCAN_COMPLETED");
      break;
    case 3: // WL_CONNECTED
      DEBUG_MSG("WiFi status: error rc: %d WL_CONNECTED");
      break;
    case 4: // WL_CONNECT_FAILED
      DEBUG_MSG("WiFi status: error rc: %d WL_CONNECT_FAILED");
      break;
    case 5: // WL_CONNECTION_LOST
      DEBUG_MSG("WiFi status: error rc: %d WL_CONNECTION_LOST");
      break;
    case 6: // WL_DISCONNECTED
      DEBUG_MSG("WiFi status: error rc: %d WL_DISCONNECTED");
      break;
    case 255: // WL_NO_SHIELD
      DEBUG_MSG("WiFi status: error rc: %d WL_NO_SHIELD");
      break;
    default:
      break;
    }
  } */
  WiFi.reconnect();
}

void tickerTimeCallback()
{
  getLocalTime(&timeinfo);
  strftime(zeit, sizeof(zeit), "%H:%M:%S", &timeinfo);
}
