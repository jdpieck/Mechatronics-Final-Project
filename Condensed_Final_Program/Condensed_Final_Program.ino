#include <Keypad.h>
#include <math.h>

#define CLOCK_FREQUENCY 16000000
#define SERIAL_BAUDRATE 57600

#define ENCODER_A 2
#define ENCODER_B 3 

#define MOTOR_ENABLE 11
#define DIR 13
#define BRAKE 8
#define MAX_PWM 255
#define MIN_PWM 55 // this one depends on the dead zone of the motor input voltage
#define PPR 800   // 1080 is ideally, but there may exists some offset from your observation and sensor noise 

// interpolated P-control
#define KP 0.3 //0.12  // P control parameter
#define TARGET_DIST 100 // pulses
#define DIS2GO 5

int dist_moved = 0;
int controlLoopRate = 95;
int distanceToGo;    
int positionError=0;
int PWM_value;

const float dial_ticks = 40; // dial padlock resolution

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

/* KeyPad parameters */
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {7, 6, 5, 4}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {A5, A4, A3, A2}; // connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

/* FTS parameters */
byte OperationMode, lastOperationMode; // used to save the operation mode
volatile unsigned int MachineStatus, lastMachineStatus;
bool errorFlag; // a flag for error level 1 
bool runFlag; // a flag for the first running
char machineMessage[25], menuTitle[25], action1[25], action2[25], action3[25], action4[25];
int dial, last_dial; // dial position

/* Functions of Motor Control */
void setMotorCW() {
  digitalWrite(DIR, LOW);
}

void setMotorCCW() {
  digitalWrite(DIR, HIGH);
}

void driveMotor(int motor_speed) {
  digitalWrite(BRAKE, LOW); // release the brake before setting the speed
  motor_speed = min(motor_speed, MAX_PWM); // upper bound
  motor_speed = max(motor_speed, MIN_PWM);  // lower bound
  analogWrite(MOTOR_ENABLE, motor_speed);
}

void stopMotor() {
  digitalWrite(BRAKE, HIGH);   //Engage the Brake for Channel A
  analogWrite(MOTOR_ENABLE, 0);   
}

// after each robotation of the motor, reset the control parameter
void resetMotorControl(int pulses) {
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
int dialDistanceToPulses(int deltaDials) {
  // Serial.print("DEBUG: dialDistanceToPulses Triggered - deltaDails: "); Serial.println(deltaDials);
  int delta_pulses = deltaDials/dial_ticks*PPR;
  // Serial.print("DEBUG: delta_pulses: "); Serial.println(delta_pulses);
  return delta_pulses;
}

// ISR for encoder reading: Channel A
void doEncoderA() {
  pinA_read = digitalRead(ENCODER_A);
  pinB_read = digitalRead(ENCODER_B);
  // for rising edge
  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  if ((pinA_read==1) && (pinB_read==1) && BfirstUp) { 
    encoderPos --; //decrement the encoder's position count
    AfirstUp = 0; //reset flags for the next turn
    BfirstUp = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==1) && (pinB_read==0)) AfirstUp = 1; 

  // for falling edge
  //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  if ((pinA_read==0) && (pinB_read==0) && BfirstDown) { 
    encoderPos --; //decrement the encoder's position count
    AfirstDown = 0; //reset flags for the next turn
    BfirstDown = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==0) && (pinB_read==1)) AfirstDown = 1; 
}

// ISR for encoder reading: Channel B
void doEncoderB() {
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
  if ((pinA_read==0) && (pinB_read==0) && AfirstDown) { 
    encoderPos ++; //decrement the encoder's position count
    AfirstDown = 0; //reset flags for the next turn
    BfirstDown = 0; //reset flags for the next turn
  }
  // Direction sensing
  else if ((pinA_read==1) && (pinB_read==0)) BfirstDown = 1; 
}

