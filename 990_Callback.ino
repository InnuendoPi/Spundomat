void tickerTempCallback() // Timer Objekt Temperatur
{
  readTemparature();
}

void tickerPressureCallback() // Timer Objekt Druck
{
  readPressure();
}

void tickerMV1Callback() // Timer Objekt Spunden
{
  if (setMode == 1)
  {
    if (pressure > calcPressure(setCarbonation, temperature))
      releasePressure();
  }
  else if (setMode == 2)
  {
    if (pressure > setPressure)
      releasePressure();
  }
  if (statusMV1)
  {
    TickerMV1.interval(mv1Open); // Setze Intervall auf Low (geschlossen)
    statusMV1 = false;
  }
  else
  {
    TickerMV1.interval(mv1Close); // Setze Intervall auf Low (geschlossen)
    statusMV1 = true;
  }
}

void tickerMV2Callback() // Timer Objekt Karbonisieren
{
  if (pressure < calcPressure(setCarbonation, temperature))
  {
    buildPressure();
    if (statusMV2)
    {
      TickerMV2.interval(mv2Open);
      statusMV2 = false;
    }
    else
    {
      TickerMV2.interval(mv2Close);
      statusMV2 = true;
    }
  }
}

void tickerNTPCallback() // Timer Objekt Druck
{
  timeClient.update();
}

void tickerEncoderCallback() // Timer Objekt Druck
{
  readEncoder();
}

void tickerButtonCallback() // Timer Objekt Druck
{
  button.tick();
}
