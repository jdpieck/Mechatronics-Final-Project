#ifndef MOTOR_H
#define MOTOR_H

// extern volatile int encoderPos;
// extern volatile int lastEncoderPos;

void setMotorCW();
void setMotorCCW();
void driveMotor(int motor_speed);
void stopMotor();
void resetMotorControl(int pulses);
int dialDistanceToPulses(int deltaDials);
void doEncoderA();
void doEncoderB();
void updateEncorderPosition();


#endif