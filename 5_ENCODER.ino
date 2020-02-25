// Lese Encoder

void readEncoder()
{
  // DEBUG_MSG("Encoder old position: %d\n", encoderOldPos);
  if (encoder.getPosition() < encoderOldPos)
  {
    // DEBUG_MSG("%s", "up\n");
    up = true;
    reflashLCD = true;
  }
  else if (encoder.getPosition() > encoderOldPos)
  {
    // DEBUG_MSG("%s", "down\n");
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
  // DEBUG_MSG("%s", "pressed\n");
}

ICACHE_RAM_ATTR void tick()
{
  encoder.tick();
  // DEBUG_MSG("%s", "tick\n");
}
