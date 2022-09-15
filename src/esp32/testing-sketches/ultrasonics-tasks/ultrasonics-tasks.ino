/*
  Ultrasonics main sketch for FYP
  This will be the basis to develop the rest of the navigation code.
  We're keeping things in singular files to get around the weirdness that is arduino classes.

  It should also be easier to debug as a result.
*/

 

// defines pins numbers
const int trigPin1 = 17;
const int trigPin2 = 5;
const int trigPin3 = 18;
const int echoPin1 = 21;
const int echoPin2 = 22;
const int echoPin3 = 23;

// Shared echo pin semaphore
SemaphoreHandle_t echoSemaphore;

// Synchroniser semaphores to enforce order.
//  It should be possible to place these in arrays but we're keeping things simple
//  C++ arrays are interesting beasts after all.
SemaphoreHandle_t step1Semaphore;
SemaphoreHandle_t step2Semaphore;
SemaphoreHandle_t step3Semaphore;
//SemaphoreHandle_t step4Semaphore;

TaskHandle_t Ultrasonic1;
TaskHandle_t Ultrasonic2;
TaskHandle_t Ultrasonic3;


// LED pins
const int led1 = 15;
const int led2 = 4;

// defines variables
long duration;
int distance;

int us1Distance;
int us2Distance;
int us3Distance;

void Ultrasonic1TaskCode(void * pvDistance) {
  int pinNumber = trigPin1;
  int echoPin = echoPin1;
  int ldistance;
  int lduration;

  // Semaphore sequencing. Pre = The step required, Post = the step after
  SemaphoreHandle_t preSemaphore = step1Semaphore;
  SemaphoreHandle_t postSemaphore = step2Semaphore;

  // Task code starts here
  for (;;) {
    delayMicroseconds(10);

    //Get Sequence Semaphore
    xSemaphoreTake(preSemaphore, portMAX_DELAY);
    //Get EchoSemaphore
    xSemaphoreTake(echoSemaphore, portMAX_DELAY);

    // Ultrasonic Code - Critical Section starts here
    // Clear the trig pin
    digitalWrite(pinNumber, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(pinNumber, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinNumber, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    lduration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    ldistance = lduration * 0.034 / 2;
    //*pvDistance = ldistance;

    us1Distance = ldistance; // < ---- Change static value here
    Serial.print(min(us1Distance,400));
  Serial.print(" ");
    // End critical section

    delay(80);
    // Return Echo and release next semaphore
    xSemaphoreGive(echoSemaphore);
    xSemaphoreGive(postSemaphore);
    
  }
}

void Ultrasonic2TaskCode(void * pvDistance) {
  int pinNumber = trigPin2;
  int echoPin = echoPin2;
  int ldistance;
  int lduration;

  // Semaphore sequencing. Pre = The step required, Post = the step after
  SemaphoreHandle_t preSemaphore = step2Semaphore;
  SemaphoreHandle_t postSemaphore = step3Semaphore;

  // Task code starts here
  for (;;) {
    delayMicroseconds(10);

    //Get Sequence Semaphore
    xSemaphoreTake(preSemaphore, portMAX_DELAY);
    //Get EchoSemaphore
    xSemaphoreTake(echoSemaphore, portMAX_DELAY);

    // Ultrasonic Code - Critical Section starts here
    // Clear the trig pin
    digitalWrite(pinNumber, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(pinNumber, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinNumber, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    lduration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    ldistance = lduration * 0.034 / 2;
    //*pvDistance = ldistance;

    us2Distance = ldistance; // < ---- Change static value here
    Serial.print(min(us2Distance,400));
  Serial.print(" ");
    // End critical section

    // Return Echo and release next semaphore
    xSemaphoreGive(echoSemaphore);
    xSemaphoreGive(postSemaphore);
    delay(80);
  }
}

void Ultrasonic3TaskCode(void * pvDistance) {
  int pinNumber = trigPin3;
  int echoPin = echoPin3;
  int ldistance;
  int lduration;

  // Semaphore sequencing. Pre = The step required, Post = the step after
  SemaphoreHandle_t preSemaphore = step3Semaphore;
  SemaphoreHandle_t postSemaphore = step1Semaphore;

  // Task code starts here
  for (;;) {
    delayMicroseconds(10);

    //Get Sequence Semaphore
    xSemaphoreTake(preSemaphore, portMAX_DELAY);
    //Get EchoSemaphore
    xSemaphoreTake(echoSemaphore, portMAX_DELAY);

    // Ultrasonic Code - Critical Section starts here
    // Clear the trig pin
    digitalWrite(pinNumber, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(pinNumber, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinNumber, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    lduration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    ldistance = lduration * 0.034 / 2;
    //*pvDistance = ldistance;

    us3Distance = ldistance; // < ---- Change static value here
    Serial.println(min(us3Distance,400));
    // End critical section

    // Return Echo and release next semaphore
    xSemaphoreGive(echoSemaphore);
    xSemaphoreGive(postSemaphore);
    delay(80);
  }
}


void setup() {
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(trigPin2, OUTPUT);
  pinMode(trigPin3, OUTPUT);

  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
  pinMode(echoPin2, INPUT);
  pinMode(echoPin3, INPUT);

  // Create Semaphores, they're all intialised to 0.
  echoSemaphore = xSemaphoreCreateBinary();
  
  step1Semaphore = xSemaphoreCreateBinary();
  step2Semaphore = xSemaphoreCreateBinary();
  step3Semaphore = xSemaphoreCreateBinary();
  //step4Semaphore = xSemaphoreCreateBinary();

  xSemaphoreGive(echoSemaphore);
  xSemaphoreGive(step1Semaphore);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Ultrasonic1TaskCode,    /* Task function. */
    "Ultrasonic 1",         /* name of task. */
    10000,                  /* Stack size of task */
    NULL,                   /* parameter of the task */
    1,                      /* priority of the task */
    &Ultrasonic1,           /* Task handle to keep track of created task */
    1);                     /* pin task to core 1 */

xTaskCreatePinnedToCore(
    Ultrasonic2TaskCode,    /* Task function. */
    "Ultrasonic 2",         /* name of task. */
    10000,                  /* Stack size of task */
    NULL,                   /* parameter of the task */
    1,                      /* priority of the task */
    &Ultrasonic2,           /* Task handle to keep track of created task */
    1);                     /* pin task to core 1 */

xTaskCreatePinnedToCore(
    Ultrasonic3TaskCode,    /* Task function. */
    "Ultrasonic 3",         /* name of task. */
    10000,                  /* Stack size of task */
    NULL,                   /* parameter of the task */
    1,                      /* priority of the task */
    &Ultrasonic3,           /* Task handle to keep track of created task */
    1);                     /* pin task to core 1 */

  Serial.begin(9600); // Starts the serial communication
}

void loop() {


}
