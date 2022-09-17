#include <Wire.h>

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(0x30); // address of the accelerometer
  // range settings
  Wire.write(0x22); //register address
  // low pass filter
  Wire.write(0x20); //register address
  Wire.write(0x05); //can be set at"0x05""0x04"......"0x01""0x00", refer to Datashhet on wiki
  Wire.endTransmission();
}

void AccelerometerInit()
{
  Wire.beginTransmission(0x30); // address of the accelerometer
  // reset the accelerometer
  Wire.write(0x04); // Y data
  Wire.endTransmission();
  Wire.requestFrom(0x30,1);    // request 6 bytes from slave device #2
  while(Wire.available())    // slave may send less than requested
  {
    Version[0] = Wire.read(); // receive a byte as characte
  }
  x_data=(int8_t)Version[0]>>2;

  Wire.beginTransmission(0x30); // address of the accelerometer
  // reset the accelerometer
  Wire.write(0x06); // Y data
  Wire.endTransmission();
  Wire.requestFrom(0x30,1);    // request 6 bytes from slave device #2
  while(Wire.available())    // slave may send less than requested
  {
    Version[1] = Wire.read(); // receive a byte as characte
  }
  y_data=(int8_t)Version[1]>>2;

  Wire.beginTransmission(0x30); // address of the accelerometer
  // reset the accelerometer
  Wire.write(0x08); // Y data
  Wire.endTransmission();
  Wire.requestFrom(0x30,1);    // request 6 bytes from slave device #2
   while(Wire.available())    // slave may send less than requested
  {
    Version[2] = Wire.read(); // receive a byte as characte
  }
   z_data=(int8_t)Version[2]>>2;

   x=(float)x_data;
   y=(float)y_data;
   z=(float)z_data;
   //Serial.print("X=");
   Serial.print(x);         // print the character
   Serial.print("  ");
   //Serial.print("Y=");
   Serial.print(y);         // print the character
   Serial.print("  ");
   //Serial.print("Z=");           // print the character
   Serial.println(z);
}

void loop()
{
  AccelerometerInit();
 delay(100);

}
