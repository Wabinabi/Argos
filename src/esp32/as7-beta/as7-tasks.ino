

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


