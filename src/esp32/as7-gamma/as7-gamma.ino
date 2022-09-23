 
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

#include <Wire.h>     // for i2c access with the accelerometer and compass
#include <Adafruit_MMC5883.h>

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

#define GRAVITY_CMS 980.6 // The acceleration due to gravity in cm/s2

/* -------------------- Pin Mapping -------------------- */
const int STATUS_FASTLED_PIN = 3;
const int STATUS_NUM_LEDS = 8;
const int STATUS_LED_BRIGHTNESS = 32;

// Ultrasonic Pin Mapping
const int NUM_US_SENSORS = 6;     // Number of US sensors
const int NUM_US_POINTS = 3;      // Number of points to average for value
const int MAX_US_DISTANCE = 2000;  // Maximum distance to be used for US Sensors

const int US_TRIGPIN_0 = 32;
const int US_ECHOPIN_0 = 35;
const int US_TRIGPIN_1 = 25;
const int US_ECHOPIN_1 = 33;
const int US_TRIGPIN_2 = 12;
const int US_ECHOPIN_2 = 13;
const int US_TRIGPIN_3 = 14;
const int US_ECHOPIN_3 = 27;
const int US_TRIGPIN_4 = 0;
const int US_ECHOPIN_4 = 4;
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

#define ACC_FREQ 200  // Minimum Frequency to poll the accelerometer and magnometer. Default: 200hz

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

// Accelerometer and Magnometer definitions
int8_t accel_x_int; // X Byte read from accelerometer
int8_t accel_y_int; // Y Byte read from accelerometer
int8_t accel_z_int; // Z Byte read from accelerometer
float accel_total_x; // Sum of the last NUM_ACCEL_READINGS, used for averaging
float accel_total_y; // Sum of the last NUM_ACCEL_READINGS, used for averaging
float accel_total_z; // Sum of the last NUM_ACCEL_READINGS, used for averaging
const float DIV_I=16; // Divisor from word (2 bytes, 16 bits) to float
byte accelData[3]; // Buffer for storing accelerometer registers

const int NUM_ACCEL_READINGS = 5;
unsigned long accel_prev_millis = 0;  // Stores the previous time the acccelerometer was read. Used to find Delta-T when integrating for position
unsigned long accel_curr_millis = 0;  // Stores the current time the acccelerometer was read. Used to find Delta-T when integrating for position
float accel_delta_s = 0;  // Time since last reading (s)
float accel_x_readings[NUM_ACCEL_READINGS]; // Stores the 5 previous accerometer points 
float accel_y_readings[NUM_ACCEL_READINGS]; // Stores the 5 previous accerometer points 
float accel_z_readings[NUM_ACCEL_READINGS]; // Stores the 5 previous accerometer points 
int accel_index = 0;

float accel_x; // X-axis g's from the accelerometer
float accel_y; // Y-axis g's from the accelerometer
float accel_z; // Z-axis g's from the accelerometer

float accel_filt_x; // Average of the last NUM_ACCEL_READINGS x-axis g's
float accel_filt_y; // Average of the last NUM_ACCEL_READINGS y-axis g's
float accel_filt_z; // Average of the last NUM_ACCEL_READINGS z-axis g's

float raw_vel_x = 0; // The estimated drone x-axis velocity
float raw_vel_y = 0; // The estimated drone y-axis velocity
float raw_vel_z = 0; // The estimated drone z-axis velocity

float filt_vel_x = 0; // The estimated drone x-velocity using the filtered value
float filt_vel_y = 0; // The estimated drone y-velocity using the filtered value
float filt_vel_z = 0; // The estimated drone z-velocity using the filtered value

float raw_pos_x = 0; // The estimated drone x-position using the raw accel value
float raw_pos_y = 0; // The estimated drone y-position using the raw accel value
float raw_pos_z = 0; // The estimated drone z-position using the raw accel value

float filt_pos_x = 0; // The estimated drone x-position using the filtered accel value
float filt_pos_y = 0; // The estimated drone y-position using the filtered accel value
float filt_pos_z = 0; // The estimated drone z-position using the filtered accel value

