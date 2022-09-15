
/** 04/09/2022 this will be done today no matter what
 * @file as7-beta.ino 
 * @brief Summary of module
 * @author Jimmy Trac jt@nekox.net, Monique Kuhn monique.n.kuhn@gmail.com, Bi Wan Low bwlow39@gmail.com
 *
 * AS7 On Board Software (gamma)
 *  version 0.0.1
 *  https://github.com/Wabinabi/Argous
 * 
 * The Argous S7 is the prototyping aerial platform for the Autonomous Underground Cave Mapping Project. 
 * 
 * Hardware List:
 *  ESP32-DEVKIT C
 *  Matek H743-SLIM
 *  Turnigy HK-MT6B
 *  HC-SR04 Ultrasonic Sensor Sonar Distance
 *  DFROBOT SEN0168 Triple Axis Accelerometer, BMA220 (Tiny)
 * 
 *                 ESP32 Devkic C
 *                  +----------+  
 *                  |          |  
 *        +-----+---+----------+---+-----+
 *       <- 3V3 |                  | GND -> 
 *       <- EN  |                  |  23 ->  
 *       <- VP  |                  |  22 -> 
 *       <- VN  |                  |  TX ->
 *       <- 34  |                  |  RX -> 
 *       <- 35  |                  |  21 -> 
 *       <- 32  |                  | GND -> 
 *       <- 33  |                  |  19 ->
 *       <- 25  |                  |  18 ->
 *       <- 26  |                  |   5 ->
 *       <- 27  |                  |  17 -> 
 *       <- 14  |                  |  16 -> 
 *       <- 12  |                  |   4 -> 
 *       <- GND |                  |   0 -> 
 *       <- 13  |                  |   2 -> 
 *       <- D2  |                  |  15 -> 
 *       <- D3  |                  |  D1 -> 
 *       <- CMD |                  |  D0 ->  
 *       <- 5V  |                  | CLK -> 
 *        +-----+------------------+-----+  
 * 
 *
 *
 */
#include <sbus.h>     // SBUS Communication Library with FC
#include <FastLED.h>  // Glowbit/WS2812B Library
#include <SPI.h>      // for SD Card
#include <SD.h>       // for SD Card

#include <map>        // Dictionary/mapping

#include "SdLogger.h" // Main logging mechanism and SD Control
#include "Drone.h"    // Main drone interface and RX/TX Control

// Pin Mapping
#define I2C_SDA 21  // I2C Pin Mapping
#define I2C_SCL 22  // 

#define SIMULATION_ENABLE false // Enables simulating IO on the ESP32

// FastLED definitions
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

/* -------------------- Pin Mapping -------------------- */
const int STATUS_FASTLED_PIN = 3;
const int STATUS_NUM_LEDS = 8;
const int STATUS_LED_BRIGHTNESS = 64;

// Ultrasonic Pin Mapping
const int NUM_US_SENSORS = 6;     // Number of US sensors
const int NUM_US_POINTS = 3;      // Number of points to average for value
const int MAX_US_DISTANCE = 400;  // Maximum distance to be used for US Sensors

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

int US_TRIGPIN[NUM_US_SENSORS] = {};
int US_ECHOPIN[NUM_US_SENSORS] = {};

// FastLED LED Pin
const int FL_LEDPIN = 3;

// FC SBUS Pins
const int SBUS_RXPIN = 16;
const int SBUS_TXPIN = 17; 

// Thermistor analogue Pin
const int TEMP_PIN = 34;

/* -------------------- Constants and Definitions -------------------- */

// Defining delay speeds for FreeRTOS Tasks
const TickType_t TICK_US_DELAY = 80 / portTICK_PERIOD_MS; // An 80ms delay between the US sensors
const TickType_t TICK_SHORT = 200 / portTICK_PERIOD_MS;
const TickType_t TICK_MEDIUM = 500 / portTICK_PERIOD_MS;
const TickType_t TICK_LONG = 1000 / portTICK_PERIOD_MS;
const TickType_t TICK_LONGLONG = 5000 / portTICK_PERIOD_MS;

