## Sample Data to be loaded on EAGLE SD Card

A more problematic run with drone control, compass failure, and an E-Stop.

### Standard Information

* Flight Duration
  * 38805ms
* Altitude
* Throttle
  * Enabled
* Emergency Stop
  * Enabled at 25411ms
* ~~Operator Control~~
* Compass or Accel Events
  * Compass Failed at 176ms

### Changes

Recording Data and Transmit have been changed from Inform to Verbose to reduce the number of inform events



### Configuration

```
SAMPLE_RATE:200
NAV_FREQ:250
CTRL_FREQ:1000
STATUS_UPDATE_DELAY:250
LED_BRIGHTNESS:255
```