float compass_x;    // Compass X axis reading (millliTeslas)
float compass_y;    // Compass Y axis reading (millliTeslas)
float compass_z;    // Compass Z axis reading (millliTeslas)
float compass_heading; // Compass heading in degrees
const float declinationAngle = 0.192228625; // Melbourne's magnetic declination is +11 deg 50 s. See https://www.magnetic-declination.com/

float accel_x_offset = 0; // The offset from 'zeroing' x
float accel_y_offset = 0; // The offset from 'zeroing' y
float accel_z_offset = 0; // The offset from 'zeroing' z

bool accelerometerCalibrated = false; // Indicates if the accelerometer offsets have been set

float estDronePos_x; // The estimated drone x position based on the accelerometer
float estDronePos_y; // The estimated drone y position based on the accelerometer
float estDronePos_z; // The estimated drone z position based on the accelerometer

sensors_event_t compass_event; // The returned data from the compass

// SBUS Comms with FC
bfs::SbusRx sbusRx(&Serial1);
bfs::SbusTx sbusTx(&Serial1);
std::array<int16_t, NUM_CH> sbusTxData;
std::array<int16_t, NUM_CH> sbusRxData;

// SD Card Interface
File config_file; // Read in configuration for drone
File output_file; // Output PLY file
File log_file;    // Output Log file

bool recordingEnabled = false; // Stores whether or not the current drone command is indicating data gathering should be enabled

// Variable for storing US results. Only to be written to by US threads
int us_distance[NUM_US_SENSORS][NUM_US_POINTS];
int us_rawDistance[NUM_US_SENSORS];

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
TaskHandle_t th_Ultrasonic; // Task handle for ultrasonic thread
TaskHandle_t th_Comms;      // Task handle for LED sequencing and colours
TaskHandle_t th_Accel;      // Task handle for the accelerometer thread
TaskHandle_t th_Switch;     // Task handle for switching. Used for testing

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

AS7::Logger logger(&Serial);                  // Logger reference used to send information to the serial and manage the SD card
AS7::Drone drone(&logger, &sbusRx, &sbusTx);  // Drone reference to control the drone and its comms

Adafruit_MMC5883 compass = Adafruit_MMC5883(11111); // Assign a unique ID to the MMC5883 Compass
/* --------------------- Function code --------------------- */
// Updates the accelerometer values accel_x accel_y accel_z
void readAccelerometer() {
  Wire.beginTransmission(0x0A); // address of the accelerometer
  
  // reset the accelerometer
  Wire.write(0x04); // Y data
  Wire.endTransmission();
  Wire.requestFrom(0x0A,1);     // request 6 bytes from slave device #2
  while(Wire.available())       // slave may send less than requested
  {
    accelData[0] = Wire.read();   // receive a byte as character
  }

  accel_x_int=(int8_t)accelData[0]>>2;

  Wire.beginTransmission(0x0A);   // address of the accelerometer
  // reset the accelerometer
  Wire.write(0x06); // Y data
  Wire.endTransmission();
  Wire.requestFrom(0x0A,1);       // request 6 bytes from slave device #2
  while(Wire.available())         // slave may send less than requested
  {
    accelData[1] = Wire.read();   // receive a byte as characte
  }
  accel_y_int=(int8_t)accelData[1]>>2;

  Wire.beginTransmission(0x0A);   // address of the accelerometer
  // reset the accelerometer
  Wire.write(0x08); // Y data
  Wire.endTransmission();
  Wire.requestFrom(0x0A,1);       // request 6 bytes from slave device #2
    while(Wire.available())        // slave may send less than requested
  {
    accelData[2] = Wire.read();   // receive a byte as characte
  }
    accel_z_int=(int8_t)accelData[2]>>2;

    // Rounding to reduce noise (minimum value 4 or higher from raw value)
    accel_x_int = round((float)accel_x_int / 4);
    accel_y_int = round((float)accel_y_int / 4);
    accel_z_int = round((float)accel_z_int / 4);

    accel_x=(float)accel_x_int*GRAVITY_CMS/DIV_I - accel_x_offset;
    accel_y=(float)accel_y_int*GRAVITY_CMS/DIV_I - accel_y_offset;
    accel_z=(float)accel_z_int*GRAVITY_CMS/DIV_I - accel_z_offset;
}

