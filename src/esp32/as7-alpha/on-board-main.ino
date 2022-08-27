/**
 * @file on-board-main.ino
 *
 * @brief The main on-board sketch for the Ecobat Project
 *
 * @author Jimmy Trac
 * Contact: support@nekox.net 
 * 
 * @note will complete banner later
 *
 */

#include <sbus.h>     // SBUS Communication Library with FC
#include <FastLED.h>  // Glowbit/WS2812B Library
#include <SPI.h>      // for SD Card
#include <SD.h>       // for SD Card
#include <stack>      // For logging



// Define Pins for devices.
//  format "device_function"

int us_trigPin[NUM_US_SENSORS] = {};
int us_echoPin[NUM_US_SENSORS] = {};

// SD Card Pins
#define CS_PIN 5

// I2C Pins, these are the default ESP32 pins
const int i2c_sdaPin = 21;
const int i2c_sclPin = 22;

#define I2C_SDA i2c_sdaPin;
#define I2C_SCL i2c_sclPin;

// Define LED constants and pins
const int debug_ledPin = 3;
const int debug_ledNum = 8;
const int debug_ledBrightness = 128;
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB debug_led[debug_ledNum];

DEFINE_GRADIENT_PALETTE( debug_palette ) {
  0,     255,  20,  147, // Fuchsia
128,     0,   255,  255,
255,     255,  20,  147};
CRGBPalette16 debugPal = debug_palette;

const TickType_t debug_fastDelay = 100 / portTICK_PERIOD_MS; // 100ms tick delay
const TickType_t debug_normDelay = 500 / portTICK_PERIOD_MS; // 500ms tick delay
const TickType_t debug_slowDelay = 1000 / portTICK_PERIOD_MS; // 1000ms tick delay
const TickType_t debug_switchModesDelay = 5000 / portTICK_PERIOD_MS; // The delay between mode switching for debugging

// FC SBUS Pins
const int sbus_rxPin = 16;
const int sbus_txPin = 17; 

// Thermistor analogue Pin
const int tmp_aPin = 34;

/* --------------------- Defining Constants --------------------- */

// Debugging levels
#define LOG_LEVEL_SILENT  0
#define LOG_LEVEL_FATAL   1
#define LOG_LEVEL_ERROR   2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_INFO    4
#define LOG_LEVEL_NOTICE  4
#define LOG_LEVEL_TRACE   5

// SBUS Comms with FC
bfs::SbusRx sbus_rx(&Serial1);
bfs::SbusTx sbus_tx(&Serial1);
std::array<int16_t, bfs::SbusRx::NUM_CH()> sbus_data;

// Defining constants
const int MAX_US_DISTANCE = 400;
const int NUM_US_SENSORS = 6;

// Variable for throttle control, float value 0 to 1.
double nav_lift = 0;

// Variable for other 3 channels, floats -1 to 1
double nav_yaw = 0;    // Yaw is turn left/right
double nav_fwd = 0;    // Forward movement/Backward is -ve forward
double nav_stf = 0;    // Strafting left/right movement

// SD Card Interface
File config_file; // Read in configuration for drone
File output_file; // Output PLY file
File log_file;    // Output Log file

// Variable for storing US results. Only to be written to by US threads
int us_distance[NUM_US_SENSORS];

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

// Drone initial states
DroneState currentState = Initialise;
DroneFlightMode currentFlightMode = ArmOnly;

// 
std::stack<std::string> message_stack;

/* --------------------- Function code --------------------- */
void init_SD() {
  if (!SD.begin(CS_PIN)) {
    Serial.println("Error, SD Initialization Failed");
    return;
  }

  File testFile = SD.open("/SDTest.txt", FILE_WRITE);
  if (testFile) {
    testFile.println("Hello ESP32 SD");
    testFile.close();
    Serial.println("Success, data written to SDTest.txt");
  } else {
    Serial.println("Error, couldn't not open SDTest.txt");
  }

}

void log(int logLevel, String message) {

}

void log_fatal (String message) {

}

void log_error (String message);
void log_warning (String message);
void log_notice  (String message);
void log_trace   (String message);
void log_verbose (String message);



/* --------------------- Task code for threads --------------------- */
// Initial trials with class methods or void* parameters for code re-use didn't go so well
//  Keeping it simple and writing out the code 6 times

