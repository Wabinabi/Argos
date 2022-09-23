/*
  Library for the MMA8452Q
  By: Jim Lindblom and Andrea DeVore
  SparkFun Electronics

  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14587

  This sketch uses the SparkFun_MMA8452Q library to initialize
  the accelerometer and stream calcuated x, y, z, acceleration
  values from it (in g units).

  Hardware hookup:
  Arduino --------------- MMA8452Q Breakout
    3.3V  ---------------     3.3V
    GND   ---------------     GND
  SDA (A4) --\/330 Ohm\/--    SDA
  SCL (A5) --\/330 Ohm\/--    SCL

  The MMA8452Q is a 3.3V max sensor, so you'll need to do some
  level-shifting between the Arduino and the breakout. Series
  resistors on the SDA and SCL lines should do the trick.

  License: This code is public domain, but if you see me
  (or any other SparkFun employee) at the local, and you've
  found our code helpful, please buy us a round (Beerware
  license).

  Distributed as is; no warrenty given.
*/

#include <Wire.h>                 // Must include Wire library for I2C
#include "SparkFun_MMA8452Q.h"    // Click here to get the library: http://librarymanager/All#SparkFun_MMA8452Q

MMA8452Q accel;                   // create instance of the MMA8452 class

float xVals[5];
float yVals[5];
float zVals[5];

float xZeroSum[10];
float yZeroSum[10];
float zZeroSum[10];

float xZeroSumVel[10];
float yZeroSumVel[10];
float zZeroSumVel[10];

float xTotal = 0;
float yTotal = 0;
float zTotal = 0;

float xZeroSumTotal = 0;
float yZeroSumTotal = 0;
float zZeroSumTotal = 0;

float xZeroSumVelTotal = 0;
float yZeroSumVelTotal = 0;
float zZeroSumVelTotal = 0;

float xOffset = 0;
float yOffset = 0;
float zOffset = 0;

bool hasCalibrated = false;

float filt_x = 0;
float filt_y = 0;
float filt_z = 0;

float vel_x = 0;
float vel_y = 0;
float vel_z = 0;

float pos_x = 0;
float pos_y = 0;
float pos_z = 0;

float deltaMillis = 0;
int accelIndex = 0;
int zeroSumIndex = 0;

float _x = 0;
float _y = 0;
float _z = 0;

//float deltaMillis = 0;
float prevMillis = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("MMA8452Q Basic Reading Code!");
  Wire.begin();

  if (accel.begin() == false) {
    Serial.println("Not Connected. Please check connections and read the hookup guide.");
    //while (1);
  }
}

