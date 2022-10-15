# AS7 On-Board Software User Guide

## Table of Contents

[toc]

## Outline

// What this contains

// what sort of software do we use -- e.g. mission planner

// the configuration for the drone

// assumption -- this iS SPECIFICALLY FOR AS7



## Before flight

* Contains operating features, concerns for data gathering, and why do one thing or another things

### Operating Features

* The remote, its controls, and the specific channel guides
* Battery, charging, and correct mounting
* Hardware inspections and checks
* Pre flight checklist

#### Remote Control System

* The control system is composed of the remote control transmitter (the tx) and the receiver on the drop (rx)
* The control has the following channels
  * channel descriptions
  * There are channels on the sbus specification that aren't being used
  * channels can be configured to this and that
  * 

#### Drone Battery and Charging

goes over the parts of the system that needs to be charged

##### Charging the Remote

using micro B

##### Charging the Drone Batteries

Using the multifunctional kit

##### Drone Battery Specifications

* the drone batteries must be between 4S-6S Poly
* Ideally above a certain C value



#### Recommended Hardware Inspection

* the drone hardware should be chcked and tightened before each flight
* all the drone uses M2 and M2.5 allen keys
* the propellers should be mounetd in the following configuration

#### The Pre-Flight Checklist

the drone goes through a pre flight checklist before opreation, however, many of these have been disable for use in underground and gps denied environments

It should be noted that the drone accelerometer and drone compass are independent of the ones present on the flight controller. Thus, the drone is capable of flight without the compass and accelrometer modules attached to the main pcb. as they are used for data collection, you won' be able to gather that specific data without the module present.



### Data Gathering and Hardware Features

* SD card set up
* US sensors and their details
* Compass and accel details

The main PCV mainly focuses on data gathering and drone control. Data gathered from the drone is written  onto the Sd card in batches 

The data is gathered from the following sources:

* the ultrasonic sensors
* the accelerometers
* the compass
* the internal clock from the esp32

### Arming and Flight set up

After all pre flight checks are done, the drone can be set down, armed, and then flown.

The process is as follows:

1. Strap the battery securely onto the donr but do not flug it in
2. Place the drone down with the signal lights facing rear



#### The Remote Interlock

As part of the safety feature set, the drone will not allow arming unless a remote control is detected. As such, the drone can be safely interacted with if the remote control (the transmitter unit) is off.



## During Flight

this section mostly details with things that could happen in flight, how do what the lights mean, and how to get out of flight

### Flight modes

the drone has primary modes --- automated mode and operator override.

#### Automated Flight

In automated mode, the drone will follow its internal instructions and aim to execute them. these are loaded on drone start-up.

Drone commands are enqueued on an internal stack and are executed first in last out. Dorne commands are composed of the following:

* A drone comamnd type, typically blind, guided, or arming
* A duration dor the command
* Drone command parameters.



#### Drone Command Parameters

The drone has 6 degrees of freedom:

* X (Forward/backward)
* Y (Sideways)
* Z (Up/Down)
* Pitch (Point Nose up/down)
* Roll (Sway Side to Side)
* Yaw (Rotate/Turn)

On a quadcopter, pitch and roll are not directly controllable. For each axis, A drone command has parameter for position and velocity. Thus there are 12 parameters, of which 4 are not directly controllable.

The command parameters are used differently depending on the type of the command

#### Blind Commands

Blind commands represent a configuration of the remote control. the blind commands only use velocity parameters, and map directly to the remote control.

For example, setting a `droneCommand.v_x` to `0.2f` represents the right hand side forward stick at the 20% position.

#### Guided Commands

Guided commands represent a guided operation of the drone using the on-board instruments. The current implementation of guided commands use the four available parameters for tunnel-based operation.

* In the X (forward) axis, the drone operates on blind velocity control, representing a blind movement forward
* In the Y (sideways) axis, the drone aims to avoid obstacles using the left and right ultrasonic sensors for collision detection. The sideways input is a weighted average of the two values, representing a tendency to balance both measurements. No command parameter is used for this at current.
* In the Z (upwards) axis, the drone aims to hold a position about the ground through PID control. The command parameter uses `droneCommand.p_z` in centimetres. The vertical thrust of the system is adjusted based on the measurement of the bottom sensor.



#### Operator Override Mode

In operator override mode, the drone will not respond to internal commands and will be directly controllable as per normal drone oepration.

In this mode, the PCB and main microcontroller pass through signals from the receiver directly 



#### Emergency Stop

In an emergency situation, the drone can be stopped using the provided emergency stop button. In the current implementation, this function is bound to the central button on the remote control. 

When pressed and held, the emergency stop will interlock drone command, setting all output channels to zero and disallowing further commands. The drone will then move to a faulted state, in which it will not respond to any further commands. Only pressing the reset button on the main PCB will release the emergency stop.

it should be noted that the drone may not stop its propellers under emergency stop.





### Communication Signals (Lights)







### Data gathering Details

mention base software here

### Automated Flight Details







## Software Features



Automated flgiht ramping and details

* this details the flight and communication system

Logging and data capture 

SBUS details

Communication protocols

SD configuration





## Maintenance 



General details on keeping drone clean

Notes on specific fragile parts

### Disassembly

How to remove props

How to remove FC

How to remove other thigns

### Manufactured Parts

Details on 3D printed parts and the files (physically provide them in the repo)

Specific material proeprties and alternatives







### Drone Specifications

Just a big list on the drone specs (like in drone details)