// Timer Setup: frequency needs to be larger than 1 HZ
void setupTimer(int freq) {
  // set up Timer
  noInterrupts(); // stop interrupts
  
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  
  // freq: 1~3
  if(freq <= 3) { 
    // set compare match register for freq(<=3) Hz increments
    OCR1A = CLOCK_FREQUENCY/(256*freq)-1; 
    // Set CS12, CS11 and CS10 bits for 256 prescaler
    TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);
  }
  // freq: 4~30
  else if(freq <= 30) { 
    // set compare match register for freq(<=30) Hz increments
    OCR1A = CLOCK_FREQUENCY/(64*freq)-1; 
    // Set CS12, CS11 and CS10 bits for 64 prescaler
    TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  }

  // freq: 31~244
  else if(freq <= 244) { 
    // set compare match register for freq(<=30) Hz increments
    OCR1A = CLOCK_FREQUENCY/(8*freq)-1; 
    // Set CS12, CS11 and CS10 bits for 8 prescaler
    TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);
  }

  // freq: >=245
  else{ 
    // set compare match register for freq(<=30) Hz increments
    OCR1A = CLOCK_FREQUENCY/(1*freq)-1; 
    // Set CS12, CS11 and CS10 bits for 1 prescaler
    TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  }

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  interrupts(); // allow interrupts
}

// ISR of timer
ISR(TIMER1_COMPA_vect) {
  // calculate the position error
  if (abs(distanceToGo - encoderPos) < TARGET_DIST) {
    positionError = distanceToGo - encoderPos;
  }
  // positionError = set_distance - encoderPos;
  else{
    dist_moved = encoderPos - lastEncoderPos;
    if (distanceToGo > 0) {
      positionError += DIS2GO - dist_moved;
    }
    else{
      positionError += -DIS2GO - dist_moved; 
    }
  }
  // positionError = distanceToGo - encoderPos;
  PWM_value = (int)(KP*(float)positionError);
  // update the last_encoder reading
  lastEncoderPos = encoderPos;
}

// parameter is the distance in pulses we want the motor to drive to
void drivePulses(int distInPulses) {
  // get the sign of running distInPulses
  int dir_sign = (distInPulses > 0 ? 1 : -1);
  resetMotorControl(distInPulses);
  while(1) {

    // Serial.print("DEBUG: distInPulses: "); Serial.println(distInPulses);
    // Serial.print("DEBUG: PWM_value: "); Serial.println(PWM_value);
    // Serial.print("DEBUG: encoderPos: "); Serial.println(encoderPos);
      
    if (PWM_value > 0) {
      setMotorCCW();
    }
    if (PWM_value < 0) {
      setMotorCW();
    }
    // setMotorCCW();
    driveMotor(abs(PWM_value));
    if ((encoderPos - distInPulses)*dir_sign >= -1) {
      stopMotor();
      break;
    }
  }
}

// parameter is the distance in ticks we want the motor to drive to
void driveTicks(float ticks) {
  int pulses = dialDistanceToPulses(ticks);
  drivePulses(pulses);
  dial = dial + ticks; 
  if (dial >= dial_ticks) {
    dial = dial - dial_ticks;
  } else if (dial < 0) {
    dial = dial + dial_ticks;
  }
}

// int correctDistance(int distance) {
//   if (distance < 0) {
//     // Serial.println("Wrapping Value");
//     return distance + dial_ticks;
//   } else if (distance == 0) {
//     return dial_ticks;
//   } else {
//     return distance;
//   }
// }

/* move CCW to a dial position */
// input: dial position combo it will go to;
// output: the motor will rotate to that number CCW;
// calculate the incremental ticks it needs to go and use function 'driveTicks(ticks)' to implement
void moveCcwToDialNO(int combo)  {
  int ticksToMove = combo - dial; 
  
  if (ticksToMove > 0) {
    // Serial.println("Wrapping Value");
    ticksToMove = ticksToMove - dial_ticks;
  } else if (ticksToMove == 0) {
    ticksToMove = -1*dial_ticks;
  } 

  // Serial.print("DEBUG: Current Location: "); Serial.println(dial);
  // Serial.print("DEBUG: Distance to Go: "); Serial.println(ticksToMove);
  driveTicks(ticksToMove);
}

