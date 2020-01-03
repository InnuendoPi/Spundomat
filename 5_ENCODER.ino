//Lese Encoder ein
void readEncoder()
{
  if (encoder.getPosition() > encoderOldPos)
  {
    up = true;
    reflashLCD = true;
  }
  else if (encoder.getPosition() < encoderOldPos)
  {
    down = true;
    reflashLCD = true;
  }
  encoderOldPos = encoder.getPosition();
}

//Encoder gedrückt
void click()
{
  buttonPressed = true;
  reflashLCD = true;
}

ICACHE_RAM_ATTR void tick()
{
  encoder.tick();
}
