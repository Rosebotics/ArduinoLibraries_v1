
//#define FLAG_PLAYBACK_SCRIPT  0x0008  // Done in first tab.

TimerEvent playbackTimerEvent(playbackTimerEventCallback,  0);
unsigned long lastScriptTimeUsed = 0;
int playbackCounter = 0;
int playbackLength = 0;

int playbackTimerEventCallback() {
  mainEventFlags |= FLAG_PLAYBACK_SCRIPT;
  return 0;
}

void startPlayback() {
  playbackCounter = 0;
  playbackLength = getScriptLengthFromEeprom();
  // Get the first event time and schedule the callback.
  unsigned long scriptTime = getScriptTimeAtAddress(2);
  if (scriptTime > 0) {
    // I think 100% will fall into this category.
    playbackTimerEvent.setTimerTicksRemaining((int) scriptTime);
    lastScriptTimeUsed = scriptTime;
  } else {
    // Just in case the first event is at exactly 0.
    // After some testing delete this else area if it is worthless.
    runWheelSpeedCommandAtAddress(2 + 4);
    playbackCounter++;
    scriptTime = getScriptTimeAtAddress(2 + 8);
    lastScriptTimeUsed = scriptTime;
    playbackTimerEvent.setTimerTicksRemaining((int) scriptTime);
  }
  lcd.setCursor(6, LINE_2);
  lcd.print(playbackCounter);
  lcd.print("/");
  lcd.print(playbackLength);
  lcd.print("  ");
}

// Called from setup()
void setupExtensionForScripts() {
  tes.addTimerEvent(&playbackTimerEvent);
}

// Called from loop()
void loopExtensionForScripts() {
  if (mainEventFlags & FLAG_PLAYBACK_SCRIPT) {
    mainEventFlags &= ~FLAG_PLAYBACK_SCRIPT;
    if (state == PLAYBACK_ACTIVE) {
      runWheelSpeedCommandAtAddress(2 + 8 * playbackCounter + 4);
      lcd.setCursor(4, LINE_2);
      lcd.print(playbackCounter);
      lcd.print("/");
      lcd.print(playbackLength);
      lcd.print("  ");
    
      playbackCounter++;
      if (playbackCounter >= playbackLength) {
        // We are done.  Don't schedule another event.
        setState(NORMAL_DRIVE_RECORDING_OFF);
      } else {
        unsigned long nextScriptTime = getScriptTimeAtAddress(2 + 8 * playbackCounter);
        int deltaTime = nextScriptTime - lastScriptTimeUsed;
        lastScriptTimeUsed = nextScriptTime;
        playbackTimerEvent.setTimerTicksRemaining(deltaTime);
      }
    }
  }
}

