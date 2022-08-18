/**
 * @file on-board-main.ino
 *
 * @brief The main on-board sketch for the Ecobat Project
 *
 * @author Jimmy Trac
 * Contact: jt@nekox.net 
 * 
 * @note will complete banner later
 *
 */

#include "sbus.h"   // SBUS Communication Library with FC

#include <SPI.h>    // for SD Card
#include <SD.h>     // for SD Card

// Define Pins for devices.
//  format "device_function"

// Ultrasonic Driver pins
const int us_trigPin1 = 32;
const int us_echoPin1 = 35;
const int us_trigPin2 = 25;
const int us_echoPin2 = 33;
const int us_trigPin3 = 12;
const int us_echoPin3 = 13;
const int us_trigPin4 = 27;
const int us_echoPin4 = 14;
const int us_trigPin5 = 4;
const int us_echoPin5 = 0;
const int us_trigPin6 = 2;
const int us_echoPin6 = 15;

// SD Card Pins
#define CS_PIN 5

// I2C Pins, these are the default ESP32 pins
const int i2c_sdaPin = 21;
const int i2c_sclPin = 22;

#define I2C_SDA i2c_sdaPin;
#define I2C_SCL i2c_sclPin;

// FC SBUS Pins
const int sbus_rxPin = 16;
const int sbus_txPin = 17; 

// Thermistor analogue Pin
const int tmp_aPin = 34;

// Defining variables and objects

// SBUS Comms with FC
bfs::SbusRx sbus_rx(&Serial1);
bfs::SbusTx sbus_tx(&Serial1);
std::array<int16_t, bfs::SbusRx::NUM_CH()> sbus_data;

// Variable for throttle control, float value 0 to 1.
double nav_lift = 0;

// Variable for other 3 channels, floats -1 to 1
double nav_yaw = 0;    // Yaw is turn left/right
double nav_fwd = 0;    // Forward movement/Backward is -ve forward
double nav_stf = 0;    // Strafting left/right movement

// SD Card Interface
File config_file; // Read in configuration for drone
File output_file; // Output PLY file

// Variables for storing US results. Only to be written to by US threads
//  to avoid race conditions
int us1_distance;
int us2_distance;
int us3_distance;
int us4_distance;
int us5_distance;
int us6_distance;

// Current temperature from thermistor
int tmp_temperature;

// Binary semaphore definitions for enforcing US order
// Each US requires its own semaphore. US1 requires Step 1 and releases Step 2
SemaphoreHandle_t us_step1Semaphore;
SemaphoreHandle_t us_step2Semaphore;
SemaphoreHandle_t us_step3Semaphore;
SemaphoreHandle_t us_step4Semaphore;
SemaphoreHandle_t us_step5Semaphore;
SemaphoreHandle_t us_step6Semaphore;

// Used by the Scribe thread after all 6 measurements taken
SemaphoreHandle_t us_startSemaphore;

// Task handle definitions for US threads
TaskHandle_t Ultrasonic1;
TaskHandle_t Ultrasonic2;
TaskHandle_t Ultrasonic3;
TaskHandle_t Ultrasonic4;
TaskHandle_t Ultrasonic5;
TaskHandle_t Ultrasonic6;

// Task handle for other threads
TaskHandle_t Scribe;  // Writes US Data to SD Card. Scribe controls Semaphore "start"
TaskHandle_t Pilot;   // Constantly writes to the SBUS line

// Drone state for Finite State Machine

/* Initialise - Drone starting up and checks all devices
 * Ready      - Drone is ready to arm and start mission, requires operator input
 * Armed      - Drone starts arming process and spins propellers
 * Flying     - Drone is in the air and flying. Drone uses Flight Modes for operation
 * Landing    - Drone will attempt to land by lowering throttle
 * Stopped    - Drone will cease all movement and read next mission
 * Faulted    - Similar to stopped, but does not reset even after powering on/off
 * Debug      - For testing
*/

const int StraightLineDist = 100; // Aim to travel 100cm in AutoStraightLine mode

enum DroneState {Initialise, Ready, Armed, Flying, Landing, Stopped, Faulted, Debug};
enum DroneFlightMode {OperatorControl, AutoStraightLine, ArmOnly};
DroneState currentState = Initialise;
DroneFlightMode currentFlightMode = ArmOnly;


void setup() {
  // Main Comms Serial to PC
  Serial.begin(115200);
  Serial.println("Starting AS7 Serial Communications");

  // Start SBUS on pins
  sbus_rx.Begin(sbus_rxPin, sbus_txPin);
  sbus_tx.Begin(sbus_rxPin, sbus_txPin);

  // Set trig pins as outputs
  pinMode(us_trigPin1, OUTPUT);
  pinMode(us_trigPin2, OUTPUT);
  pinMode(us_trigPin3, OUTPUT);
  pinMode(us_trigPin4, OUTPUT);
  pinMode(us_trigPin5, OUTPUT);
  pinMode(us_trigPin6, OUTPUT);

  // Set echo pins as inputs
  pinMode(us_echoPin1, INPUT);
  pinMode(us_echoPin2, INPUT);
  pinMode(us_echoPin3, INPUT);
  pinMode(us_echoPin4, INPUT);
  pinMode(us_echoPin5, INPUT);
  pinMode(us_echoPin6, INPUT);

  // Create binary semaphores
  us_step1Semaphore = xSemaphoreCreateBinary();
  us_step2Semaphore = xSemaphoreCreateBinary();
  us_step3Semaphore = xSemaphoreCreateBinary();
  us_step4Semaphore = xSemaphoreCreateBinary();
  us_step5Semaphore = xSemaphoreCreateBinary();
  us_step6Semaphore = xSemaphoreCreateBinary();
  
  us_startSemaphore = xSemaphoreCreateBinary();



}

void loop() {


  // Main loop for the state
  switch(currentState) {
    case Initialise:


      break;

    case Ready:

      break;

    case Armed:

      break;

    case Flying:
      switch(currentFlightMode) {
        case OperatorControl:
          break;

        case AutoStraightLine:
          break;
      }


      break;

    case Landing:

      break;

    case Stopped:

      break;

    case Faulted:

      break;

    case Debug:

      break;
  }

  // Checking for state transfer conditions
  switch( currentState ) {
    case Initialise:
      break;
    case Ready:
      break;
  }

  // Completing state transfer
  switch( currentState ) {
    case Initialise:
      break;
    case Ready:
      break;
  }



  if(sbus_rx.Read()) {
    sbus_data = sbus_rx.ch();
     for (int i = 0; i < 16; i++) {
        Serial.print(sbus_data[i]);
       Serial.print(" ");
     }
  }

  
   Serial.println();

  /* Set the SBUS TX data to the received data */
  sbus_tx.ch(sbus_data);
  /* Write the data to the servos */
  sbus_tx.Write();
  
}