void loop() {
  if (accel.available()) {      // Wait for new data from accelerometer

    if (!hasCalibrated) {
      xOffset = accel.getCalculatedX() * 9.81;
      yOffset = accel.getCalculatedY() * 9.81;
      zOffset = accel.getCalculatedZ() * 9.81;

      Serial.print("Offsets: ");
      Serial.print(xOffset);
      Serial.print("/");
      Serial.print(yOffset);
      Serial.print("/");
      Serial.println(zOffset);


      hasCalibrated = true;
    }

    _x = accel.getCalculatedX() * 9.81 - xOffset;
    _y = accel.getCalculatedY() * 9.81 - yOffset;
    _z = accel.getCalculatedZ() * 9.81 - zOffset;

    if ( _x <= 0.3 & _x >= -0.3) {
      _x = 0;
    }
    if ( _y <= 0.3 & _y >= -0.3) {
      _y = 0;
    }
    if ( _z <= 0.3 & _z >= -0.3) {
      _z = 0;
    }

    xVals[accelIndex] = _x;
    yVals[accelIndex] = _y;
    zVals[accelIndex] = _z;
    xZeroSum[zeroSumIndex] = _x;
    yZeroSum[zeroSumIndex] = _y;
    zZeroSum[zeroSumIndex] = _z;
    
    xTotal = 0;
    yTotal = 0;
    zTotal = 0;

    for (int i = 0; i < 5; i++) {
      xTotal += xVals[i];
      yTotal += yVals[i];
      zTotal += zVals[i];
    }

    for (int i = 0; i < 10; i++) {
      xZeroSumTotal += xZeroSum[i];
      yZeroSumTotal += yZeroSum[i];
      zZeroSumTotal += zZeroSum[i];
    }

    // we want to adjust it to make the last 10 zero
    xVals[accelIndex] = _x - xZeroSumTotal;
    yVals[accelIndex] = _y - yZeroSumTotal;
    zVals[accelIndex] = _z - zZeroSumTotal;
    xZeroSum[zeroSumIndex] = _x - xZeroSumTotal;
    yZeroSum[zeroSumIndex] = _y - yZeroSumTotal;
    zZeroSum[zeroSumIndex] = _z - zZeroSumTotal;

    for (int i = 0; i < 5; i++) {
      xTotal += xVals[i];
      yTotal += yVals[i];
      zTotal += zVals[i];
    }

    xZeroSumTotal = 0;
    yZeroSumTotal = 0;
    zZeroSumTotal = 0;

    for (int i = 0; i < 10; i++) {
      xZeroSumTotal += xZeroSum[i];
      yZeroSumTotal += yZeroSum[i];
      zZeroSumTotal += zZeroSum[i];
    }

    filt_x = xTotal / 5;
    filt_y = yTotal / 5;
    filt_z = zTotal / 5;

    if ( filt_x <= 0.1 & filt_x >= -0.1) {
      filt_x = 0;
    }
    if ( filt_y <= 0.1 & filt_y >= -0.1) {
      filt_y = 0;
    }
    if ( filt_z <= 0.1 & filt_z >= -0.1) {
      filt_z = 0;
    }

    // VELOCITY
    _x += filt_x * deltaMillis;
    _y += filt_y * deltaMillis;
    _z += filt_z * deltaMillis;
    

    if ( _x <= 0.1 & _x >= -0.1) {
      _x = 0;
    }
    if ( _y <= 0.1 & _y >= -0.1) {
      _y = 0;
    }
    if ( _z <= 0.1 & _z >= -0.1) {
      _z = 0;
    }
    
    xZeroSumVel[zeroSumIndex] = _x;
    yZeroSumVel[zeroSumIndex] = _y;
    zZeroSumVel[zeroSumIndex] = _z;

    xZeroSumVelTotal = 0;
    yZeroSumVelTotal = 0;
    zZeroSumVelTotal = 0;

    for (int i = 0; i < 10; i++) {
      xZeroSumVelTotal += xZeroSumVel[i];
      yZeroSumVelTotal += yZeroSumVel[i];
      zZeroSumVelTotal += zZeroSumVel[i];
    }

    vel_x += _x - xZeroSumVelTotal;
    vel_y += _y - yZeroSumVelTotal;
    vel_z += _z - zZeroSumVelTotal;
    xZeroSumVel[zeroSumIndex] = _x - xZeroSumVelTotal;
    yZeroSumVel[zeroSumIndex] = _y - yZeroSumVelTotal;
    zZeroSumVel[zeroSumIndex] = _z - zZeroSumVelTotal;

    // POSITION
    _x += vel_x * deltaMillis;
    _y += vel_y * deltaMillis;
    _z += vel_z * deltaMillis;

    if ( _x <= 0.05 & _x >= -0.05) {
      _x = 0;
    }
    if ( _y <= 0.05 & _y >= -0.05) {
      _y = 0;
    }
    if ( _z <= 0.05 & _z >= -0.05) {
      _z = 0;
    }

    pos_x += _x;
    pos_y += _y;
    pos_z += _z;

    // loop between 0 and 4
    accelIndex = (accelIndex++) % 5;
    zeroSumIndex = (zeroSumIndex++) % 10;

    deltaMillis = ((float)millis()/1000 - (float)prevMillis);

    prevMillis = millis() / 1000;

    // Acceleration of x, y, and z directions in g units
    Serial.print(filt_x);
    Serial.print(" * ");
    Serial.print(deltaMillis);
    Serial.print("\t   | ");
    Serial.print(filt_x);
    Serial.print("\t");
    Serial.print(filt_y);
    Serial.print("\t");
    Serial.print(filt_z);
    Serial.print("   |   ");
    Serial.print(vel_x);
    Serial.print("\t");
    Serial.print(vel_y);
    Serial.print("\t");
    Serial.print(vel_z);
    Serial.print("   |   ");
    Serial.print(pos_x);
    Serial.print("\t");
    Serial.print(pos_y);
    Serial.print("\t");
    Serial.print(pos_z);
    Serial.println();

    delay(10);
  }
}
