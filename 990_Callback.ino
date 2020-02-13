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
  readEncoder();
}

void tickerButtonCallback() // Timer Objekt Druck
{
  button.tick();
}
void tickerSpundomatCallback() // Timer Objekt Kombi-Modus
{
  updateSpundomat();
}