// Define FastLED constants
const int FL_LEDNUM = 8;
const int FL_LEDBRIGHTNESS = 64;
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB FL_LED[FL_LEDNUM];

DEFINE_GRADIENT_PALETTE( DEV_PALLETE ) {
  0,     255,  20,  147, // Fuchsia
128,     0,   255,  255,
255,     255,  20,  147};
CRGBPalette16 DEVPAL = DEV_PALLETE;

// SBUS Comms with FC
bfs::SbusRx sbusRx(&Serial1);
bfs::SbusTx sbusTx(&Serial1);
std::array<int16_t, NUM_CH> sbusTxData;
std::array<int16_t, NUM_CH> sbusRxData;

// SD Card Interface
File config_file; // Read in configuration for drone
File output_file; // Output PLY file
File log_file;    // Output Log file

// Variable for storing US results. Only to be written to by US threads
int us_distance[NUM_US_SENSORS][NUM_US_POINTS];

// Current temperature from thermistor
int tmp_temperature;

// Used by the Scribe thread after all 6 measurements taken
SemaphoreHandle_t enable_usSemaphore;
SemaphoreHandle_t enable_pilotSemaphore;

// Used to Start/Stop Mode Switching
SemaphoreHandle_t debug_switchModesSemaphore;

// Enable the Scribe, also forces one scribe at a time.
SemaphoreHandle_t enable_scribeSemaphore;

// Used for reading the message stacks for the scribe task
SemaphoreHandle_t scribe_logSemaphore;
SemaphoreHandle_t scribe_msgSemaphore;

// Mutex for the message stacks
SemaphoreHandle_t scribe_logStackMutex;
SemaphoreHandle_t scribe_msgStackMutex;

// Task handle for main threads
TaskHandle_t th_Ultrasonic;
TaskHandle_t th_Scribe;  // Maintains SD card.
TaskHandle_t th_Pilot;   // Constantly writes to the SBUS line
TaskHandle_t th_Comms;   // Handles LED sequencing and colours
TaskHandle_t th_Switch;  // Handles Switching for testing purposes

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
const int StraightLineDist = 100; // Aim to travel 100cm in AutoStraightLine mode

// Enums for drone FSM
enum DroneState {Initialise, Ready, Armed, Flying, Landing, Stopped, Faulted, Debug};
enum DroneFlightMode {OperatorControl, AutoStraightLine, ArmOnly};
enum DroneDebugTest {SBUS_COMMS, ARMING_DISARMING, SD_READ_WRITE, LED_RESPONSE}; // Unit testing scenarios

std::map<DroneState, std::string> droneStateMap = {
  {Initialise, "Initialise"},
  {Ready, "Ready"},
  {Armed, "Armed"},
  {Flying, "Flying"},
  {Landing, "Landing"},
  {Stopped, "Stopped"},
  {Faulted, "Faulted"},
  {Debug, "Debug"}
 };

// Drone initial states
DroneState currentState = Initialise;
DroneState nextState = Initialise;
DroneFlightMode currentFlightMode = ArmOnly;


/* ------------------------ Main Objects ------------------------ */

AS7::Logger logger(&Serial);
//AS7::Drone drone(&logger, &sbusRx, &sbusTx, &sbusRxData, &sbusTxData);
AS7::Drone drone(&logger, &sbusRx, &sbusTx);


/* --------------------- Function code --------------------- */

/* --------------------- Task code for threads --------------------- */
// Initial trials with class methods or void* parameters for code re-use didn't go so well
//  Keeping it simple and writing out the code 6 times

