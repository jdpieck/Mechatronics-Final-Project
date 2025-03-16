---
Files & media:
  - "[[EME154 Project Padlock Opener.pdf]]"
---
## Project Tasks
1. [ ] Project Description: summary of the project and functionality of each mode
2. [ ] Description of the FTS: Description of each component of the FTS for the project
3. [ ] Project Circuit: Show a clear diagram of the circuit.
4. [ ] Human-Machine Interface (Serial Monitor and LCD): Show a picture of the human-machine interaction and description.
    1. [ ] Menu for each mode
    2. [ ] Input and output interface.
5. [ ] System development: detailed description of the code development of the project
6. [ ] Explain challenges encountered
7. [ ] General comments/feedback
8. [ ] Attach a picture of the final hardware setup
9. [ ] Attach completed codes  
    - [ ] EME154S24_FinalProject_FirstName_LastName.ino
## Personal Task
- [x] Fix encoder distance per pulse
## Nice to Have
- [ ] Display current location on setup
- [ ] have live number entry
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
## [[Gutted Code]]