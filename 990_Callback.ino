void tickerTempCallback() // Timer Objekt Temperatur
{
  readTemparature();
}

void tickerInfluxDBCallback() // Timer Objekt Influx Datenbank
{
  sendDBData();
}

void tickerPressureCallback() // Timer Objekt Druck
{
  readPressure();
}

void tickerEncoderCallback() // Timer Objekt Druck
{
  reflashLCD = true;
}
