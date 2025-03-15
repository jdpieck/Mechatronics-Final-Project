#include "keypad.h"
#include "motor.h"

void setup() {
  Serial.begin(9600);

    // pin mode for encoder B 
  pinMode(ENCODER_B, INPUT);
  // set up interrupt
  attachInterrupt(0, doEncoderA, CHANGE); 
  attachInterrupt(1, doEncoderB, CHANGE); 
  //Motor pins
  pinMode(MOTOR_ENABLE,OUTPUT); // speed with PWM output
  pinMode(DIR,OUTPUT);   // direction pin
  pinMode(BRAKE,OUTPUT);   // direction pin

  digitalWrite(DIR, LOW); //Establishes forward direction of Channel A
  digitalWrite(BRAKE, LOW);   //Disengage the Brake for Channel A
  Serial.print("Encoder position: ");Serial.println(encoderPos);

  
}

void loop() {
  char key = getKeyInput();  // Use the wrapper function
  
  if (key) {
      Serial.println(key);
  }

  if (key == 'A') {
    Serial.println("CW");
    setMotorCW();
    driveMotor(55);
  }
  if (key == 'B') {
    Serial.println("CCW");
    setMotorCCW();
    driveMotor(55);
  }
  if (key=='C') {
    stopMotor();
    Serial.println("Stop");
  }

  updateEncorderPosition();

}
