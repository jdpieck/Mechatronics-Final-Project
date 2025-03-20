#import "@local/oasis-align:0.2.0": *
#import "template.typ": *

#show: project.with(
  title: "Final Project Report",
  class: "EME 154 A03",
  authors: "Jason Daniel Pieck",
  emails: "jdpieck@ucdavis.com",
  page-height: 10in,
  page-width: 7.5in,
)


= Project Description
The Automatic Padlock Opener Control (APOC) is a free-time system (FTS) designed to mount to and open a pad lock. 

After initializing the system by entering the starting position, the user can use the manual mode to manually enter in the lock combination. Alternatively, the user can also select the automatic mode in which they can enter the three combination numbers onto the keypad and let the system enter the combination. 

If at any point the user would like to reinitialize the system, they are able to do so through the setup menu. One the user is finished, they can then exit the system. 


= Free-Time System (FTS)
The free time system is modeled as shown in the @diagram.  
#fig("diagram.svg") <diagram>

Based on the frequency determined in the clock setup, the system runs through the entire system, activating different modules based on user inputs and state information. 

// The

= Project Circuit 
The following circuit diagram seen in @circuit was provided to us in the project documentation. The hardware setup can be seen in @hardware.
#fig("PadlockCircuit.png") <circuit>

// #set page(columns: 2)
= Human-Machine Interface <hmi>
The entire Human-Machine Interface (HMI) exists through the serial monitor. This means that in order for user to get feedback and instructions from the system, the Arduino must be connected to the Arduino IDE.  

== Startup
When the system first powers on, it sends the following message.
```serial
══════ System Startup Initialized ══════
═══ System Successfully Initialized! ═══
```
This helps the user understand when a new session is started. 

== Setup 
Once the system has been initialized, it will display the following menu.
```serial
EME-154 Mechatronics    Free Time System
Jason Daniel Pieck
****************************************
            MACHINE SETUP MENU
****************************************
 
 
 
 
****************************************
```
This is immediately followed by a prompt asking the user to enter in the starting position. In the case the the user enters in a position that is outside the dial range, the system will throw and error, and inform the user what the range of the dial is. 

One a dial position within range is entered, the system send a confirmation message, and sets the system position to the entered position. 

```serial
Input the initial dial position.
(2-digit) Combination Input: 88
Invalid Input! Input should be from 0-40

(2-digit) Combination Input: 24
Combination Number Accepted!

Dial Position Set to 24
```

Please not that the logic used here to determine whether a combination is in range is the same as logic used in the Automatic Mode, and will send the user the same messages when an out-of-range combination is entered. 

== Idle 
Immediately after the user has entered an in-range combination, the following menu is displayed. From this menu, the user is able to access the rest of the system functionality.
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
Once manual mode is selected, the system displays the following menu.
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
Dial Position is at: 24
```
When the user selects `1` or `2` on the keypad, the system will physically respond, and inform the user of the direction of motion as well as the dial position.  
```serial
Moving 1 Tick CW
Dial Position is at: 25

Moving 1 Tick CW
Dial Position is at: 26

Moving 1 Tick CWW
Dial Position is at: 25
```
If the user selects `5`, the system will exit and return to the Idle Menu.
== Automatic
Upon selecting Automatic Mode, the following menu is displayed. 
```serial
EME-154 Mechatronics    Free Time System
Jason Daniel Pieck
****************************************
            AUTOMATIC MENU
****************************************
 
 
 
 
****************************************
Please enter the lock combination.
```
This is immediately followed by a series of prompts for each combination number. Each time, the system check the combination number to ensure that it is in-range using the same logic as in the Setup Mode. 

```serial
~~~~ Combination 1 ~~~~
(2-digit) Combination Input: 10
Combination Number Accepted!

~~~~ Combination 2 ~~~~
(2-digit) Combination Input: 20
Combination Number Accepted!

~~~~ Combination 3 ~~~~
(2-digit) Combination Input: 30
Combination Number Accepted!
```
After all three combinations have been accepted, the system then moves through each combination along with the added rotations for the first and second combination. 

Once complete, the user is prompted to exit the automatic mode by pressing `#` on the keypad. This will return the user to the Idle Menu. 
```serial
Moving to: 10

Moving to: 20

Moving to: 30

You can pull the shackle now!
Press # to Exit
```
== Exit 
If the user ops to exit the system from the Idle Menu, the following message will be displayed. 
```serial
═════════════ System Exit ══════════════
```


= System Development 
This system was developed over the course of a two week period after acquiring hardware. 

The system was built on top of the already provided code for the project, though large parts of the code base have been rewritten. This allowed me to focus my time on the development of the Automatic Mode and tuning the motor control parameters, PI control included. 

== Coding Accessability
Since code is constantly under revision, I took several measure to enure code readability to decrease the total time debugging. 

=== Renaming Functions
Before beginning work, I went through and renamed the FTS functions to their full, unshortened names. This allowed me to more quickly identify what a particular function's goal was, as I did not need to reference a key. 

This practice was also applied to other functions across the code base to better reflect code functionality and purpose. 

=== Serial Monitor Messages <serial>
While testing specific parts of the FTS, I made heavy use of Serial Monitor messages to provide insight to code operations. Not only was this useful in determining function outputs, but also when functions were being triggered.

== Localizing Function Logic
In many instances while writing the code base, I realized that being near duplicated. This meant lots of copy-pasting while making changes, and increased the odds for accidental bugs.

A great example of this was the logic used to determine whether a user-entered combination was within the dial limit. Initially, this code was present once in the Setup Function, and three separate times in the Automatic Function. 

By combing it all into the a single function `getCombination()`, I was able to use the same function in four different places across the code base, letting me make modifications and adjustments with ease. 

== Git Version Management
After running into some trouble with code development, I begun using Git. After successfully implementing a new feature or change to the system, I would create a commit. 

This allowed me to have set return points for the system should the code stop functioning as expected. This also made it easier to keep track of my changes over time, increasing my capacity to perform trouble shooting, and also making documentation a straight forward process. 

You can view the full repository #link("https://github.com/jdpieck/Mechatronics-Final-Project")[here].

= Encountered Challenges
While I encountered and overcame a wide range of challenges during the development of this system, listed below are some of the more notable ones. 

== Backwards PWM Setup
After uploading the provided starting code for the system, I noticed while  rotating the dial in Manual Mode, the system would not stop. Using Serial messages to debug as discussed in @serial, I was able to determine the the stop trigger for the motor in `drivePulses()` was not being called. 

It turned out that the stop condition based on whether the PWM was positive or negative was swapped such that it would never trigger. 

After swapping the comparison operators, Manual Mode was functional. 

== Memory Issues
On my second day of development, I noticed that the menus discussed in @hmi would start disappearing seemingly at random. Initially, I suspected that the menu functions were not being called properly, but that was not the case.

Instead, it turns out that the Arduino was running out of system memory. 

== Motor Control Tuning
=== Re-Writing the ISR
=== Re-Writing `drivePulses()`

= General Comments/Feedback

= Hardware Setup <hardware>
#fig("wiring.jpg")