/* move CW to a dial position */
// input: dial position combo it will go to;
// output: the motor will rotate to that number CW;
// calculate the incremental ticks it needs to go and use function 'driveTicks(ticks)' to implement
void moveCwToDialNO(int combo)  {
  int ticksToMove = combo - dial;
  
  if (ticksToMove < 0) {
    // Serial.println("Wrapping Value");
    ticksToMove = ticksToMove + dial_ticks;
  } else if (ticksToMove == 0) {
    ticksToMove = dial_ticks;
  } 
  
  // Serial.print("DEBUG: Current Location: "); Serial.println(dial);
  // Serial.print("DEBUG: Distance to Go: "); Serial.println(ticksToMove);
  driveTicks(ticksToMove);
  
}

/* Functions of Free Time System  */ 
// FTS initialization function
void InitializeSupervisor() {
  // initailize parameters
  OperationMode = 0;
  lastOperationMode = 0;
  lastMachineStatus = 0;
  errorFlag = false;
  runFlag = true;
}

// Diagnosis function
int DiagnosticsSupervisor() {
  // put the diagnostics program here
  if (errorFlag) {
    Serial.println("Error Detected!!");
    return 0; // for error
  } else {
    return 1; // successful diagnostics
  }
}

// set idle menu
void setIdleMenu() {
  String str1, str2, str3, str4, str5;
  str1 = "OPERATION MENU";
  str2 = "1. Manual Operation";
  str3 = "2. Automatic Operation";
  str4 = "3. Machine Data Set-Up";
  str5 = "5. Exit";
  str1.toCharArray(menuTitle,25);
  str2.toCharArray(action1, 25);
  str3.toCharArray(action2, 25);
  str4.toCharArray(action3, 25);
  str5.toCharArray(action4, 25);
}

// set Manual Menu
void setManualMenu() {
  String str1, str2, str3, str4, str5;
  str1 = "MANUAL OPERATION MENU";
  str2 = "1. Move One Tick CW";
  str3 = "2. Move One Tick CCW";
  str4 = " ";
  str5 = "5. Exit";
  str1.toCharArray(menuTitle,25);
  str2.toCharArray(action1, 25);
  str3.toCharArray(action2, 25);
  str4.toCharArray(action3, 25);
  str5.toCharArray(action4, 25);
}

// set Automatic menu
void setAutomaticMenu() {
  String str1, str2, str3, str4, str5;
  // fill str1~5 to have the Automatic manual print out correctly
  str1 = "AUTOMATIC MENU";
  str2 = " ";
  str3 = " ";
  str4 = " ";
  str5 = " ";
  str1.toCharArray(menuTitle,25);
  str2.toCharArray(action1, 25);
  str3.toCharArray(action2, 25);
  str4.toCharArray(action3, 25);
  str5.toCharArray(action4, 25);
}

// set Machine setup menu
void setSetupMenu() {
  String str1, str2, str3, str4, str5;
  str1 = "MACHINE SETUP MENU";
  str2 = " ";
  str3 = " ";
  str4 = " ";
  str5 = " ";
  str1.toCharArray(menuTitle,25);
  str2.toCharArray(action1, 25);
  str3.toCharArray(action2, 25);
  str4.toCharArray(action3, 25);
  str5.toCharArray(action4, 25);
}

void printHeaderAndMenu() {
  char spacer[41] = "****************************************";
  Serial.println("\nEME-154 Mechatronics    Free Time System");
  Serial.println("Jason Daniel Pieck");
  Serial.println(spacer);
  Serial.print("            "); 
  Serial.println(menuTitle);
  Serial.println(spacer);
  Serial.println(action1);
  Serial.println(action2);
  Serial.println(action3);
  Serial.println(action4);
  Serial.println(spacer);
  // delay(10); // wait for printing
}

// get input digits from the KeyPad
int getCombination(char *arr) {
  char key = customKeypad.getKey();
  int dialPosition;
  int inputFlag = 1;

  while(inputFlag) {
    Serial.print("(2-digit) Combination Input: ");
    
    while(!key) {
      key = customKeypad.getKey();
    }

    arr[0] = key;
    Serial.print(key);
    key = customKeypad.getKey();
    
    while(!key) {
      key = customKeypad.getKey();
    }

    // get zero position
    arr[1] = key;
    Serial.print(key);
    key = customKeypad.getKey();
    dialPosition = atoi(arr);
    delay(300);

    if (dialPosition >= 0 && dialPosition <= dial_ticks) {
      Serial.println("\nCombination Number Accepted!\n"); 
      // Serial.println(); 
      inputFlag = 0;
      return dialPosition;
    }
    
    else{
      Serial.print("\nInvalid Input! Input should be from 0-"); Serial.println((int)dial_ticks);
      Serial.println();
      // Serial.println("Please try again.\n");
    }
  }
}

