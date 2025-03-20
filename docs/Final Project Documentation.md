---
Files & media:
  - "[[EME154 Project Padlock Opener.pdf]]"
---
# Overview
## Canvas Tasks
1. [x] Project Description: summary of the project and functionality of each mode
2. [x] Description of the FTS: Description of each component of the FTS for the project
3. [x] Project Circuit: Show a clear diagram of the circuit.
4. [x] Human-Machine Interface (Serial Monitor and LCD): Show a picture of the human-machine interaction and description.
    1. [x] Menu for each mode
    2. [x] Input and output interface.
5. [x] System development: detailed description of the code development of the project
6. [x] Explain challenges encountered
7. [x] General comments/feedback
8. [x] Attach a picture of the final hardware setup
9. [ ] Attach completed codes  `EME154S24_FinalProject_FirstName_LastName.ino`
## Document Task
1. [x] Build the circuit given the drawings we provided. 
2. [x] Evaluate the motor and encoder based on the contents in Lab 5/Lab 6 and showing how many pulses you can get with your detection method. 
3. [x] We provide the interpolated P control for the motor and the student need to try different numbers of P to get the motor response curves. They can print the encoder position into the Serial monitor and use Excel to plot the positions. After tuning P well, update the parameter into the code. 
4. [x] Finish the code of incomplete FTS to implement the whole designed functionality. 
5. [x] Draw the general flowchart of the FTS (Free Time System) of APOC by adding/deleting the modules to/from the comprehensive one that was explained in Lab 4. 
6. [x] Explain the types of interrupts they used in the system. G. Draw the flow chart for the user manual in different operation modes. 
7. [x] *Extra points: PI control implementation in the system*
## Demonstration
1. Introduction of the system modules. 
2. Motor control: 
	1. Evaluation of motor and encoder. 
	2. P-control tuning process of the motor.
3. Human-machine interface design in each step of the flow chart. 
4. User manual to process the system. 
5. Developing process of the whole system. 
6. Demonstration of various operation modes
## Personal Tasks
- [x] Fix encoder distance per pulse
- [ ] Update Header 3
- [ ] Check if we need to redraw circuit diagram 
# Development
## Extra Functionality
- [ ] Display current location on setup
- [x] have live number entry
## Questions
- [ ] Ask if operation needs to be mirrored
- [x] Ask if we are doing 40 tick or 60 tick? 
## Overview
- `AutomaticControl` found on line `540`
- `#define PPR` is a tuning parameter
## Documentation
- Renamed functions for improved legibility
- fixed bug with PWM being backwards
	- talk about debugging process
- added startup messages
- ran into memory leak issue
	- had to basically restart 
	- `dialPos = getCombination(numbers);`
## Thinking
- from 10 to 30
	- CW = 30 - 10 = 20
	- CWW = 10 - 30 = -20 
- from 5 to 10
	- CW = 10 - 5 = 5
	- CWW = 5 - 10 + 40 = 35
- from 15 to 10
	- CW = 10 - 15 + 40 = 35
	- CWW = 15 - 10 = 5
## [[Gutted Code]]
## [[Debug Messages]]