void us_Task(void * parameters) { 

  // Semaphore sequencing. Pre = The step required, Post = the step after
  //SemaphoreHandle_t preSemaphore = us_step1Semaphore;
  //SemaphoreHandle_t postSemaphore = us_step2Semaphore;

  // Task code starts here
  int ldistance;
  int lduration;
  for (;;) {
    xSemaphoreTake(enable_usSemaphore, portMAX_DELAY);
    for (int i = 0; i < NUM_US_SENSORS; i++) {
    delayMicroseconds(10);

    //Get Sequence Semaphore
    // Ultrasonic Code - Critical Section starts here
    // Clear the trig pin
    digitalWrite(us_trigPin[i], LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 us
    digitalWrite(us_trigPin[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(us_trigPin[i], LOW);
    // Reads the echoPin, returns the sound wave travel time in us
    lduration = pulseIn(us_echoPin[i], HIGH);
    // Calculating the distance
    ldistance = lduration * 0.034 / 2;

    us_distance[i] = min(ldistance,MAX_US_DISTANCE); // < ---- Change static value here   
    // End critical section

    // TODO: replace with logging task
    //if (serialEnabled) {
    //  Serial.print(us_distance[i]);
    //  Serial.print(" ");
    //}

    delay(80); // A delay of >70ms is recommended
    }
    xSemaphoreGive(enable_usSemaphore);
  }
}

void pilot_Task(void * parameters) { 
  // Task code starts here

  /*
  This is the pilot task which keeps the sbus 



  */

  for (;;) {
    xSemaphoreTake(enable_pilotSemaphore, portMAX_DELAY);

    if(sbus_rx.Read()) {
      sbus_data = sbus_rx.ch();
      for (int i = 0; i < 16; i++) {
          Serial.print(sbus_data[i]);
        Serial.print(" ");
      }
    }

    /* Set the SBUS TX data to the received data */
    sbus_tx.ch(sbus_data);
    /* Write the data to the servos */
    sbus_tx.Write();




    xSemaphoreGive(enable_pilotSemaphore);

  }
}


void debug_switchModes(void * parameters) {
  xSemaphoreTake(debug_switchModesSemaphore, portMAX_DELAY);
  
  for (;;) {
    // If the switch modes semaphore is released, AS7 will constantly switch modes

    currentState = Initialise;
    Serial.println("Moving to Initialise Mode");
    vTaskDelay(debug_switchModesDelay);

    currentState = Ready;
    Serial.println("Moving to ReadyMode");
    vTaskDelay(debug_switchModesDelay);

    currentState = Armed;
    Serial.println("Moving to Armed Mode");
    vTaskDelay(debug_switchModesDelay);

    currentState = Flying;
    Serial.println("Moving to Flying-Default Mode");
    vTaskDelay(debug_switchModesDelay);

    currentFlightMode = ArmOnly;
    Serial.println("Moving to Flying-ArmOnly Mode");
    vTaskDelay(debug_switchModesDelay);

    currentFlightMode = OperatorControl;
    Serial.println("Moving to Flying-OperatorControl Mode");
    vTaskDelay(debug_switchModesDelay);

    currentFlightMode = AutoStraightLine;
    Serial.println("Moving to Flying-AutoStraightLine Mode");
    vTaskDelay(debug_switchModesDelay);

    currentState = Debug;
    Serial.println("Moving to Debug Mode");

    currentState = Faulted;
    Serial.println("Moving to Faulted Mode");
    
    vTaskDelay(debug_switchModesDelay);
    
    
  }
}

void debug_LEDComms(void * parameters) {
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
    debug_led[0] = CRGB::Red;
    debug_led[debug_ledNum-1] = CRGB::Green;

    // delay is vTaskDelay( xDelay );
    // Main loop for the state
    switch(currentState) {
      case Initialise:
        for (int i = 1; i < debug_ledNum-1; i++) {
          debug_led[i] = CRGB::Gold;
        }

        FastLED.show();
        vTaskDelay(debug_normDelay);
        for (int i = 1; i < debug_ledNum-1; i++) {
          debug_led[i] = CRGB::Black;
        }

        FastLED.show();
        vTaskDelay(debug_normDelay);
        break;

      case Ready:
        for (int i = 1; i < debug_ledNum-1; i++) {
          debug_led[i] = CRGB::DodgerBlue;
        }

        FastLED.show();
        vTaskDelay(debug_slowDelay);
        for (int i = 1; i < debug_ledNum-1; i++) {
          debug_led[i] = CRGB::Black;
        }

        FastLED.show();
        vTaskDelay(debug_slowDelay);
        break;

      case Armed:
          for (int i = 1; i < debug_ledNum-1; i++) {
            debug_led[i] = CRGB::DodgerBlue;
          }
  
          FastLED.show();
          vTaskDelay(debug_normDelay);
          for (int i = 1; i < debug_ledNum-1; i++) {
            debug_led[i] = CRGB::Orange;
          }
  
          FastLED.show();
          vTaskDelay(debug_normDelay);
            
        break;

      case Flying:
        
        switch(currentFlightMode) {
          case OperatorControl:
            for (int i = 1; i < debug_ledNum-1; i++) {
              debug_led[i] = CRGB::DeepPink;
            }
            debug_led[1] = CRGB::Grey;
            debug_led[6] = CRGB::Grey;
            FastLED.show();
            vTaskDelay(debug_normDelay);
            for (int i = 1; i < debug_ledNum-1; i++) {
              debug_led[i] = CRGB::Black;
            }
            debug_led[1] = CRGB::Grey;
            debug_led[6] = CRGB::Grey;
            FastLED.show();
            vTaskDelay(debug_normDelay);
            break;

          case AutoStraightLine:
            for (int i = 1; i < debug_ledNum-1; i++) {
              debug_led[i] = CRGB::DodgerBlue;
            }
            debug_led[1] = CRGB::Grey;
            debug_led[6] = CRGB::Grey;
            FastLED.show();
            vTaskDelay(debug_normDelay);
            for (int i = 1; i < debug_ledNum-1; i++) {
              debug_led[i] = CRGB::LawnGreen;
            }
            
            debug_led[1] = CRGB::Grey;
            debug_led[6] = CRGB::Grey;
            FastLED.show();
            vTaskDelay(debug_normDelay);
            break;
        }

        
        break;

      case Landing:

        break;

      case Stopped:

        break;

      case Faulted:
        for (int i = 1; i < debug_ledNum-1; i++) {
              debug_led[i] = CRGB::Red;
            }
            debug_led[1] = CRGB::Orange;
            debug_led[6] = CRGB::Orange;
            FastLED.show();
            vTaskDelay(debug_fastDelay);
            break;

        break;

      case Debug:
        for (int i = 0; i < 8; i++) {
          fill_palette(debug_led, 8, i * 255/8, 255/8, debugPal, 128, LINEARBLEND);
          FastLED.show();
          vTaskDelay(debug_fastDelay);
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
  Serial.println("Starting AS7 Serial Communications");

  // Start SBUS on pins
  sbus_rx.Begin(sbus_rxPin, sbus_txPin);
  sbus_tx.Begin(sbus_rxPin, sbus_txPin);

  // Ultrasonic Driver pins
  us_trigPin[0] = 32;
  us_echoPin[0] = 35;
  us_trigPin[1] = 25;
  us_echoPin[1] = 33;
  us_trigPin[2] = 12;
  us_echoPin[2] = 13;
  us_trigPin[3] = 27;
  us_echoPin[3] = 14;
  us_trigPin[4] = 4;
  us_echoPin[4] = 0;
  us_trigPin[5] = 2;
  us_echoPin[5] = 15;

  // Set trig pins as outputs
  pinMode(us_trigPin[0], OUTPUT);
  pinMode(us_trigPin[1], OUTPUT);
  pinMode(us_trigPin[2], OUTPUT);
  pinMode(us_trigPin[3], OUTPUT);
  pinMode(us_trigPin[4], OUTPUT);
  pinMode(us_trigPin[5], OUTPUT);

  // Set echo pins as inputs
  pinMode(us_echoPin[0], INPUT);
  pinMode(us_echoPin[1], INPUT);
  pinMode(us_echoPin[2], INPUT);
  pinMode(us_echoPin[3], INPUT);
  pinMode(us_echoPin[4], INPUT);
  pinMode(us_echoPin[5], INPUT);
  

  // Set up rear status LEDs (Glowbit 1x8 or any 8-length WS2812B)
  FastLED.addLeds<LED_TYPE, debug_ledPin, COLOR_ORDER>(debug_led, debug_ledNum).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( debug_ledBrightness );

  // Create binary semaphores
  enable_usSemaphore = xSemaphoreCreateBinary();
  enable_scribeSemaphore = xSemaphoreCreateBinary();
  enable_pilotSemaphore = xSemaphoreCreateBinary();
  debug_switchModesSemaphore = xSemaphoreCreateBinary();

  scribe_logStackMutex = xSemaphoreCreateBinary();
  scribe_msgStackMutex = xSemaphoreCreateBinary();

  // Create Semaphores for tracking the number of messages/logs
  scribe_logSemaphore = xSemaphoreCreateCounting(65535,0);
  scribe_msgSemaphore = xSemaphoreCreateCounting(65535,0);

  // Enable startup tasks
  xSemaphoreGive(enable_usSemaphore);
  xSemaphoreGive(enable_scribeSemaphore);
  xSemaphoreGive(enable_pilotSemaphore);

  // Mutexes for messages
  xSemaphoreGive(scribe_logStackMutex);
  xSemaphoreGive(scribe_msgStackMutex);

  // Use this semaphore to enable automatic mode switching
  xSemaphoreGive(debug_switchModesSemaphore);

  /* --------------------- Initialise Modules --------------------- */
  init_SD();

  xTaskCreatePinnedToCore(
    us_Task,                /* Task function. */
    "US Task",              /* name of task. */
    8192,                   /* Stack size of task */
    NULL,                   /* parameter of the task */
    2, /* priority of the task */
    &th_Ultrasonic,         /* Task handle to keep track of created task */
    1);                     /* pin task to core 1 */

  xTaskCreatePinnedToCore(
    debug_LEDComms,         /* Task function. */
    "LED Comms",            /* name of task. */
    2056,                   /* Stack size of task */
    NULL,                   /* parameter of the task */
    1,                      /* priority of the task */
    &th_Comms,              /* Task handle to keep track of created task */
    1);                     /* pin task to core 1 */



  // xTaskCreatePinnedToCore(
  //   debug_switchModes,        /* Task function. */
  //   "Switch Modes",           /* name of task. */
  //   2056,                     /* Stack size of task */
  //   NULL,                     /* parameter of the task */
  //   3,                        /* priority of the task */
  //   &th_Switch,               /* Task handle to keep track of created task */
  //   1);                       /* pin task to core 1 */

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
  
}
