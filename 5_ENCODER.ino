// Lese Encoder
void readEncoder()
{
  if (encoder.getPosition() < encoderOldPos)
  {
    showDisplay = true;
    if (DisplayIsOn)
    {
      up = true;
      reflashLCD = true;
    }
  }
  else if (encoder.getPosition() > encoderOldPos)
  {
    showDisplay = true;
    if (DisplayIsOn)
    {
      down = true;
      reflashLCD = true;
    }
  }
  encoderOldPos = encoder.getPosition();
}

// Encoder gedrückt
void click()
{
    showDisplay = true;
    if (DisplayIsOn)
    {
      buttonPressed = true;
      reflashLCD = true;
    }
}

ICACHE_RAM_ATTR void tick()
{
  encoder.tick();
}
