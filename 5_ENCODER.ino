// Lese Encoder
void readEncoder()
{
  if (encoder.getPosition() > encoderOldPos)
  {
    Serial.println("up");
    up = true;
    reflashLCD = true;
  }
  else if (encoder.getPosition() < encoderOldPos)
  {
    Serial.println("down");
    down = true;
    reflashLCD = true;
  }
  encoderOldPos = encoder.getPosition();
}


// Encoder gedrückt
void click()
{
  buttonPressed = true;
  reflashLCD = true;
}

ICACHE_RAM_ATTR void tick()
{
  encoder.tick();
}
