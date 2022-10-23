# Argous S7: Autonomous Mine Surveying Drone

The Argous Sector 7 (AS7) Project is the design and implementation of an autonomous mine surveying drone for a Swinburne University Final Year Project. The main repository can be found here: https://github.com/Wabinabi/Argous

<a id="outline"></a>

## Outline

The project is composed of the following modules:

* The AS7 Quadcopter, which itself is composed of:
  * A modified Sector 7 Racing Drone
  * An accompanying electronics and sensor package mounted under the drone containing the ‘on-board software’, documented within the AS7 Namespace
* The AS7 Base Station Software, or ‘Off-board software’

Configuration and source files can be found in the following locations:

* On-board software can be found under `src/esp32`
* Off-board software can be found under `src/win10`
* ArduPilot configuration can be found under `src/ardupilot`
<a id="table-of-contents"></a>
## Table of contents

- Argous S7: Autonomous Mine Surveying Drone
  - [Outline](index.html#outline)
  - [Table of contents](index.html#table-of-contents)
  - [Getting started for surveying](index.html#getting-started-for-surveying)
  - [Getting started for development](index.html#getting-started-for-development)
  - [Authors](index.html#authors)
- [Off-Board Software User Guide](md__h__repos__argous_src_esp32_as7_gamma__off__board__software.html)
  - [Importing Drone Data](md__h__repos__argous_src_esp32_as7_gamma__off__board__software.html#importing-drone-data)
  - [Viewing Trip Details](md__h__repos__argous_src_esp32_as7_gamma__off__board__software.html#viewing-trip-details)
  - [Exporting 3D Map](md__h__repos__argous_src_esp32_as7_gamma__off__board__software.html#exporting-3d-map)
  - [Software Features and Application Data](md__h__repos__argous_src_esp32_as7_gamma__off__board__software.html#software-features-and-application-data)
- [On-Board Software User Guide](md__h__repos__argous_src_esp32_as7_gamma__on__board__software.html)
  - [Before flight](md__h__repos__argous_src_esp32_as7_gamma__on__board__software.html#before-flight)
  - [During Flight](md__h__repos__argous_src_esp32_as7_gamma__on__board__software.html#during-flight)
  - [After Flight](md__h__repos__argous_src_esp32_as7_gamma__on__board__software.html#after-flight)
  - [Maintenance](md__h__repos__argous_src_esp32_as7_gamma__on__board__software.html#maintenance)

A full index of AS7 on-board terms can be found in [File Members](globals.html).

<a id="getting-started-for-surveying"></a>

## Getting started for surveying

For surveying use, the AS7 drone comes pre-flashed with the on-board software with details configurable through the SD card. 

<a id="getting-started-for-surveying"></a>

### Base Station Software

The Base Station Software requires Windows 10 or newer to operate. It can be downloaded from the releases section of the [GitHub Repository](https://github.com/Wabinabi/Argous). 

### Mission Planner

The Flight Controller on AS7 uses [ArduPilot](https://ardupilot.org/) for flight control. It can be configured through [Mission Planner](https://ardupilot.org/planner/). Installation instructions for Mission Planner can be found [here](https://ardupilot.org/planner/docs/mission-planner-installation.html).

<a id="getting-started-for-development"></a>

## Getting started for development

All source code can be found in the [GitHub Repository](https://github.com/Wabinabi/Argous), under the ‘src’ (source) folder. 

### On Board Software

The On-Board software is written using the [Arduino IDE](https://www.arduino.cc/en/software/) with the ESP32 Dev Kit. 

#### Adding the ESP32 to the Arduino IDE

The add-on for the Arduino IDE can be found by adding the ESP32 Library URL to the Additional Board Manager:

1. Head to `file` -> `preferences` and add the following to *Additional Board Manager URLs*
   1. `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
2. Press OK, and head to `Tools` -> `Board:…` -> `Boards Manager`. Searching for ESP32, it should show the library package.
3. Hit install, and you should be able to see `ESP32 Dev Kit` under `Boards.`

A more extensive guide can be found [here](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).

#### Required Dependencies and Libraries

The on-board software requires the following libraries:

* [BFS Sbus](https://github.com/bolderflight/sbus)
* [FASTLed](https://github.com/FastLED/FastLED)
* [Adafruit_MMC5883](https://github.com/adafruit/Adafruit_HMC5883_Unified)

These libraries can be installed by:

1. Going to `Tools` in the top bar and opening `Manage Libraries`.
2. Search for the above libraries and hit `Install`.
3. The libraries should be installed

To verify library installation, they can be found under `C:\Users\<<Your Username>>\Documents\Arduino\libraries`.

### Off Board Software

Development of the off-board software or the base station software requires the Qt IDE and framework. An open-source version of Qt can be found here: https://www.qt.io/download-open-source.

The base station software uses Qt 4.3.2 and does not require any additional modules.

#### Documentation Generation with Doxygen

Documentation (this file plus the reference) uses [Doxygen](https://doxygen.nl/). The specific configuration can be found inside `docs/doxygen` with the input as `src/esp32/as7-gamma` and output folder as `src\Win\Argous Qt code base\ArgouseBaseSoftware\appdata`.

The documentation uses [Doxygen Awesome CSS](https://github.com/jothepro/doxygen-awesome-css) with minor modifications to match the overall theme of the system. The additional CSS is located at `docs/doxygen/as7-custom.css`. The doxyfile configuration is as follows:

```css
HTML_EXTRA_STYLESHEET = doxygen-awesome.css as7-custom.css
```

<a id="authors"></a>

## Authors

Jimmy Trac jt@nekox.net

Monique Kuhn monique.n.kuhn@gmail.com

Bi Wan Low bwlow39@gmail.com

Joshua Redolfi joshua.redolfi@gmail.com