/* --------------------- Task code for threads --------------------- */

void taskUltrasonicSensor(void * parameters) { 
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
      _distance = _duration * 0.034 / 2;        // get duration in cm (rounded down due to int)
      
      us_rawDistance[i] = min(_distance,MAX_US_DISTANCE);
      us_distance[i][_pointCount] = min(_distance,MAX_US_DISTANCE);

      logger.inform("Recording data from US " + std::to_string(i) + "->" + std::to_string(_distance));
      logger.recordData("us_"+std::to_string(i),_distance);
      
      // Section for performing any filtering on US inputs

       vTaskDelay(80/ portTICK_PERIOD_MS); // A delay of >70ms is recommended
    }
    xSemaphoreGive(enable_usSemaphore);
    logger.recordData("recordingEnabled",drone.recordingEnabled());
    logger.pushData();

    // Push US data to drone
    drone.usUp    = us_rawDistance[4];
    drone.usFront = us_rawDistance[0];
    drone.usDown  = us_rawDistance[1];
    drone.usLeft  = us_rawDistance[5];
    drone.usRight = us_rawDistance[2];
    drone.usBack  = us_rawDistance[3];

  }
}

// Accelerometer threaded task code 
//  Runs continuously on startup and updates accel_x, accel_y, accel_z
void taskAccelerometer(void * parameters) {
  for (;;) {
    readAccelerometer(); // Update accelerometer data

    if (!accelerometerCalibrated) {
      accel_x_offset = accel_x;
      accel_y_offset = accel_y;
      accel_z_offset = accel_z;
      accelerometerCalibrated = true;
    }

    // Get the low-pass filtered readings
    //  This returns the average of the last 5 accerometers
    accel_x_readings[accel_index] = accel_x;
    accel_y_readings[accel_index] = accel_y;
    accel_z_readings[accel_index] = accel_z;

    accel_total_x = 0;
    accel_total_y = 0;
    accel_total_z = 0;

    for (int i = 0; i < NUM_ACCEL_READINGS; i++) {
      accel_total_x += accel_x_readings[i];
      accel_total_y += accel_y_readings[i];
      accel_total_z += accel_z_readings[i];
    }

    accel_filt_x = accel_total_x / NUM_ACCEL_READINGS;
    accel_filt_y = accel_total_y / NUM_ACCEL_READINGS;
    accel_filt_z = accel_total_z / NUM_ACCEL_READINGS;

    // Inertial position estimation using the accelerometer
    //  This section uses both the filtered and non-filtered values.

    accel_curr_millis = millis();
    accel_delta_s = ((float)accel_curr_millis - (float)accel_prev_millis)/1000;

    // Work out the estimated velocity in cm/s
    // vel(cm/s) = accel_x (cm/s2) * s
    raw_vel_x += accel_x * accel_delta_s;
    raw_vel_y += accel_y * accel_delta_s;
    raw_vel_z += accel_z * accel_delta_s;

    filt_vel_x += accel_filt_x * accel_delta_s;
    filt_vel_y += accel_filt_y * accel_delta_s;
    filt_vel_z += accel_filt_z * accel_delta_s;

    raw_pos_x += raw_vel_x * accel_delta_s;
    raw_pos_y += raw_vel_y * accel_delta_s;
    raw_pos_z += raw_vel_z * accel_delta_s;

    filt_pos_x += filt_vel_x * accel_delta_s;
    filt_pos_y += filt_vel_y * accel_delta_s;
    filt_pos_z += filt_vel_z * accel_delta_s;

    // Push data to logger
    logger.recordData("raw_accel_x", accel_x);
    logger.recordData("raw_accel_y", accel_y);
    logger.recordData("raw_accel_z", accel_z);
    logger.recordData("filt_accel_x", accel_filt_x);
    logger.recordData("filt_accel_y", accel_filt_y);
    logger.recordData("filt_accel_z", accel_filt_z);
    
    logger.recordData("raw_vel_x", raw_vel_x);
    logger.recordData("raw_vel_y", raw_vel_y);
    logger.recordData("raw_vel_z", raw_vel_z);
    logger.recordData("filt_vel_x", filt_vel_x);
    logger.recordData("filt_vel_y", filt_vel_y);
    logger.recordData("filt_vel_z", filt_vel_z);
    
    logger.recordData("raw_pos_x", raw_pos_x);
    logger.recordData("raw_pos_y", raw_pos_y);
    logger.recordData("raw_pos_z", raw_pos_z);
    logger.recordData("filt_pos_x", filt_pos_x);
    logger.recordData("filt_pos_y", filt_pos_y);
    logger.recordData("filt_pos_z", filt_pos_z);
    
    accel_index = (accel_index++) % NUM_ACCEL_READINGS;

    compass.getEvent(&compass_event); // Get compass data

    // Get compass data from event
    compass_x = compass_event.magnetic.x; 
    compass_y = compass_event.magnetic.y;
    compass_z = compass_event.magnetic.z;

    // Get compass heading and account for declination angle
    compass_heading = atan2(compass_y, compass_x) - declinationAngle;

    if(compass_heading < 0) { compass_heading += 2*PI; }    // Correct for when signs are reversed
    if(compass_heading > 2*PI) { compass_heading -= 2*PI; } // Check for wrap with declination

    compass_heading = compass_heading * 180/M_PI; // Convert to degrees for ease of use

    // Push data to logger
    logger.recordData("compass_x", compass_x);
    logger.recordData("compass_y", compass_y);
    logger.recordData("compass_z", compass_z);
    logger.recordData("heading", compass_heading);

    // Push data to drone
    drone.compassHeading = compass_heading;

    accel_prev_millis = millis();
    vTaskDelay((1000 / ACC_FREQ)/ portTICK_PERIOD_MS);
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
  
// Always-On LEDs:
//  LED 0 is LEFT (RED)
//  LED 7 is RIGHT (GREEN)
//  CRGB colours can be found at http://fastled.io/docs/3.1/struct_c_r_g_b.html
// 
// Key:
//   Blue relates to flying-related tasks
//   Lime relates to autonomous-related tasks
//   
//   LED 0 and 7 are always RED and GREEN unless Debugging
//   LED 1 and 6 are always WHITE when flying// 
// Initialise:
//   Normal Speed - Flashing Yellow
// Ready:
//   Slower Speed - Flashing Blue
// Armed:
//   Normal Speed - Flashing BlueOrange
// Flying:
//   ArmOnly:
//     NA
//   OperatorControl:
//     Normal Speed - Flashing Pink
//   AutoStraightLine:
//     Normal Speed - Flashing Blue-Lime
// Faulted:
//   Fast Speed - Flashing Red, LEDs 1 and 6 Orange
// Debug:
//   Blue-Pink Gradient
void taskStatusLeds(void * parameters) {


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
            FL_LED[1] = CRGB::Red;
            FL_LED[6] = CRGB::Red;
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
    vTaskDelay(100  / portTICK_PERIOD_MS); 
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
  US_TRIGPIN[0] = US_TRIGPIN_0;
  US_ECHOPIN[0] = US_ECHOPIN_0;
  US_TRIGPIN[1] = US_TRIGPIN_1;
  US_ECHOPIN[1] = US_ECHOPIN_1;
  US_TRIGPIN[2] = US_TRIGPIN_2;
  US_ECHOPIN[2] = US_ECHOPIN_2;
  US_TRIGPIN[3] = US_TRIGPIN_3;
  US_ECHOPIN[3] = US_ECHOPIN_3;
  US_TRIGPIN[4] = US_TRIGPIN_4;
  US_ECHOPIN[4] = US_ECHOPIN_4;
  US_TRIGPIN[5] = US_TRIGPIN_5;
  US_ECHOPIN[5] = US_ECHOPIN_5;

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
  //debug_switchModesSemaphore = xSemaphoreCreateBinary();

  // Enable startup tasks
  xSemaphoreGive(enable_usSemaphore);
  //xSemaphoreGive(enable_pilotSemaphore);

  // Use this semaphore to enable automatic mode switching
  //xSemaphoreGive(debug_switchModesSemaphore);
  

  /* --------------------- Initialise Modules --------------------- */

  xTaskCreatePinnedToCore(
    taskUltrasonicSensor,
    "AS7 Ultrasonic Sensor Thread",
    4096,
    NULL,  
    1, 
    &th_Ultrasonic,
    0);
  

  xTaskCreatePinnedToCore(
    taskStatusLeds,
    "AS7 LED Status Thread",
    2056,
    NULL,
    1,
    &th_Comms,
    0);

  xTaskCreatePinnedToCore(
    taskAccelerometer,
    "AS7 Accelerometer Thread",
    2056,
    NULL,
    1,
    &th_Accel,
    0);

  // Uncomment for task switcher
  // xTaskCreatePinnedToCore(
  //   debug_switchModes,        /* Task function. */
  //   "Switch Modes",           /* name of task. */
  //   2056,                     /* Stack size of task */
  //   NULL,                     /* parameter of the task */
  //   3,                        /* priority of the task */
  //   &th_Switch,               /* Task handle to keep track of created task */
  //   1);                       /* pin task to core 1 */
  
  if (SIMULATION_ENABLE) {
    Serial.println("-------------------- Simulation Enabled! --------------------");
    logger.inform("AS7 is starting in simulation mode!");
  }

  /* --------------------- Initialise Hardware --------------------- */
  // Set up the Accelerometer
  Wire.begin();
  Wire.beginTransmission(0x0A); // address of the accelerometer

  // Range Settings
  Wire.write(0x22); // Register Address
  Wire.write(0x00); // Range value (0x00)

  // Low Pass Filter
  Wire.write(0x20); // Register Address
  Wire.write(0x05); // Refer to datasheet on wiki
  Wire.endTransmission();

  // Set up the Compass
  if(!compass.begin()) {
    logger.fatal("Compass was unable to start, could the module be loose?");
  }


  // Initialise drone class
  drone.start();

  AS7::DroneCommand armingCommand;
  AS7::DroneCommand blindCommand;

  armingCommand.desc = "Arming Drone";
  armingCommand.type = AS7::Arm;
  armingCommand.duration = 8000;

  drone.enqueueCommand(armingCommand);

  blindCommand.desc = "Flying up a little bit - 10% initial thrust";
  blindCommand.type = AS7::Blind;
  blindCommand.duration = 2000;

  blindCommand.v_y = 0.0f;
  blindCommand.v_x = 0.0f;
  blindCommand.v_z = 0.15f;
  blindCommand.v_yw = 0.0f;
  
  drone.enqueueCommand(blindCommand);
  blindCommand.desc = "Flying forward at 5%";
  blindCommand.type = AS7::Blind;
  blindCommand.duration = 2000;

  blindCommand.v_y = 0.0f;
  blindCommand.v_x = -0.3f;
  blindCommand.v_z = 0.15f;
  blindCommand.v_yw = 0.0f;
  
  drone.enqueueCommand(blindCommand);

  blindCommand.desc = "Stopping Drone";
  blindCommand.type = AS7::Blind;
  blindCommand.duration = 5000;

  blindCommand.v_y = 0.0f;
  blindCommand.v_x = 0.0f;
  blindCommand.v_z = 0.0f;
  blindCommand.v_yw = 0.0f;
  
  drone.enqueueCommand(blindCommand);


}

void loop() {
  

  // Main loop for the state
  switch(currentState) {
    case Initialise:

      if (logger.running()) {logger.inform("Initialise: Logger is reporting healthy."); }

      nextState = Ready;

      break;

    case Ready:
      // wait for sbus signal from drone
      // something like dorne.readch(threshold, ch)
      
      //if (drone.droneAllowedToFly()) {nextState = Armed; }
      if (drone.channelConfirm(CH_FLIGHTMODE, 0.4f)) {
        drone.allowArming();
        nextState = Armed; 
      }

      break;

    case Armed:

      
      //delay(1500);

      // drone is arming
      // basically queue drone to do arming thing 
      //' eue drone wait
      // then wait a bit more
      // then move to flying
      if (drone.getDroneArmComplete()) {
        nextState = Flying;
      }

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

  if (drone.getEnableEmergencyStop()) { // Drone has triggered E-Stop
    nextState = Faulted;
  } else if (drone.getEnableOperatorControl()) {
    nextState = Flying;
    currentFlightMode = OperatorControl;
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


    vTaskDelay(5  / portTICK_PERIOD_MS); 
  }

  
  
}
