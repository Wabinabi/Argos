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

// SD Card Interface

File config_file; // Read in configuration for drone
File output_file; // Output PLY file

void setup() {
  
  // Main Comms Serial to PC
  Serial.begin(115200);
  Serial.println("Starting");

  // Start SBUS on pins
  sbus_rx.Begin(sbus_rxPin, sbus_txPin);
  sbus_tx.Begin(sbus_rxPin, sbus_txPin);
  
  
}

void loop() {

  
  
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