// Machine setup function
void MachineSetup() {
  char numbers[2];

  Serial.println("Input the initial dial position.");
  dial = getCombination(numbers);
  Serial.print("Dial Position Set to "); Serial.println(dial);

  setIdleMenu();
  printHeaderAndMenu();
  OperationMode = 0;
}

// Mode print functions
// char ModePrint() {
//   switch (OperationMode) {
//     case 1:
//       Serial.println("Manual");
//       break;
//     case 2:
//       Serial.println("Automatic");
//       break;
//     case 3:
//       Serial.println("Machine Setup");
//       break;
//     case 5:
//       Serial.println("System Turned Off");
//       break;
//     default:
//       Serial.println("Idle Mode");
//   }
// }

// Output Control Supervisor
void OutputControl() {
  if (runFlag) {
    runFlag = false;//Clear first run flag
    setSetupMenu();
    printHeaderAndMenu();
    MachineSetup();
  }
  else if (lastOperationMode != OperationMode) {
    lastOperationMode = OperationMode;
    Serial.println(machineMessage);
    // Serial.print("Current Mode: "); 
    // ModePrint();
    Serial.print("Dial Position is at: ");
    Serial.println(dial);
  }

}

// MSS: Machine Status Scan Supervisor
void MachineStatusScan() {
  String str;
  if (runFlag) OutputControl();   //skip this for the first run
  // Serial.println("Running MachineStatusScan");

  char selection = customKeypad.getKey(); // get a key from the keypad
  if (selection) {
    // Serial.print("DEBUG: Key Selected: ");    Serial.println(selection);
    
    switch (selection) {
      // manual operation mode
      case '1':
        if (OperationMode == 1) { //in manual mode
          // Serial.println("DEBGUG: Manual Mode Active - Selected 1");
          ManualOperation(selection);
        }
        else if (OperationMode == 0) {//in idle mode
          OperationMode = 1;
          setManualMenu();
          str = "Manual Mode Accepted";
          str.toCharArray(machineMessage,25);
          printHeaderAndMenu();
        }
        break;
      // automatic operation mode 
      case '2': //this key will...
        if (OperationMode == 1) { //in manual mode
          // Serial.println("DEBGUG: Manual Mode Active - Selected 2");
          ManualOperation(selection);
        }
        else if (OperationMode == 0) {//in idle mode
          OperationMode = 2;
          setAutomaticMenu();
          printHeaderAndMenu();
          str = "Automatic Mode Accepted";
          str.toCharArray(machineMessage,25);
        }
        break;
      case '3': //this key will...
        if (OperationMode == 1) { //in manual mode
          // Serial.println("DEBUG: Manual Mode Active - Selected 3");
          ManualOperation(selection);
        }
        else if (OperationMode == 0) {//in idle mode
          OperationMode = 3;
          setSetupMenu();
          printHeaderAndMenu();
        }
        break;
      // Exit
      case '5':
        if (OperationMode == 0) {
          OperationMode = 5;
          str = "Exit Request Accepted";
          str.toCharArray(machineMessage,25);
        }
        else {
          setIdleMenu();
          printHeaderAndMenu();
          OperationMode = 0;
        }
      default:
        break;
    }
  }
}

// MCS: Mode Control Supervisor 
void ModeControl() {
  if (OperationMode==1) ManualOperation(0); // do nothing
  if (OperationMode==2) AutomaticControl();  
  if (OperationMode==3) MachineSetup();
}

