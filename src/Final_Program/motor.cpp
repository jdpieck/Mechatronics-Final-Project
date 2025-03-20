// #include "motor.h"
#include <math.h>
#include <Arduino.h>

#define CLOCK_FREQUENCY 16000000
#define SERIAL_BAUDRATE 57600

// encoder pins
#define ENCODER_A 2
#define ENCODER_B 3
// motor pins
#define MOTOR_ENABLE 11
#define DIR 13
#define BRAKE 8
#define MAX_PWM 255
#define MIN_PWM 55 // this one depends on the dead zone of the motor input voltage
#define PPR 1040   // 1080 is ideally, but there may exists some offset from your observation and sensor noise 

// interpolated P-control
#define KP 0.12  // P control parameter
#define TARGET_DIST 100 // pulses
#define DIS2GO 5
int dist_moved = 0;
int controlLoopRate = 95;
int distanceToGo;    
int positionError=0;
int PWM_value;

const float dial_ticks = 60; // dial padlock resolution

// encoder reading flags
volatile byte AfirstUp = 0; 
volatile byte BfirstUp = 0; 
volatile byte AfirstDown = 0; 
volatile byte BfirstDown = 0; 
// CW when it increases, CCW when it decreases
volatile int encoderPos = 0; //this variable stores our current value of encoder position. 
volatile int lastEncoderPos = 0; //this variable stores our last value of encoder position. 
volatile byte pinA_read = 0; // this variable is shared among interrupts
volatile byte pinB_read = 0; // this variable is shared among interrupts


void setMotorCW(){
  digitalWrite(DIR, LOW);
}

void setMotorCCW(){
  digitalWrite(DIR, HIGH);
}

void driveMotor(int motor_speed){
  digitalWrite(BRAKE, LOW); // release the brake before setting the speed
  motor_speed = min(motor_speed, MAX_PWM); // upper bound
  motor_speed = max(motor_speed, MIN_PWM);  // lower bound
  analogWrite(MOTOR_ENABLE, motor_speed);
}

void stopMotor(){
  digitalWrite(BRAKE, HIGH);   //Engage the Brake for Channel A
  analogWrite(MOTOR_ENABLE, 0);   
}
// after each robotation of the motor, reset the control parameter
void resetMotorControl(int pulses){
  noInterrupts();
  encoderPos = 0;
  lastEncoderPos = 0;
  AfirstUp = 0; //reset flags for the next turn
  BfirstUp = 0; //reset flags for the next turn
  AfirstDown = 0; //reset flags for the next turn
  BfirstDown = 0; //reset flags for the next turn
  // clear position error
  positionError = 0;
  PWM_value = 0;
  dist_moved = 0;
  distanceToGo = pulses;
  interrupts();
}
// convert delta dial distance to pulses
int dialDistanceToPulses(int deltaDials){
  int delta_pulses;
  delta_pulses = deltaDials/dial_ticks*PPR;
  
  return delta_pulses;
}

// ISR for encoder reading: Channel A
void doEncoderA(){
  pinA_read = digitalRead(ENCODER_A);
  pinB_read = digitalRead(ENCODER_B);
  // for rising edge
  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  if((pinA_read==1) && (pinB_read==1) && BfirstUp) { 
    encoderPos --; //decrement the encoder's position count
    AfirstUp = 0; //reset flags for the next turn
    BfirstUp = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==1) && (pinB_read==0)) AfirstUp = 1; 

  // for falling edge
  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  if((pinA_read==0) && (pinB_read==0) && BfirstDown) { 
    encoderPos --; //decrement the encoder's position count
    AfirstDown = 0; //reset flags for the next turn
    BfirstDown = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==0) && (pinB_read==1)) AfirstDown = 1; 
}
// ISR for encoder reading: Channel B
void doEncoderB(){
  // reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  pinA_read = digitalRead(ENCODER_A);
  pinB_read = digitalRead(ENCODER_B);
  if ((pinA_read==1) && (pinB_read==1) && AfirstUp) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos ++; //increment the encoder's position count
    AfirstUp = 0; //reset flags for the next turn
    BfirstUp = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==0) && (pinB_read==1)) BfirstUp = 1; 

  // for falling edge
    //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  if((pinA_read==0) && (pinB_read==0) && AfirstDown) { 
    encoderPos ++; //decrement the encoder's position count
    AfirstDown = 0; //reset flags for the next turn
    BfirstDown = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==1) && (pinB_read==0)) BfirstDown = 1; 
}

void updateEncoderPosition() {
  if (encoderPos != lastEncoderPos) {
    Serial.print("Encoder position: ");
    Serial.println(encoderPos);
    lastEncoderPos = encoderPos;
  }
}