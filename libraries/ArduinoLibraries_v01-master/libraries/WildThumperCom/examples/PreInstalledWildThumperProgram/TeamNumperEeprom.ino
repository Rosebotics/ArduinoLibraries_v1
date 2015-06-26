#define EEPROM_TEAM_NUMBER_ADDRESS 0
#define EEPROM_NO_VALUE 255

/**
 * Returns the team number stored in EEPROM.
 * If there is no value in EEPROM returns the default team number.
 */
byte readEepromTeamNumber() {
  byte eepromTeamNumber = EEPROM.read(EEPROM_TEAM_NUMBER_ADDRESS);
  return eepromTeamNumber == EEPROM_NO_VALUE ? DEFAULT_TEAM_NUMBER : eepromTeamNumber;
}

/**
 * Writes the team number to the EEPROM.
 */
void writeEepromTeamNumber(byte teamNumber) {
  EEPROM.write(EEPROM_TEAM_NUMBER_ADDRESS, teamNumber);
}