void us_Task(void * parameters) { 

  // Task code starts here
  int _distance;
  int _duration;
  int _pointCount = 0;
  for (;;) {
    xSemaphoreTake(enable_usSemaphore, portMAX_DELAY);
    _pointCount = (_pointCount++) % NUM_US_POINTS; // Cycles between 0 and NUM_US_POINTS

    for (int i = 0; i < NUM_US_SENSORS; i++) {
    //delayMicroseconds(10);    // Delay for 10us to provide some time between US executions
    digitalWrite(US_TRIGPIN[i], LOW);       // Clear the trig pin
    delayMicroseconds(2);
    digitalWrite(US_TRIGPIN[i], HIGH);      // Sets the trigPin on HIGH state for 10 us
    delayMicroseconds(10);
    digitalWrite(US_TRIGPIN[i], LOW);
    _duration = pulseIn(US_ECHOPIN[i], HIGH); // Reads the echoPin, returns the sound wave travel time in us
    _duration = _duration * 0.034 / 2;        // get duration in cm (rounded down due to int)
    us_distance[i][_pointCount] = min(_distance,MAX_US_DISTANCE); // < ---- Change static value here   

    // Section for performing any filtering on US inputs

    delay(80); // A delay of >70ms is recommended
    }
    xSemaphoreGive(enable_usSemaphore);
  }
}


void debug_switchModes(void * parameters) {
  xSemaphoreTake(debug_switchModesSemaphore, portMAX_DELAY);
  
  for (;;) {
    // If the switch modes semaphore is released, AS7 will constantly switch modes

    currentState = Initialise;
    Serial.println("Moving to Initialise Mode");
    vTaskDelay(TICK_LONGLONG);

    currentState = Ready;
    Serial.println("Moving to ReadyMode");
    vTaskDelay(TICK_LONGLONG);

    currentState = Armed;
    Serial.println("Moving to Armed Mode");
    vTaskDelay(TICK_LONGLONG);

    currentState = Flying;
    Serial.println("Moving to Flying-Default Mode");
    vTaskDelay(TICK_LONGLONG);

    currentFlightMode = ArmOnly;
    Serial.println("Moving to Flying-ArmOnly Mode");
    vTaskDelay(TICK_LONGLONG);

    currentFlightMode = OperatorControl;
    Serial.println("Moving to Flying-OperatorControl Mode");
    vTaskDelay(TICK_LONGLONG);

    currentFlightMode = AutoStraightLine;
    Serial.println("Moving to Flying-AutoStraightLine Mode");
    vTaskDelay(TICK_LONGLONG);

    currentState = Debug;
    Serial.println("Moving to Debug Mode");

    currentState = Faulted;
    Serial.println("Moving to Faulted Mode");
    
    vTaskDelay(TICK_LONGLONG);
    
    
  }
}

