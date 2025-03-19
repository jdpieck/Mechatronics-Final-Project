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
// #set raw(lang: "c")
#set raw(lang: none)
#show raw.where(block: true): it => figure(it)
#set image(width: 75%)
#set rect(width: 100%)
#show rect: set align(left)
#let fig(path) = figure(image(path))

= Project Description

= Free-Time System (FTS)
#fig("diagram.svg")

= Project Circuit 
#fig("PadlockCircuit.png")

= Human-Machine Interface
== Setup 
== Idle 
== Manual
== Automatic
== Exit 

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


