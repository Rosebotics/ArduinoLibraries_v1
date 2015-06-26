
//the right encoder isr function
void rightCount()
{
  //Serial.println("r");
  rVal++;
  if (rVal % 50 == 0) {
    Serial.print("Right Count ");
    Serial.println(rVal);
 //   Serial.println(rightWheelPwm);
  }
  if (encoderCount > 0) {
    encoderCount--;
  }
  else {
    encoderCount = 10;
    if (rVal > lVal) {
      rightWheelPwm--;
      rightWheelPwm = max(rightWheelPwm, 0);
    }
    else {
      rightWheelPwm++;
      rightWheelPwm = min(rightWheelPwm, 255);
    }
  }
}
//the left encoder isr function
void leftCount()
{
  //Serial.println("L");
  lVal++;
  if (lVal % 50 == 0) {
 Serial.print("Left Count  ");
 Serial.println(lVal);
  
}
}
