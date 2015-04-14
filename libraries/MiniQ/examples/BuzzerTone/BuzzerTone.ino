#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <MiniQCom.h>
#include <MiniQ.h>

MiniQ miniQ;

int numTones = 10;
int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A

void setup() {
  Serial.begin(9600);
  miniQ.begin();

  // Start with the buzzer off for 500 mS.
  miniQ.setBuzzerTone(0, 500);
  
  // Go through a simple scale.
  for (int i = 0; i < numTones; i++) {
    miniQ.setBuzzerTone(tones[i], 500);
  }
  miniQ.setBuzzerTone(0, 1000);
  
  // Play a fun song.
  marioTheme();
}

void loop() {
  if (Serial.available()) {
    char newByte = Serial.read();
    if (newByte == 'm') {
      marioTheme();
    }
  }
}

