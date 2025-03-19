#import "@local/oasis-align:0.2.0": *
#import "template.typ": *

#show: project.with(
  title: "Lab 5",
  class: "EME 154 A03",
  authors: "Jason Daniel Pieck",
  emails: "jdpieck@ucdavis.com",
  page-height: 10in,
  page-width: 7.5in,
)


= Project Description

= Free-Time System (FTS)
#fig("diagram.svg")

= Project Circuit 
#fig("PadlockCircuit.png")

// #set page(columns: 2)
= Human-Machine Interface
== Startup
```serial
══════ System Startup Initialized ══════
═══ System Successfully Initialized! ═══
```
== Setup 
```serial
EME-154 Mechatronics    Free Time System
Jason Daniel Pieck
****************************************
            MACHINE SETUP MENU
****************************************
 
 
 
 
****************************************
```

```serial
Input the initial dial position.
(2-digit) Combination Input: 88
Invalid Input! Input should be from 0-40

(2-digit) Combination Input: 24
Combination Number Accepted!

Dial Position Set to 24
```
== Idle 
```serial
EME-154 Mechatronics    Free Time System
Jason Daniel Pieck
****************************************
            OPERATION MENU
****************************************
1. Manual Operation
2. Automatic Operation
3. Machine Data Set-Up
5. Exit
****************************************
```
== Manual
```serial
EME-154 Mechatronics    Free Time System
Jason Daniel Pieck
****************************************
            MANUAL OPERATION MENU
****************************************
1. Move One Tick CW
2. Move One Tick CCW
 
5. Exit
****************************************
Manual Mode Accepted
Dial Position is at: 5
```

```serial
Manual Mode Accepted
Dial Position is at: 24

Moving 1 Tick CW
Dial Position is at: 25

Moving 1 Tick CW
Dial Position is at: 26

Moving 1 Tick CW
Dial Position is at: 27
```
== Automatic
```serial
EME-154 Mechatronics    Free Time System
Jason Daniel Pieck
****************************************
            AUTOMATIC MENU
****************************************
 
 
 
 
****************************************
Please enter the lock combination.
```

```serial
Please enter the lock combination.
~~~~ Combination 1 ~~~~
(2-digit) Combination Input: 10
Combination Number Accepted!

~~~~ Combination 2 ~~~~
(2-digit) Combination Input: 20
Combination Number Accepted!

~~~~ Combination 3 ~~~~
(2-digit) Combination Input: 30
Combination Number Accepted!


Moving to: 10

Moving to: 20

Moving to: 30

You can pull the shackle now!
Press # to Exit
```
== Exit 
```serial
═════════════ System Exit ══════════════
```


= System Development 
== Coding Accessability
=== Renaming Functions
=== Serial Monitor Messages
== Localizing Function Logic
== Git Version Management

= Encountered Challenges
== Backwards PWM Setup
== Memory Issues
== Motor Control Tuning
=== Re-Writing the ISR
=== Re-Writing `drivePulses`

= General Comments/Feedback

= Hardware Setup
#fig("wiring.jpg")


