## Sample Data to be loaded on SNDK SD Card

A very normal run with the drone used to gather data only. Drone was never armed.

Additional configuration added to map US 0's Echo and Trig Pins.

### Standard Information

* Flight Duration
  * 147694ms
* Altitude
* Throttle
  * Disabled (Drone never armed)
* ~~Emergency Stop~~
* ~~Operator Control~~
* ~~Compass or Accel Events~~

### Changes

Recording Data and Transmit have been changed from Inform to Verbose to reduce the number of inform events



### Configuration

```
SAMPLE_RATE:200
NAV_FREQ:250
CTRL_FREQ:1000
STATUS_UPDATE_DELAY:250
LED_BRIGHTNESS:32
US_TRIGPIN_0:15
US_ECHOPIN_0:16
```

