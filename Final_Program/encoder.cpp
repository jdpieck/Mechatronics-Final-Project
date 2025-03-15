// #include <math.h>

// #define ENCODER_A 2
// #define ENCODER_B 3

// // encoder reading flags
// volatile byte AfirstUp = 0; 
// volatile byte BfirstUp = 0; 
// volatile byte AfirstDown = 0; 
// volatile byte BfirstDown = 0; 
// // CW when it increases, CCW when it decreases
// volatile int encoderPos = 0; //this variable stores our current value of encoder position. 
// volatile int lastEncoderPos = 0; //this variable stores our last value of encoder position. 
// volatile byte pinA_read = 0; // this variable is shared among interrupts
// volatile byte pinB_read = 0; // this variable is shared among interrupts

// // ISR for encoder reading: Channel A
// void doEncoderA(){
//   pinA_read = digitalRead(ENCODER_A);
//   pinB_read = digitalRead(ENCODER_B);
//   // for rising edge
//   //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
//   if((pinA_read==1) && (pinB_read==1) && BfirstUp) { 
//     encoderPos --; //decrement the encoder's position count
//     AfirstUp = 0; //reset flags for the next turn
//     BfirstUp = 0; //reset flags for the next turn
//   }
//   // Direction sensing
//   else if ((pinA_read==1) && (pinB_read==0)) AfirstUp = 1; 

//   // for falling edge
//   //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
//   if((pinA_read==0) && (pinB_read==0) && BfirstDown) { 
//     encoderPos --; //decrement the encoder's position count
//     AfirstDown = 0; //reset flags for the next turn
//     BfirstDown = 0; //reset flags for the next turn
//   }
//   // Direction sensing
//   else if ((pinA_read==0) && (pinB_read==1)) AfirstDown = 1; 
// }
// // ISR for encoder reading: Channel B
// void doEncoderB(){
//   // reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
//   pinA_read = digitalRead(ENCODER_A);
//   pinB_read = digitalRead(ENCODER_B);
//   if ((pinA_read==1) && (pinB_read==1) && AfirstUp) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
//     encoderPos ++; //increment the encoder's position count
//     AfirstUp = 0; //reset flags for the next turn
//     BfirstUp = 0; //reset flags for the next turn
//   }
//   // Direction sensing
//   else if ((pinA_read==0) && (pinB_read==1)) BfirstUp = 1; 

//   // for falling edge
//     //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
//   if((pinA_read==0) && (pinB_read==0) && AfirstDown) { 
//     encoderPos ++; //decrement the encoder's position count
//     AfirstDown = 0; //reset flags for the next turn
//     BfirstDown = 0; //reset flags for the next turn
//   }
//   // Direction sensing
//   else if ((pinA_read==1) && (pinB_read==0)) BfirstDown = 1; 
// }