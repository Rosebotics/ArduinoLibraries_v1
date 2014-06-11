#include <RobotAsciiCom.h>

RobotAsciiCom robotCom;

void setup() {
  Serial.begin(115200);
  delay(100);
  robotCom.registerBatteryVoltageRequestCallback(batteryVoltageRequestCallback);
  robotCom.registerCurrentRequestCallback(currentRequestCallback);
  robotCom.registerPwmCallback(pwmCallback);
  robotCom.registerCustomStringCallback(customStringCallback);
}

void batteryVoltageRequestCallback(void) {
  Serial.println();
  Serial.print("Battery voltage request");
  Serial.println();
}

void currentRequestCallback(void) {
  Serial.println();
  Serial.print("Current request");
  Serial.println();
}

void pwmCallback(int leftDutyCycle, int rightDutyCycle) {
  Serial.println();
  Serial.print("PWM command.  ");
  Serial.print(leftDutyCycle);
  Serial.print("  ");
  Serial.print(rightDutyCycle);  
  Serial.println();
}

void customStringCallback(String customString) {
  Serial.print(customString);  
  Serial.println();
}


void loop() {  
  Serial.print("Test 1: ");
  test1();
  Serial.println("");
  Serial.print("Test 2: ");
  test2();
  Serial.println("");
  Serial.print("Test 3: ");
  test3();
  Serial.println("");
  Serial.print("Test 4: ");
  test4();
  Serial.println("");
  Serial.print("Test 5: ");
  test5();
  Serial.println("");
  Serial.print("Test 6: ");
  test6();
  Serial.println("");
  Serial.println("Test 7: ");
  test7();
  Serial.println("");
  Serial.println("Test 8: ");
  test8();
  Serial.println("");
  Serial.println("Test 9: ");
  test9();
  Serial.println("");
  // Output should be... (ignoring line breaks)
  // Test 1: Pwm command. 123  -80
  // Test 2: 
  // Test 3: Battery voltage request
  // Test 4: 
  // Test 5: 
  // Test 6: 
  // Test 7: Hello, World!
  // Test 8: 
  // Test 9: 
  
  while(1);
}

void test1() {
  byte testArray[] = "PWM 123 -80";
  testArray[17] = '\n';
  robotCom.handleRxBytes(testArray, 18);
}

void test2() {

}

void test3() {
  robotCom.handleRxByte('B');
  robotCom.handleRxByte('A');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte('Y');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('V');
  robotCom.handleRxByte('O');
  robotCom.handleRxByte('L');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('A');
  robotCom.handleRxByte('G');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte(' ');
  robotCom.handleRxByte('R');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('Q');
  robotCom.handleRxByte('U');
  robotCom.handleRxByte('E');
  robotCom.handleRxByte('S');
  robotCom.handleRxByte('T');
  robotCom.handleRxByte('\n');
}

void test4() {
}

void test5() {
}

void test6() {
}

void test7() {
  byte testArray[] = "CUSTOM Hello, World! ";
  testArray[20] = '\n';
  robotCom.handleRxBytes(testArray, 21);
}

void test8() {
}

void test9() {
}