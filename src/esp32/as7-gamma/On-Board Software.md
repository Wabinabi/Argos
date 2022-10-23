# AS7 On-Board Software User Guide

## Outline

The on-board software refers to the program flashed onto the ESP32 Microcontroller present on the AS7 Electronics and Sensor Package. This is located underneath the drone. 

This document provides an overview of the primary features on the drone. It includes the process before a flight, during, and after, alongside software configuration.

The drone will come with a software configuration already loaded onto the ESP32. Even if development is not required, the [Arduino IDE](https://www.arduino.cc/en/software/) is recommended to interface with the Serial transmissions from the drone for fault diagnostics.

## Table of contents

- [AS7 On-Board Software User Guide](#as7-on-board-software-user-guide)
  - [Before flight](#before-flight)
  - [During Flight](#during-flight)
  - [After Flight](#after-flight)
  - [Maintenance](#maintenance)

<a id="before-flight"></a>

## Before flight

Details of the systems associated with flight.

### Remote Control System

The Remote Control (RC) system is consists of a transmitter and receiver. The transmitter is the handheld ‘remote control’ and is also known as the “TX”. The Receiver is located on the drone located above the communications lights and is known as the “RX”. 

This system uses the SBUS communication protocol to transfer channel information from the receiver to the flight controller. Though SBUS can support up to 16 channels, 8 are used for communication with the drone. They are:

```C++
#define CH_THROTTLE     2   // Left stick y axis (starts from 0)
#define CH_YAW          3   // Left stick x axis
#define CH_STRAIGHT     1   // Right stick y axis
#define CH_STRAFE       0   // Right stick x axis
#define CH_BUTTON1      6   // Button on middle of controller
#define CH_SW1          7   // Right toggle switch
#define CH_FLIGHTMODE   4   // Left toggle switch
#define CH_ESTOP        6   // Other button?
```

The remainder are not used but are generally set to 50% of the channel width (about 1024). SBUS values are unitless, and the controller emits and receives values from 0 to 2056, though the channel can allow for 2^15 values. Internally, the on-board software uses `int_16t` or `word` for each channel.

Each channel can be configured independently through the drone configuration file to have a differing minimum and maximum range. An example configuration (if implemented), would be:

```
SBUS_RX_CH_1_MIN:100
SBUS_RX_CH_1_MAX:2000
```

The channel range would then be between the minimum and maximum values. For example, a 50% value for the above configuration would be (2000 - 100) * 0.5 + 100 = 1150.

Channels can also be configured to be absolute. Typically, the floating-point range of a channel is between [-1.0f, 1.0f], where 0.0f represents 50% of the channel value. This is analogous to the right stick on the remote control, where the default resting position is in-between the axes. The only exception is the thrust channel, which the default resting position is 0%. These are coined *absolute channels*, and have a range of [0.0f, 1.0f. If implemented, the channel configuration would be:

```
SBUS_RX_CH_1_ABS:TRUE
```

Channels 9-16 are not recommended for use as they fall outside of the scope of the controller. Additionally, these auxiliary channels are currently used for filtering incomplete or offset data frames. As the channels are typically not used, an incomplete or offset data frame will produce a very different result from the standard value of the channel (around 1024), allowing for the filtering of only complete frames. The checks that these channels are around the 1000 value for use, if not, the frame is dropped.

### Drone Battery and Charging

The drone is primarily powered by the Lithium Polymer (Li-Po) battery strapped on the 'back' of the drone. The battery supplies power to both the drone and the electronics package underneath the drone. Commonly used in racing drones, the the drone requires a 4-cell or 6-cell Li-Po battery to function. These are normally referred to as '4S' or '6S' cells. The battery will require an XT-60 connector to power the drone.

Calibration should not be required when changing between 4S and 6S batteries, however, it should be noted that 6S batteries carry significantly more voltage than 4S models. At 3.7v per cell, a 4S battery is 14.8v whereas a 6S battery is 22.2v.

The voltage of the battery is dropped to 5v with the on-board voltage regulator, and dropped to a further 3.3v on the ESP32. There are both 5v and 3.3v rails on the electronics package if additional components are required. 

##### Charging the Drone Batteries

The drone batteries are charged with a balance or multi-functional charger for Li-Po batteries. Supplied with the drone is a ToolkitRC M6 Multifunctional Charger with a Refurbished Toshiba PA5114E-1AC3 Power Supply connected to an XT-60 adaptor. Further resources on using the Toolkit M6 is provided in the box.

**Under no circumstances should the power supply be directly connected to the battery!** This may result in serious injury and Li-Po battery fires!

##### Charging the Remote

The remote control (Tx) is charged through a Micro-B USB Port on the right hand side of the device, near the screen. 

### Recommended Hardware Inspection

1. Inspect the Drone Frame for any damage
2. Inspect the 3D printed parts for any damage. Ensure that the locking bolts on all four drone legs are pushed in as far as they can or held in securely with a nut.
3. Tighten all nuts on the drone to ensure that none are loose. They can be tightened using a 2mm or 2.5mm Allen Key.
4. Ensure that there are no loose wires in the main electronics package. 
5. Follow the wires from the package and ensure that they are connected to the following modules:
   1. The communication lights
   2. The Flight Controller
   3. The Compass and Accelerometer Modules
   4. All 6 Ultrasonic Sensors

6. Ensure the propellers are securely mounted onto the motors. The propellers can be removed using a 6mm Hex Socket



The propellers should be mounted in the following layout:

<center><img src="https://ardupilot.org/copter/_images/motororder-quad-x-2d.png" alt="Drone Propeller Configuration" width="400" /></center>

#### The Pre-Flight Checklist, External Compass, and Accelerometer

Conventional drones complete a pre-flight checklist before operation, however, this is not the case with AS7. Given the GPS-denied nature of the drone, these interlocks have been disabled. Thus the only requirement for arming is that a battery and motors be present.

The electronics package contains an accelerometer and compass, in addition to the ones mounted on the flight controller. These operate independently of the flight system and are not required for flight. They are, however, used for data collection and position estimation for the 3D point cloud.

### Data Gathering and Hardware Features

The AS7 drone houses an electronics package for collecting mapping data of the environment. On-board sensors collect and store the data onto the MicroSD card located on the rear of the drone.

#### Setting up the MicroSD Card

The MicroSD Slot uses the Adafruit `MicroSD card breakout board+`. This requires the SD Card to be formatted to a FAT16 or FAT32 format. The recommended formatter is provided by the SD Foundation and can be found [here](https://www.sdcard.org/downloads/formatter/).

#### On-Board instrumentation

The electronics package contains several sensors and modules for data collection and storage. The primary sensors on the drone are:

* 6 Ultrasonic Sensors
* An SD Module
* An I2C Accelerometer
* An I2C Compass

Data is gathered continuously on the drone and is written to the SD card in batches, typically once every ultrasonic cycle. An ultrasonic cycle is the amunt of time taken for all 6 ultrasonic sensors to gather dat in sequence. As the ultrasonic sensors can interfere with each other in an enclosed space, each ultrasonic sensor gathers data independently, with an approximate 80ms delay between each sensor to ensure that no false signals are recorded. Thus, data is pushed to the SD card every 600-700ms. 

Each ‘batch’ is pushed at the end of the ultrasonic cycle. However, data from the accelerometer and compass are collected between batches and updated internally. At the end of the ultrasonic cycle, the the current value for all sensors is gathered and pushed into an internal logger buffer for recording with the current time step. The time step of the on-board software is given in milliseconds after start, and is determined by the frequency of the microcontroller. Low voltage may have an impact on this and the value will overflow at 56 days, however, these were not determined to be a concern with the implemented system. The next time the logger is activated, it will record the data and any available logging messages.

### Arming and Flight Setup

Arming the drone and starting a flight mission should be done carefully as not to risk harm to the operator. Once the pre-flight checks are done, strap the battery to the drone **but to not plug it in**. The transmitter should be kept **off** until the operator is ready to start the mission.

Place the drone down in the mission area in the intended orientation. The battery should then be plugged in the the operator should hear a series of beeps.  

> There should be a three start-up beeps, followed by two pairs of beeps. The drone is only ready after the second set of beeps.

The drone will start flashing the communication lights on the rear. Once the drone is ready, the lights should flash blue on and off.

The remote control should be turned on **only when the operator is ready to start the mission**. The drone will remain interlocked if the remote control is off. To start the mission, flick the left position switch to the middle position. This is the start command for the drone.

In case of an **emergency** or to stop the drone abruptly, **press and hold the middle button**. This button is under the power button on the remote control.

#### The Remote Interlock

As part of the safety feature set, the drone will not allow arming or execute commands unless a remote control is detected. As such, the drone can be safely interacted with if the remote control (the transmitter unit) is off.

The remote interlock will also be enabled if the transmitter is out of range of the drone.

<a id="during-flight"></a>

## During Flight

This section details the features of automated flight for setting up new flights and detailing the mechanisms behind drone control. 

### Flight modes

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

Mounted on the rear of the drone, the signal lights signal the current internal state of the drone. The drone has the following internal states:

```
Initialise, Ready, Armed, Flying, Landing, Stopped, Faulted, Debug
```

In the flying state, the drone can have the following substates:

```
OperatorControl, AutoStraightLine, ArmOnly
```

Each state has its own light patterns.

#### The default light pattern

Following aircraft convention, the leftmost signal light will always be red, and the rightmost light will always be green. When flying, the second-leftmost and second-rightmost lights will light up white.

```
The rear lights have 8 LEDs:
| 1 2 3 4 5 6 7 8 |

Under normal operation, the leftmost and rightmost are red and green:
| RED | 2 3 4 5 6 7 | GRN |

When flying, the second leftmost and second rightmost light up dim white:
| RED | WHT | 3 4 5 6 | WHT | GRN |
```

#### Light Patterns for each state

##### Initialise

The drone is starting up and initialising all components. The compass and accelerometer is calibrated at this time.

```
Gold / Off / Gold / Off...
```

##### Ready

The drone is ready for arming and flight commands. Requires operator to acknowledge by toggling left switch

```
Blue / Off / Blue / Off...
```

##### Armed

Drone is executing arming command to spin propellers

```
(Flashing Quickly)
Blue / Orange / Blue / Orange...
```

##### Flying - Operator Control

The drone is being directly controlled by the operator. 

```
Pink / Black / Pink / Black...
```

##### Flying - Automated Control

The drone is flying based on the automated commands in its memory

```
Blue / Lime / Blue / Lime...
```

##### Faulted

The drone has encountered a fatal error and/or the emergency stop has been triggered

```
Red / Black / Red / Black...
```

##### Debug

The drone is in a debugging mode, typically not accessible unless programmatically encoded onto the drone

```
(Gradient)
Pink ---- Blue
```

##### Unused States

The following states have not been implemented and are not used:

* Landing
* Stopped

<a id="after-flight"></a>

## After Flight

Once a mission is complete, the data can be extracted from the drone and passed to the Base Station Software. The drone can also be configured by connecting to Mission Planner.

### Extracting Mission Data from the Drone

Data on the drone can be accessed by removing the MicroSD card on the rear of the drone. The MicroSD card is accessed in batches and can be safely removed regardless of drone state.

**Please Note:** The SD Module may loosen itself over time, and thus, the files on the SD card may have not updated. The drone sends a notification through the Serial Port if it cannot detect an SD Card, however, the drone does not enter a faulted mode. This recommendation has been made for future improvements.

<a id="maintenance"></a>

## Maintenance

As a mechanical device, the drone requires maintenance to ensure that it remains in optimal condition. 

### General Maintenance

The drone should be inspected for the following:

* Ensure that there is no visible cracks to the carbon fibre frame of the drone. 
* Ensure that the propellers have no damage, are mounted in the correct orientation, and are not warped.
* Inspect the electronics package for loose wires. If there are any, please refer to the Connection Diagram for repair
* Ensure the ultrasonic sensors are free of dust and debris

### Drone Transport

The drone legs can be folded to protect the electronics package. The locking pin can be removed and the legs can be safely held in a closed position.

When transporting the drone, the propellers should be removed to reduce damage to the propellers.

### Manufactured Components

The drone has 3D printed parts to mount the electronics package and mount landing legs. The files have been provided on the repository under `src/stl`.

The components have been manufactured from PETG, a common and flexible 3D printing filament. This material has been chosen to allow for shock absorption and a higher glass transition temperature than PLA. Other recommended materials are ABS or ASA, but not PLA. 

Under high stress, the motors may generate excess heat, potentially causing the mounts to deform mid-flight. The specifications for the 3D printed parts are:

* Infill type: Cubic
* Infill Percentage: 55% to 70% (Recommended above 55%)
* Printing Temperature: 235 Degrees Celcius
* Layer Height: 0.1mm
* Printing Speed: 40mm/s
* Supports: Connected to build plate, only for Landing Legs

The provided components have been printed on an Artillery Sidewinder X2

### Drone Specifications

**Accelerometer:** DFROBOT SEN0168 Triple Axis Accelerometer BMA220

**Aviation Reference Number:** 1179999

**Battery:** 1100 mAh 6s 100c

**CASA License:** CASA-13QFCC

**Charger:** ToolkitRC M6 Multifunctional Charger

**Charger Adapter:** Refurbished Toshiba PA5114E-1AC3

**Compass:** Duinotech XC-4496

**ESC:** T-Motor F45A V3

**Firmware:** Ardupilot Firmware Matek H743 v. stable-4.1.5

**Flight Controller:** Matek H743 SLIM

**Frame:** HGLRC Sector 7 V3

**Microcontroller:** ESP32-DEVKIT C

**Motors:** T-Motor BMS Racing V2 2306.5 2000kv

**Propellers:** Gemfan 5" props, Pitch 3"

**RX Protocol:** SBUS

**RX/TX System:** Turnigy HK-MT6B

**Ultrasonic Sensors:** HC-SR04

### Wiring Diagram

[Argous S7 Wiring Diagram - Link to PDF](ArgousS7_TryFour.pdf)



