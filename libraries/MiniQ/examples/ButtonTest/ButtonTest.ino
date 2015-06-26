/*   Simple test to identify which button is pressed on the MiniQ

All 3 buttons share one pin, so a voltage divider setup is used to differenitate between the buttons presses.


Last modified 06/12/15
*/
#include <Adafruit_NeoPixel.h>
#include <MiniQCom.h>
#include <MiniQ.h>
#include <Wire.h>
#define TIME_DELAY 1000

int analogValue;

void setup() {
  pinMode(PIN_ANALOG_READING_FOR_BUTTONS,INPUT);
}

void loop() {

  delay(TIME_DELAY);
  
analogValue=analogRead(PIN_ANALOG_READING_FOR_BUTTONS);

Serial.println(analogValue);
 
 if (analogValue<BUTTON_1_THRESHOLD){
 Serial.println("Button 1 Pressed");}
 else  if (analogValue<BUTTON_2_THRESHOLD){
 Serial.println("Button 2 Pressed");}
  else if (analogValue<BUTTON_3_THRESHOLD){
 Serial.println("Button 3 Pressed");}
 else
 {Serial.println("No button pressed");}
 
 }