// automatic lock opener
void AutomaticControl() {
  int combo1 = -1, combo2 = -1, combo3 = -1, task = 0;
  char number[2];
  // char key;
  // String str;

  while(OperationMode ==2) {
    Serial.println("Please enter the lock combination.");
    Serial.println("~~~~ Combination 1 ~~~~");
    combo1 = getCombination(number);
    Serial.println("~~~~ Combination 2 ~~~~");
    combo2 = getCombination(number);
    Serial.println("~~~~ Combination 3 ~~~~");
    combo3 = getCombination(number);
        
    //Move to first location
    Serial.print("\nMoving to: "); Serial.println(combo1);
    // move 1 circle CW with moveCwToDialNO()
    moveCwToDialNO(dial);
    delay(500);

    // move 1 circle CW with function moveCwToDialNO()
    moveCwToDialNO(dial);
    delay(500);
    
    // move to the 1st number CW with function moveCwToDialNO()
    moveCwToDialNO(combo1);
    delay(500);
    
    //Move to second number
    Serial.print("\nMoving to: "); Serial.println(combo2);
    // move 1 circle CCW with function moveCcwToDialNO()
    moveCcwToDialNO(dial);
    delay(500);
    // move to the 2nd number CCW with function moveCcwToDialNO()
    moveCcwToDialNO(combo2);
    delay(500);
    
    
    // Move to third number
    Serial.print("\nMoving to: "); Serial.println(combo3);
    
    // move to the 3rd number CW with function moveCwToDialNO()
    moveCwToDialNO(combo3);
    delay(500);

    //pop the lock with the solenoid connected to port 1
    Serial.println("\nYou can pull the shackle now!");

    // update machine message
    
    // prompt the user to input '#' to go back the main menu

    // go back to the idle mode
    setIdleMenu();
    printHeaderAndMenu();
    OperationMode = 0;
  }
}

// MOS: Manual Operation Supervisor
void ManualOperation(char selection) {
  // Serial.println("DEBUG: Manual Operation Function Called");
  String str;
  if (selection) {
    // Serial.print("DEBUG: Manual Mode Input: "); Serial.println(selection);

    str = "Manual Idle Mode";
    str.toCharArray(machineMessage,25);
    switch (selection) {
      case '1': 
        str = "Manual Active Mode";
        str.toCharArray(machineMessage,25);
        Serial.println("\nMoving 1 Tick CW");
        driveTicks(1); //move 1 tick CW decrease
        // dial++;
        // if (dial == dial_ticks) dial = 0;
        break;
        
      case '2': 
        str = "Manual Active Mode";
        str.toCharArray(machineMessage,25);
        Serial.println("\nMoving 1 Tick CWW");
        driveTicks(-1); //zero is CCW
        // dial--;
        // if (dial ==-1) dial = dial_ticks-1;
        break;
      }
      Serial.print("Dial Position is at: ");
      Serial.println(dial);
  }
}

// ERT
void ErrorTreatment() {
  // take actions here to treat detected errors
}

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("");
  Serial.println("══════ System Startup Initialized ══════");

  // pin mode for encoder A, B
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  // set up interrupt for Pin A, B
  attachInterrupt(0, doEncoderA, CHANGE); 
  attachInterrupt(1, doEncoderB, CHANGE); 
  //Motor pins
  pinMode(MOTOR_ENABLE, OUTPUT); // speed with PWM output
  pinMode(DIR, OUTPUT);   // direction pin
  pinMode(BRAKE, OUTPUT);   // brake pin
  digitalWrite(DIR, LOW); //Establishes forward direction of Channel A
  digitalWrite(BRAKE, LOW);   //Disengage the Brake for Channel A
  // setup timer: given frequency for the motor controling
  setupTimer(controlLoopRate);

  // Initialization of FTS
  InitializeSupervisor();
  Serial.println("═══ System Successfully Initialized! ═══");
  Serial.println("");
}

void loop() {
  // the main FTS
  if (DiagnosticsSupervisor()) {
    // if diagnostics succeed
    MachineStatusScan();
    ModeControl();
    OutputControl();
  } else {
    // go to ERT if diagnostics fail
    ErrorTreatment();
    OutputControl();
  }
  // if press the button 5, stop the system
  if (OperationMode == 5) {
    Serial.println("\n═════════════ System Exit ══════════════");
    while(1);
  }
} 


