/**
 * @file as7-beta.ino
 * @brief The main on-board sketch for the Ecobat Project
 * @author Jimmy Trac support@nekox.net 
 * 
 * AS7 On Board Software (beta)
 *  version 0.0.1
 *  https://github.com/Wabinabi/Argous
 * 
 * 
 * This file is mainly used for setting up and configuration
 *  See as7-main.ino 
 * 
 * 
 *
 */

#include <sbus.h>     // SBUS Communication Library with FC
#include <FastLED.h>  // WS2812B Library
#include <SPI.h>      // for SD Card
#include <SD.h>       // for SD Card
#include <stack>      // For logging

#include "as7-drone.h"
#include "as7-sdlogging.h"

/* ------------------------ Definitions and Constants ------------------------ */
// Where possible, we will be using constant integers to enforce type checking.
//  This is purely preference

// Pin Mapping
#define I2C_SDA 21  // I2C Pin Mapping
#define I2C_SCL 22  // 
#define CS_PIN 5    // SD Card Clock Pin

#define SIMULATION_ENABLE false // Enables simulating IO on the ESP32

// FastLED definitions
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

// Drone state for Finite State Machine

/* Initialise - Drone starting up and checks all devices
 * Ready      - Drone is ready to arm and start mission, requires operator input
 * Armed      - Drone starts arming process and spins propellers
 * Flying     - Drone is in the air and flying. Drone uses Flight Modes for operation
 * Landing    - Drone will attempt to land by lowering throttle
 * Stopped    - Drone will cease all movement and read next mission
 * Faulted    - Similar to stopped, but does not reset even after powering on/off
 * Debug      - For unit testing
*/

// Enums for drone FSM
enum DroneState {Initialise, Ready, Armed, Flying, Landing, Stopped, Faulted, Debug};
enum DroneFlightMode {OperatorControl, AutoStraightLine, ArmOnly};
enum DroneDebugTest {SBUS_COMMS, ARMING_DISARMING, SD_READ_WRITE, LED_RESPONSE}; // Unit testing scenarios

DroneState currentState = Initialise;
DroneFlightMode currentFlightMode = ArmOnly;


/* -------------------- Pin Mapping -------------------- */
const int STATUS_FASTLED_PIN = 3;
const int STATUS_NUM_LEDS = 8;
const int STATUS_LED_BRIGHTNESS = 128;

// Ultrasonic Pin Mapping
const int NUM_US_SENSORS = 6;
const int MAX_US_DISTANCE = 400;

const int US_TRIGPIN_0 = 32;
const int US_ECHOPIN_0 = 35;
const int US_TRIGPIN_1 = 25;
const int US_ECHOPIN_1 = 33;
const int US_TRIGPIN_2 = 12;
const int US_ECHOPIN_2 = 13;
const int US_TRIGPIN_3 = 27;
const int US_ECHOPIN_3 = 14;
const int US_TRIGPIN_4 = 4;
const int US_ECHOPIN_4 = 0;
const int US_TRIGPIN_5 = 2;
const int US_ECHOPIN_5 = 15;

// FC SBUS Pins
const int SBUS_RXPIN = 16;
const int SBUS_TXPIN = 17; 

// Thermistor analogue Pin
const int TEMP_PIN = 34;

// Defining delay speeds for FreeRTOS Tasks
const TickType_t TICK_US_DELAY = 80 / portTICK_PERIOD_MS; // An 80ms delay between the US sensors
const TickType_t TICK_SHORT = 200 / portTICK_PERIOD_MS;
const TickType_t TICK_MEDIUM = 500 / portTICK_PERIOD_MS;
const TickType_t TICK_LONG = 1000 / portTICK_PERIOD_MS;
const TickType_t TICK_LONGLONG = 5000 / portTICK_PERIOD_MS;

/* -------------------- Initialisation -------------------- */
// Recorded US Sensor Distances
int us_distance[NUM_US_SENSORS];

// Current value from thermistor
int temp_cv;

// SBUS Comms with FC
bfs::SbusRx SBUS_RX(&Serial1);
bfs::SbusTx SBUS_TX(&Serial1);
std::array<int16_t, bfs::SbusRx::NUM_CH()> sbus_data;


//CRGB debug_led[debug_ledNum];

int US_TRIGPIN[NUM_US_SENSORS] = {};
int US_ECHOPIN[NUM_US_SENSORS] = {};

// Status LED Gradient Palette
//  Essentially the nekox colours
DEFINE_GRADIENT_PALETTE( DEBUG_PALETTE ) {
  0,     255,  20,  147,  // Fuchsia
128,     0,   255,  255,  // Cyan
255,     255,  20,  147}; // Fuchsia
CRGBPalette16 DEBUG_PAL = DEBUG_PALETTE;

// Used by the Scribe thread after all 6 measurements taken
SemaphoreHandle_t sem_enable_us;
SemaphoreHandle_t sem_enable_pilot;
SemaphoreHandle_t sem_enable_scribe;
SemaphoreHandle_t sem_enable_modeSwitch;

// Task handle for main threads
TaskHandle_t th_Ultrasonic;
TaskHandle_t th_Scribe;  // Maintains SD card.
TaskHandle_t th_Pilot;   // Constantly writes to the SBUS line
TaskHandle_t th_Comms;   // Handles LED sequencing and colours
TaskHandle_t th_Switch;  // Handles Switching for testing purposes


/* ------------------------ Main Objects ------------------------ */

//AS7::Drone drone;
//AS7::Logger logger;

/* ------------------------ Main Loop Starts Here ------------------------ */

void setup()
{
    Serial.begin(115200);
    Serial.println("AS7 starting up");
    Serial.println("(c) Ecobat Project 2022");

    if (SIMULATION_ENABLE) {
        Serial.println("SimulationMode Enabled");
    }
    //sbus_rx.Begin(sbus_rxPin, sbus_txPin);
    //sbus_tx.Begin(sbus_rxPin, sbus_txPin);

    // Set up rear status LEDs (Glowbit 1x8 or any 8-length WS2812B)
    //FastLED.addLeds<LED_TYPE, debug_ledPin, COLOR_ORDER>(debug_led, debug_ledNum).setCorrection( TypicalLEDStrip );
    //FastLED.setBrightness( debug_ledBrightness );


	// Set pinModes
}

void loop()
{
	
}
