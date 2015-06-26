void obstacleAvoidance(void)
{
  char i;
  count = 0;
  for (i = 0; i < 20; i++) //left transmitter sends 20 pulses
  {
    L_Send40KHZ();
    delayMicroseconds(600);
  }
  if (count > 20) //if recieved a lot pulse , it means there's a obstacle
  {
    miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_REVERSE, 100, 100);
    delay(300);
    miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_FORWARD, 100, 100);
    delay(500);
  }
  else
  {
    count = 0;
    for (i = 0; i < 20; i++) //right transmitter sends 20 pulses
    {
      R_Send40KHZ();
      delayMicroseconds(600);
    }
    if (count > 20)
    {
      miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_REVERSE, 100, 100);
      delay(300);
      miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_REVERSE, 100, 100);
      delay(500);
    }
    else
    {
      miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 100, 100);
    }
  }
}
void L_Send40KHZ(void)//left ir transmitter sends 40kHZ pulse
{
  int i;
  for (i = 0; i < 24; i++)
  {
    digitalWrite(PIN_IR_LED_LEFT, LOW);
    delayMicroseconds(8);
    digitalWrite(PIN_IR_LED_LEFT, HIGH);
    delayMicroseconds(8);
  }
}
void R_Send40KHZ(void)//right ir transmitter sends 40kHZ pulse
{
  int i;
  for (i = 0; i < 24; i++)
  {
    digitalWrite(PIN_IR_LED_RIGHT, LOW);
    delayMicroseconds(8);
    digitalWrite(PIN_IR_LED_RIGHT, HIGH);
    delayMicroseconds(8);
  }
}
