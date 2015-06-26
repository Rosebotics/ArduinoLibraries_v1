/*
A simple template designed for customization of the remote control function of the miniQ
Incorporates obstacle avoidance and line following examples
Last modified 14th June 2015
*/


#include <IRremote.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MiniQCom.h>
#include <MiniQ.h>


//Variables
int16_t lineFollowerReadings[5] = {};
uint16_t rVal = 0;
uint16_t lVal = 0;
uint16_t leftWheelPwm = 125;
uint16_t rightWheelPwm = 125;
int8_t pwmAdjust = 0;
bool lineFollowerToggle = false;
int count = 0;
uint8_t encoderCount = 10; //Used to adjust the time between right and left endocer checks
MiniQ miniQ;
IRrecv irrecv(PIN_CENTER_IR_MODULE);
decode_results results;


//State variable, describes what State the miniQ is currently in. 
enum
{
  FORWARD,
  REVERSE,
  STOP,
  LINE_FOLLOWER,
  TURN_RIGHT,
  TURN_LEFT,
  OBSTACLE_AVOIDANCE,

} State;


void setup()
{
  State = STOP;
  miniQ.begin();
  irrecv.enableIRIn();
  miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 0, 0);
  pcint0_init();
  attachInterrupt(RIGHT_ENCODER_INTERRUPT, rightCount , CHANGE);
  attachInterrupt(LEFT_ENCODER_INTERRUPT, leftCount, CHANGE);
  sei();

}

void loop()
{
  readValue();//read the value of line following sensors
  if (lineFollowerToggle) {
    followLine();//control function for following the line
  }
  if (irrecv.decode(&results)) {
    dump(&results);
    irrecv.resume();
  }
  updatePosition();

}


void dump(decode_results* results) {
  switch (results->value) {
    case BUTTON_PRESS_STOP:
      State = STOP;
      //Serial.println("Stopping");
      break;
    case BUTTON_PRESS_POWER:
      State = STOP;
      //Serial.println("Powering off");
      break;
    case BUTTON_PRESS_PLAY:
      if (lineFollowerToggle) {
        lineFollowerToggle = false;
        State = STOP;
      }
      else {
        lineFollowerToggle = true;
        State = LINE_FOLLOWER;
      }
      //Serial.println("Look, I'm toggling! I should wear toggles");
      break;
    case BUTTON_PRESS_VOL_UP:
      State = FORWARD;
      //Serial.println("Forward!");
      break;
    case BUTTON_PRESS_VOL_DOWN:
      State = REVERSE;
      //Serial.println("Reversing");
      break;
    case BUTTON_PRESS_PREVIOUS:
      miniQ.setDrivePwm(WHEEL_DIRECTION_REVERSE, WHEEL_DIRECTION_FORWARD, (leftWheelPwm + pwmAdjust) / 2, (rightWheelPwm + pwmAdjust) / 2);
      delay(200);
      miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 0, 0);
      //Serial.println("Turning Left");
      break;
    case BUTTON_PRESS_NEXT:
      miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_REVERSE, (leftWheelPwm + pwmAdjust) / 2, (rightWheelPwm + pwmAdjust) / 2);
      delay(200);
      miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, 0, 0);
      //Serial.println("Turning Right");
      break;
    case BUTTON_PRESS_0:
      State = OBSTACLE_AVOIDANCE;
      break;
    case BUTTON_PRESS_UP:

      pwmAdjust += 10;
      if (pwmAdjust > 110) {
        pwmAdjust = 110;
      }
      break;
    case BUTTON_PRESS_DOWN:
      pwmAdjust -= 10;
      if (pwmAdjust < -110) {
        pwmAdjust = -110;
      }
      break;
    case BUTTON_PRESS_1:
      rVal = 0;
      lVal = 0;
      break;
      case BUTTON_PRESS_2:
      leftWheelPwm=120;
      rightWheelPwm=120;
    default:
      break;
  }
}
void pcint0_init(void)//init the interrupt
{
  PCICR = 0X01;//
  PCMSK0 = 0X01;//
}
ISR(PCINT0_vect)//motor encoder interrupt
{
  count++;
}

