void tickerTempCallback() // Timer Objekt Temperatur
{
  readTemparature();
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
