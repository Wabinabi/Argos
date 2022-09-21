#include <HMC5883L.h>
#include <Wire.h>

HMC5883L compass;

void setup()
{
  Serial.begin(9600);
  Serial.println("Initialize HMC5883L");
  
//  while (!compass.begin())
//  {
//    Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
//    delay(500);
//  }

  /* Set up magetometer */
  compass.setMeasurementMode(MEASUREMENT_CONTINUOUS);
}

void loop()
{
MagnetometerRaw raw = compass.readRawAxis();
    Serial.print("X: ");
    Serial.print(raw.XAxis);
    Serial.print(", Y: ");
    Serial.print(raw.YAxis);
    Serial.print(", Z: ");
    Serial.print(raw.ZAxis);
    Serial.println();

  delay(100);
}