void FL_LEDComms(void * parameters) {
  /*
  Always-On LEDs:
   LED 0 is LEFT (RED)
   LED 7 is RIGHT (GREEN)
   CRGB colours can be found at http://fastled.io/docs/3.1/struct_c_r_g_b.html
  
  Key:
    Blue relates to flying-related tasks
    Lime relates to autonomous-related tasks
    
    LED 0 and 7 are always RED and GREEN unless Debugging
    LED 1 and 6 are always WHITE when flying

  Initialise:
    Normal Speed - Flashing Yellow
  Ready:
    Slower Speed - Flashing Blue
  Armed:
    Normal Speed - Flashing BlueOrange
  Flying:
    ArmOnly:
      NA
    OperatorControl:
      Normal Speed - Flashing Pink
    AutoStraightLine:
      Normal Speed - Flashing Blue-Lime
  Faulted:
    Fast Speed - Flashing Red, LEDs 1 and 6 Orange
  Debug:
    Blue-Pink Gradient

  */

  // Task code starts here
  for (;;) {
    FL_LED[0] = CRGB::Red;
    FL_LED[FL_LEDNUM-1] = CRGB::Green;

    // delay is vTaskDelay( xDelay );
    // Main loop for the state
    switch(currentState) {
      case Initialise:
        for (int i = 1; i < FL_LEDNUM-1; i++) {
          FL_LED[i] = CRGB::Gold;
        }

        FastLED.show();
        vTaskDelay(TICK_MEDIUM);
        for (int i = 1; i < FL_LEDNUM-1; i++) {
          FL_LED[i] = CRGB::Black;
        }

        FastLED.show();
        vTaskDelay(TICK_MEDIUM);
        break;

      case Ready:
        for (int i = 1; i < FL_LEDNUM-1; i++) {
          FL_LED[i] = CRGB::DodgerBlue;
        }

        FastLED.show();
        vTaskDelay(TICK_MEDIUM);
        for (int i = 1; i < FL_LEDNUM-1; i++) {
          FL_LED[i] = CRGB::Black;
        }

        FastLED.show();
        vTaskDelay(TICK_MEDIUM);
        break;

      case Armed:
          for (int i = 1; i < FL_LEDNUM-1; i++) {
            FL_LED[i] = CRGB::DodgerBlue;
          }
  
          FastLED.show();
          vTaskDelay(TICK_SHORT);
          for (int i = 1; i < FL_LEDNUM-1; i++) {
            FL_LED[i] = CRGB::Orange;
          }
  
          FastLED.show();
          vTaskDelay(TICK_SHORT);
            
        break;

      case Flying:
        
        switch(currentFlightMode) {
          case OperatorControl:
            for (int i = 1; i < FL_LEDNUM-1; i++) {
              FL_LED[i] = CRGB::DeepPink;
            }
            FL_LED[1] = CRGB::Grey;
            FL_LED[6] = CRGB::Grey;
            FastLED.show();
            vTaskDelay(TICK_MEDIUM);
            for (int i = 1; i < FL_LEDNUM-1; i++) {
              FL_LED[i] = CRGB::Black;
            }
            FL_LED[1] = CRGB::Grey;
            FL_LED[6] = CRGB::Grey;
            FastLED.show();
            vTaskDelay(TICK_MEDIUM);
            break;

          case AutoStraightLine:
            for (int i = 1; i < FL_LEDNUM-1; i++) {
              FL_LED[i] = CRGB::DodgerBlue;
            }
            FL_LED[1] = CRGB::Grey;
            FL_LED[6] = CRGB::Grey;
            FastLED.show();
            vTaskDelay(TICK_MEDIUM);
            for (int i = 1; i < FL_LEDNUM-1; i++) {
              FL_LED[i] = CRGB::LawnGreen;
            }
            
            FL_LED[1] = CRGB::Grey;
            FL_LED[6] = CRGB::Grey;
            FastLED.show();
            vTaskDelay(TICK_MEDIUM);
            break;
        }

        
        break;

      case Landing:

        break;

      case Stopped:

        break;

      case Faulted:
        for (int i = 1; i < FL_LEDNUM-1; i++) {
              FL_LED[i] = CRGB::Red;
            }
            FL_LED[1] = CRGB::Orange;
            FL_LED[6] = CRGB::Orange;
            FastLED.show();
            vTaskDelay(TICK_SHORT);
            break;

        break;

      case Debug:
        for (int i = 0; i < 8; i++) {
          fill_palette(FL_LED, 8, i * 255/8, 255/8, DEVPAL, 128, LINEARBLEND);
          FastLED.show();
          vTaskDelay(TICK_SHORT);
        }
        
        FastLED.show();

        break;
    }
  }
}



