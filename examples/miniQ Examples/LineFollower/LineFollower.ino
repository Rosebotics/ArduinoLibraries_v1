#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MiniQCom.h>
#include <MiniQ.h>


#define WHEEL_DIRECTION_FORWARD 0      // value for forward
#define WHEEL_DIRECTION_REVERSE 1         //value for go back

int lineFollowerReadings[5]={};
int leftWheelPwm=0;
int rightWheelPwm=0;
MiniQ miniQ;

void setup()
{
  miniQ.begin();
}

void loop()
{
  readValues();//read the value of sensors
  followLine();//function for line hunt
}


void readValues(void)//read the five sensors
{  
  for(int i=0;i<5;i++)
  {
    lineFollowerReadings[i]=analogRead(i);//store the value read from the sensors
  }
}

void followLine( )//function for line-hunt, white and black line, the value is less than 700 when detect line and for paper is bigger than 800
{
  if ((lineFollowerReadings[0] < 650 || lineFollowerReadings[1] < 650) && (lineFollowerReadings[3] > 800 && lineFollowerReadings[4] > 800)) //left detect black line
  {
    leftWheelPwm = 80;
    rightWheelPwm = 40; //turn right
  }
  else  if ((lineFollowerReadings[3] < 650 || lineFollowerReadings[4] < 650) && (lineFollowerReadings[0] > 800 && lineFollowerReadings[1] > 800)) //black line in the right
  {
    leftWheelPwm = 40;
    rightWheelPwm = 80; // turn left
  }
  else if (lineFollowerReadings[2] < 700) //line in middle
  {
//continue straight
    leftWheelPwm = 80;
    rightWheelPwm = 80;
  }
  else
  {
    //go slowly
    leftWheelPwm = 40;
    rightWheelPwm = 40;
  }


  miniQ.setDrivePwm(WHEEL_DIRECTION_FORWARD, WHEEL_DIRECTION_FORWARD, leftWheelPwm, rightWheelPwm); //undefine state just go slowly
}