void setup() {
  /* --------------------- Config --------------------- */
  // Main Comms Serial to PC
  Serial.begin(115200);
  Serial.println("AS7 starting up");
  Serial.println("(c) Ecobat Project 2022");

  logger.start(0, 2);

  sbusRx.Begin(SBUS_RXPIN, SBUS_TXPIN);
  sbusTx.Begin(SBUS_RXPIN, SBUS_TXPIN);

  

  // Ultrasonic Driver pins
  
  US_TRIGPIN[0] = 32;
  US_ECHOPIN[0] = 35;
  US_TRIGPIN[1] = 25;
  US_ECHOPIN[1] = 33;
  US_TRIGPIN[2] = 12;
  US_ECHOPIN[2] = 13;
  US_TRIGPIN[3] = 27;
  US_ECHOPIN[3] = 14;
  US_TRIGPIN[4] = 4;
  US_ECHOPIN[4] = 0;
  US_TRIGPIN[5] = 2;
  US_ECHOPIN[5] = 15;

  // Set trig pins as outputs
  pinMode(US_TRIGPIN[0], OUTPUT);
  pinMode(US_TRIGPIN[1], OUTPUT);
  pinMode(US_TRIGPIN[2], OUTPUT);
  pinMode(US_TRIGPIN[3], OUTPUT);
  pinMode(US_TRIGPIN[4], OUTPUT);
  pinMode(US_TRIGPIN[5], OUTPUT);

  // Set echo pins as inputs
  pinMode(US_ECHOPIN[0], INPUT);
  pinMode(US_ECHOPIN[1], INPUT);
  pinMode(US_ECHOPIN[2], INPUT);
  pinMode(US_ECHOPIN[3], INPUT);
  pinMode(US_ECHOPIN[4], INPUT);
  pinMode(US_ECHOPIN[5], INPUT);
  

  // Set up rear status LEDs (Glowbit 1x8 or any 8-length WS2812B)
  FastLED.addLeds<LED_TYPE, FL_LEDPIN, COLOR_ORDER>(FL_LED, FL_LEDNUM).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( FL_LEDBRIGHTNESS );
  
  // Create binary semaphores
  enable_usSemaphore = xSemaphoreCreateBinary();
  //enable_pilotSemaphore = xSemaphoreCreateBinary();
  debug_switchModesSemaphore = xSemaphoreCreateBinary();

  // Enable startup tasks
  //xSemaphoreGive(enable_usSemaphore);
  //xSemaphoreGive(enable_pilotSemaphore);

  // Use this semaphore to enable automatic mode switching
  //xSemaphoreGive(debug_switchModesSemaphore);
  

  /* --------------------- Initialise Modules --------------------- */

/*

  xTaskCreatePinnedToCore(
    us_Task,
    "US Task",
    8192,
    NULL,  
    4, 
    &th_Ultrasonic,
    1);
  */

  xTaskCreatePinnedToCore(
    FL_LEDComms,
    "LED Comms",
    2056,
    NULL,
    1,
    &th_Comms,
    1);


  // xTaskCreatePinnedToCore(
  //   debug_switchModes,        /* Task function. */
  //   "Switch Modes",           /* name of task. */
  //   2056,                     /* Stack size of task */
  //   NULL,                     /* parameter of the task */
  //   3,                        /* priority of the task */
  //   &th_Switch,               /* Task handle to keep track of created task */
  //   1);                       /* pin task to core 1 */
  
  //logger.inform("AS7 has finished setup and is moving to main loop");

  if (SIMULATION_ENABLE) {
    Serial.println("-------------------- Simulation Enabled! --------------------");
    logger.inform("AS7 is starting in simulation mode!");
  }
  drone.start();

}

void loop() {
  
  AS7::DroneCommand armingCommand;
  armingCommand.desc = "This is a test arming command!";
  armingCommand.type = AS7::Arm;
  armingCommand.duration = 1000;

  // Main loop for the state
  switch(currentState) {
    case Initialise:

      if (logger.running()) {logger.inform("Initialise: Logger is reporting healthy."); }

      nextState = Ready;

      break;

    case Ready:
      // wait for sbus signal from drone
      // something like dorne.readch(threshold, ch)
      drone.allowArming();
      if (drone.droneAllowedToFly()) {nextState = Armed; }

      break;

    case Armed:

      drone.enqueueCommand(armingCommand);
      delay(1500);

      // drone is arming
      // basically queue drone to do arming thing 
      //' eue drone wait
      // then wait a bit more
      // then move to flying

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
  if (currentState != nextState) {
    // Transfer States

    // Completing state transfer
    switch( currentState ) {
      case Initialise:
        break;
      case Ready:
        break;
    }

    logger.inform("AS7 moving to state: " + droneStateMap[nextState]);
    currentState=nextState;


    
  }

  
  
}
