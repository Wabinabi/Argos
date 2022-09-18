#include "Drone.h"

namespace AS7 
{
    /* ---------------------------------- Task Methods ---------------------------------- */ 
    
    void Drone::startNavTask(void* _this) {
        ((Drone*)_this)->navigationTask(NULL);
    }

    void Drone::startCtlTask(void* _this) {
        ((Drone*)_this)->controllerTask(NULL);
    }


    // Drone Navigation Task
    //  Runs on Drone::Start() and can be stoped by using Drone::pause().
    //
    // Processes commands from the command queue. Goes through them one-by-one.
    // Commands are popped from the queue and are stored as the "current command".
    //  The current millis() + cmd.duration is saved. Once millis() exceeds the saved value, the function proceeds to the next command
    // 
    // All commands will adhere to the correct ramp rate or easing function provided. Not sure how this will be implemented yet ( :D )
    // If there are no more commands, the drone will indicate droneCommandsCompleted
    //
    // process is:
    // Checks if there is a current command running
    // checks if there is a command available
    //
    // STRAIGHT X
    // SIDEWAYS Y
    // UPWARRDS Z
    //
    // Channel 1 Left   - Right     201-1846
    // Channel 2 Up     - Down      201-1846
    // Channel 3 LowTh  - HiTh      240-1791
    // Channel 4 YawL   - YawR      500-1500
    // Channel 5 LLever             240-1807
    // Channel 6 FLIGHT MODE
    // Channel 7 Button             240-1807
    // Channel 8 RLever             240-1807



    void Drone::navigationTask(void * parameters) { 
        getLogger()->verbose("Navigation task thread reporting running status");
        unsigned long finishTime = millis(); // Keeps track of the end time of our current command
        
        DroneCommand currentCommand;

        rampChannel(0.0f, 8, 0.0f,  RAMPRATE_NONE);
        rampChannel(0.0f, 9, 0.0f,  RAMPRATE_NONE);
        rampChannel(0.0f, 10, 0.0f, RAMPRATE_NONE);
        rampChannel(0.0f, 11, 0.0f, RAMPRATE_NONE);
        rampChannel(0.0f, 12, 0.0f, RAMPRATE_NONE);
        rampChannel(0.0f, 13, 0.0f, RAMPRATE_NONE);
        rampChannel(0.0f, 14, 0.0f, RAMPRATE_NONE);
        rampChannel(0.0f, 15, 0.0f, RAMPRATE_NONE);
        
        for (;;) {
            xSemaphoreTake(getSemDroneEnableMutex(), portMAX_DELAY);
            // Main Block
            //getLogger()->verbose("Drone active command status:" + std::to_string(getHasActiveComamnd()));
            if (getHasActiveComamnd()) {
                // Process Command block
                
                switch(currentCommand.type) {

                    
                            
                    case Blind:
                        if (getDroneHasArmed()) {
                            // Blind is only concerned with velocities, not position.
                            rampChannel(currentCommand.v_x, CH_STRAIGHT, 0.05f, RAMPRATE_LINEAR);
                            rampChannel(currentCommand.v_y, CH_STRAFE, 0.05f, RAMPRATE_LINEAR);
                            rampChannel(currentCommand.v_z, CH_THROTTLE, 0.10f, RAMPRATE_LINEAR);
                            rampChannel(currentCommand.v_yw, CH_YAW, 0.05f, RAMPRATE_LINEAR);

                            //getLogger()->verbose("Throttle: " + std::to_string(readTxChannel_f(CH_THROTTLE)));
                            
                        }
                        break;

                    case Guided:

                        break;

                    case Landing:

                        break;

                    case Arm:
                        // Send arming command
                        rampChannel( 0.9f, CH_STRAIGHT, 0.15f, RAMPRATE_NONE);
                        rampChannel(-0.9f, CH_STRAFE, 0.15f, RAMPRATE_NONE);
                        rampChannel( 0.0f, CH_THROTTLE, 0.3f, RAMPRATE_NONE);
                        rampChannel( 0.9f, CH_YAW, 0.15f, RAMPRATE_NONE);
                        setDroneHasArmed();
                        break;


                }
                if (!getDroneHasArmed()) {
                    finishTime = currentCommand.duration + millis();
                }
                //getLogger()->verbose("Mills vs FinishTime: " + std::to_string(millis()) + " vs. " + std::to_string(finishTime));    
                setHasActiveCommand(millis() < finishTime); // If we've passed our command duration, we unset the active command
            } else {
                //getLogger()->inform("Command completed, seeking new command");
                // Get Command Block
                //  As there is no active command, we will attempt to get one and set it up

                if (nextCommandAvailable()) {      // Check if there's a command available
                    getLogger()->verbose("New command found");
                    if (droneAllowedToFly()) {                          // Check if the drone is allowed to fly
                        currentCommand = dequeueCommand();
                        finishTime = currentCommand.duration + millis();
                        getLogger()->inform("Starting new command: " + currentCommand.desc + " for " + std::to_string(currentCommand.duration) + "ms");
                        getLogger()->verbose("Command to finish at " + std::to_string(finishTime) + "ms");
                        setHasActiveCommand(true);

                    } else { // Drone is not allowed to fly yet
                        //_logger->verbose("Drone is not armed yet, waiting for arming...");
                        // This thread runs often and therefore would flood the verbose serial port.
                    }
                    setDroneCommandsCompleted(); // indicate that drone has no commands
                    // When the drone has no commands, we send the e-stop tx packet to the FC, unless there's another one we want to send
                    
                    //setSbusTxData(getEStopTx());
                }
            }
            xSemaphoreGive(getSemDroneEnableMutex());
            // Delay for 1ms
            vTaskDelay((1000/NAV_FREQ) / portTICK_PERIOD_MS);
        }
    }


    // Controller Task/Thread implementation
    //  Runs on Drone::Start() and can be stopped using Drone::pause().
    //  
    // Aims to keep the channel alive by constantly sending information to the tx channel (pin).
    //  Emergency stops are proceeded here. To ensure that the E-Stop and Operator Override always work correctly,
    //   they have been nested in an if-else function
    //  This should give preference to the emergency stop, then override, then normal drone commands (if any). 
    //  If there are no commands, the drone should indicate it's state

    void Drone::controllerTask(void * parameters) { 
        getLogger()->verbose("Controller task thread reporting running status");
        bool _dataAvailable;

        for (;;) {
            xSemaphoreTake(getSemControlEnableMutex(), portMAX_DELAY);
            

            // If there is available data, update the internally received data
            _dataAvailable =_sbusRx->Read();
            
            if (_dataAvailable) {
                setSbusRxData(_sbusRx->ch());

                if (getSbusRxData()[15] > 900 & getSbusRxData()[14] > 900 & getSbusRxData()[13] > 900 & getSbusRxData()[12] > 900 & getSbusRxData()[11] > 900 & getSbusRxData()[10] > 900) {
                //getLogger()->verbose(formatSbusArray(getSbusRxData()));
                // Estop and Override Check
                // This may differ depending on your controller
                if (readRxChannel_f(CH_ESTOP) > 0.7f) { // EStop Threshold
                    emergencyStop(); // Toggle EStop
                } else if (readRxChannel_f(CH_FLIGHTMODE) > 0.7f) {
                    //getLogger()->inform(std::to_string(readRxChannel(CH_FLIGHTMODE)));
                    enableOperatorControl();
                }

                

                // Set Data
                if (getEnableEmergencyStop()) {
                    getSbusTx()->ch(getEStopTx());          // Writes EStop Packet to TX
                } else if (getEnableOperatorControl()) {
                    getSbusTx()->ch(getSbusRxData());       // Writes received RX packets to TX channel. Drone acts as pass-through
                } else { 
                    getSbusTx()->ch(getSbusTxData());       // Transmits normal data to drone              
                }

                // Share status to logger every STATUS_UPDATE_DELAY updates
                

                // Transmit data to drone
                //getLogger()->inform("TX: " + formatSbusArray(getSbusTxData()));
                getSbusTx()->Write();
                }
            }

            if (getControllerStatusCount()) {
                    //getLogger()->inform("Controller status: estop/operator: " + std::to_string(getEnableEmergencyStop()) + "/"+  std::to_string(getEnableOperatorControl()));
                    getLogger()->inform("DATA: " + std::to_string(_dataAvailable) + " TX: " + formatSbusArray(getSbusTx()->ch()) + " RX: " + formatSbusArray(getSbusRx()->ch()));
                }

            
            xSemaphoreGive(getSemControlEnableMutex());
            vTaskDelay((1000/CTL_FREQ) / portTICK_PERIOD_MS);
        }
    }

    // Returns true every 250 calls
    bool Drone::getControllerStatusCount() {
        _controllerStatusCount++;
        _controllerStatusCount = _controllerStatusCount % STATUS_UPDATE_DELAY;
        return _controllerStatusCount == 0;        
    }
    

    // hello
    // X add enqueue
    // X and stack
    // X add reader
    // X add sbus writer (always writing to sbus)
    // X add things that write to sbus -- sbus writer therefore needs semaphore for WRITE not READ
    // X add functions to change whats writetn to sbus, syz and ch
    // and then simpler fuunctions like things to do (raise up and fly)
    // add in tuning for flight mode and stuff

    // add amounts and variables
    // X add tuning adjustments for min-maxing the amount
    // X  -> as in scalaing

    // X add operator override
    // X add operttor estop

    // X dont forget about ramp rates
    // X they will need to be slowly adjusted up so the drone doesnt jerk
    // X rates might change per ch

    void Drone::start(int core, int priority) {
        BaseType_t xReturned;

        xReturned = xTaskCreatePinnedToCore(
        this->Drone::startNavTask,
        "Navigation",  
        8192,
        this,
        priority,
        &thDrone,
        core);

        if (xReturned == pdPASS ) {
            getLogger()->verbose("Navigator thread successfully started");
        } else {
            getLogger()->fatal("Navigator thread failed to start, is the core set correctly or the memory allocation too big to allocate?");
        }
        

        xReturned = xTaskCreatePinnedToCore(
        this->Drone::startCtlTask,
        "Controller",  
        8192,
        this,
        priority,
        &thRemote,
        core);

        if (xReturned == pdPASS ) {
            getLogger()->verbose("Controller thread successfully started");
        } else {
            getLogger()->fatal("Controller thread failed to start, is the core set correctly or the memory allocation too big to allocate?");
        }
    
        _running = true;
    }

    /* ---------------------------------- Private Member Methods ---------------------------------- */ 

    void Drone::pause() {
        if (!_running) {
            _logger->error("Drone pause request received despite drone not running.");
        } else {
            _running = false;
            xSemaphoreTake(getSemDroneEnableMutex(), portMAX_DELAY);
            xSemaphoreTake(getSemControlEnableMutex(), portMAX_DELAY);
            _logger->verbose("Drone and Control Enable Mutexes taken");
            _logger->warn("Drone thread operation paused");
        }
    }

    void Drone::resume() {
        if (!_running) {
            _running = true;
            xSemaphoreGive(getSemDroneEnableMutex());
            xSemaphoreGive(getSemControlEnableMutex());
            _logger->verbose("Drone Control and Enable Mutexes released");
            _logger->warn("Drone thread operation resumed");
        } else {
            _logger->error("Drone resume request received despite drone running.");
        }
        
    }

    // Getters for task access
    SemaphoreHandle_t Drone::getSemDroneEnableMutex() { return _semDroneEnableMutex; }
    SemaphoreHandle_t Drone::getSemControlEnableMutex() { return _semControlEnableMutex; }
    SemaphoreHandle_t Drone::getTxChMutex() { return _semTxChMutex; }
    SemaphoreHandle_t Drone::getRxChMutex() { return _semRxChMutex; }

    Logger* Drone::getLogger() { return _logger;}
    std::array<bool, NUM_CH> Drone::getSbusAbsChannels() {return _sbusAbsChannels; }

    bfs::SbusRx* Drone::getSbusRx() { return _sbusRx; }
    bfs::SbusTx* Drone::getSbusTx() { return _sbusTx; }

    // Setters for task access and write persmissions
    void Drone::setSbusRxData(std::array<int16_t, NUM_CH> data) {
        xSemaphoreTake(getRxChMutex(), portMAX_DELAY);
            _sbusRxData = data;
        xSemaphoreGive(getRxChMutex());
    }
    /*
    void Drone::setSbusTxData(std::array<int16_t, NUM_CH> data) {
        xSemaphoreTake(getTxChMutex(), portMAX_DELAY);
            _sbusRxData = data;
        xSemaphoreGive(getTxChMutex());
    }
    */

    void Drone::initUpperLowerBoundArrays() {
        
        _logger->verbose("Setting all channel lower bounds to " + std::to_string(SBUS_CHANNEL_LOWER));
        _logger->verbose("Setting all channel upper bounds to " + std::to_string(SBUS_CHANNEL_UPPER));

        for (int i = 0; i < NUM_CH; i++) {
            _sbusTxChLower[i] = SBUS_CHANNEL_LOWER;
            _sbusTxChUpper[i] = SBUS_CHANNEL_UPPER;
            _sbusRxChLower[i] = SBUS_CHANNEL_LOWER;
            _sbusRxChUpper[i] = SBUS_CHANNEL_UPPER;
        }
    }

    // Returns a string with all 16 channel values
    // Used for converting sbus data into a string for serial logging
    std::string Drone::formatSbusArray(std::array<int16_t, NUM_CH> chData) {
        std::string formattedData = "";
        for (int i = 0; i < NUM_CH; i++) {
            formattedData += "\t" + std::to_string(chData[i]);
        }
        return formattedData;
    }

    // Returns the next ramped up value
    // Ramp Rates restrict the amount a value can increase and shape the way there.
    // NOTE THAT THIS FUNCTION IS TIME-INDEPENDENT!
    //
    // For example:
    //  Current Value = 0f    (e.g. a current SBUS channel) 
    //  Target Value  = 0.5f  (e.g. Drone needs to be at 50% thrust)
    //  Rate          = 0.05f (e.g. We want to slowly accelerate to 50% thrust)
    //
    // No Ramp Rate - Instant jump
    // Returns 0.5f
    //
    // Ramp Rate Linear - Literally linear
    // Returns 0.05, 0.10, 0.15... 0.5
    //
    // Ramp Rate Proportional - Changes by RATE% of the diffence. In this case, 5% of the difference
    // Returns 0.025, 0.0485, 0.0713, 0.0927... -> 0.5 (but mathematically never reaches)
    //
    // A ramp rate of 0.4f would be:
    // Returns 0.2, 0.32, 0.329, 0.4352, 0.461, 0.486, 0.491, 0.494, 0.495, 0.498...
    //
    float Drone::rampValue(float value, float target, float rate, int rampRateType) {
        float _returnValue;

        //getLogger()->verbose("RampValue value, target, rate, rampratetype:");
        //getLogger()->verbose(std::to_string(value)+"/"+std::to_string(target)+"/"+std::to_string(rate)+"/"+std::to_string(rampRateType));

        switch(rampRateType) {

            case RAMPRATE_LINEAR:
                /*if (min(value + rate, target) < target) {
                    _returnValue = min(value + rate, target);
                } else {
                    _returnValue = target;
                }
                */
               
               _returnValue = value + clamp(target - value, -1 * rate, rate);
                break;

            case RAMPRATE_PROP:
                    _returnValue = (target - value) * rate;
                break;

            default: // Default to no ramping or RAMPRATE_NONE
                _returnValue = target;
                break;
        }
        //getLogger()->verbose("RampValue returned value:" + std::to_string(_returnValue));
        return _returnValue;
    }

    void Drone::rampChannel(float target, int8_t ch, float rate, int rampRateType) {
        //getLogger()->verbose("Ramping ch " + std::to_string(ch) + " -> in:" + std::to_string(readTxChannel_f(ch)));
        writeTxChannel_f(rampValue(readTxChannel_f(ch), target, rate, rampRateType), ch);
    }

    // Returns the corrected integer value for this channel (adjusted for min/max and abs of the channel)
    //  This will only use the values for the TX channels
    int16_t Drone::convRxChannel_i(float value, int8_t ch) {
        int16_t _adjustedValue;

        float _chLower = _sbusRxChLower[ch];
        float _chUpper = _sbusRxChUpper[ch];
        float _chValue = value;

        if (_sbusAbsChannels[ch]) { // This is an absolute channel. Accepts (0, 1)
            _adjustedValue = _chLower + (_chUpper - _chLower) * value;
        } else {
            value += 1; // Range is now (0, 2)
            value /= 2; // Range is now (0, 1), we can use the same formula as above.
            _adjustedValue = _chLower + (_chUpper - _chLower) * value;
        }

        return _adjustedValue;
    }

    // Returns the corrected floating point value for this channel (adjusted for min/max and abs of the channel)
    //  This will only use the values for the RX channels
    float Drone::convRxChannel_f(int16_t value, int8_t ch) {
        //_logger->verbose("Pre Conversion Channel: " + std::to_string(value));
        //int16_t _value = clamp(value, _sbusRxChUpper[ch], _sbusRxChLower[ch]);
        float _adjustedValue;

        float _chLower = _sbusRxChLower[ch];
        float _chUpper = _sbusRxChUpper[ch];
        float _chValue = value;

        //_logger->verbose("Converting Channel: " + std::to_string(ch));
        //_logger->verbose("Converting value: " + std::to_string(_chValue) + " converted from " + std::to_string(value));

        if (_sbusAbsChannels[ch]) { // This is an absolute channel. Returns (0, 1)
            //_logger->verbose("Numerator: " + std::to_string(_chValue - _chLower));
            //_logger->verbose("Demoninator: " + std::to_string(_chUpper - _chLower));
            
            //_logger->verbose("Actual numerator: " + std::to_string(_adjustedValue));
            _adjustedValue = (_chValue - _chLower) / (_chUpper - _chLower);
            //_logger->inform("This channel is absolute");
        } else { // Returns (-1, 1)
            _adjustedValue =  (value - ((_chUpper - _chLower) / 2)) / ((_chUpper - _chLower)/2);
            //_logger->inform("This channel is not absolute");
        }

        //_logger->verbose("The range of this channel is: " + std::to_string(_sbusRxChLower[ch]) + "-" + std::to_string(_sbusRxChUpper[ch]));
        //_logger->verbose("The range using floats is: " + std::to_string(_chLower) + "-" + std::to_string(_chUpper));
        //_logger->verbose("Adjusted value is: " + std::to_string(_adjustedValue));

        return _adjustedValue;
    }

    // Returns the corrected integer value for this channel (adjusted for min/max and abs of the channel)
    //  This will only use the values for the TX channels
    int16_t Drone::convTxChannel_i(float value, int8_t ch) {
        int16_t _adjustedValue;

        float _chLower = _sbusTxChLower[ch];
        float _chUpper = _sbusTxChUpper[ch];
        float _chValue = value;

        if (_sbusAbsChannels[ch]) { // This is an absolute channel. Accepts (0, 1)
            _adjustedValue = _chLower + (_chUpper - _chLower) * value;
        } else {
            value += 1; // Range is now (0, 2)
            value /= 2; // Range is now (0, 1), we can use the same formula as above.
            _adjustedValue = _chLower + (_chUpper - _chLower) * value;
        }

        return _adjustedValue;
    }

    // Returns the corrected floating point value for this channel (adjusted for min/max and abs of the channel)
    //  This will only use the values for the Tx channels
    float Drone::convTxChannel_f(int16_t value, int8_t ch) {
        
        float _adjustedValue;

        float _chLower = _sbusTxChLower[ch];
        float _chUpper = _sbusTxChUpper[ch];
        float _chValue = value;

        if (_sbusAbsChannels[ch]) { // This is an absolute channel. Returns (0, 1)
            _adjustedValue = (_chValue - _chLower) / (_chUpper - _chLower);
        } else { // Returns (-1, 1)
            _adjustedValue = (value - ((_chUpper - _chLower) / 2)) / ((_chUpper - _chLower)/2);
        }

        //getLogger()->verbose("ConvTxChannel_f (" + std::to_string(value) + ") =" + std::to_string(_adjustedValue));

        return _adjustedValue;
    }

    int16_t Drone::readRxChannel(int16_t ch) {
        return _sbusRxData[ch];
    }
    
    float Drone::readRxChannel_f(int16_t ch) {
        float chValue = convRxChannel_f(readRxChannel(ch), ch);
        return chValue;
    }

    int16_t Drone::readTxChannel(int16_t ch) {
        return _sbusTxData[ch];
    }
    
    float Drone::readTxChannel_f(int16_t ch) {
        //getLogger()->verbose("Reading in channel: " + std::to_string(readTxChannel(ch)) +"->"+std::to_string(convTxChannel_f(readTxChannel(ch), ch)));
        float chValue = convTxChannel_f(readTxChannel(ch), ch);
        return chValue;
    }

    void Drone::writeRxChannel_f(float value, int8_t ch) {
        writeChannel(convRxChannel_i(value, ch), ch);
    }

    void Drone::writeTxChannel_f(float value, int8_t ch) {
        //getLogger()->verbose("WriteChannel_f: " + std::to_string(ch) + " -> f/value: " + std::to_string(value) + "/" + std::to_string(convTxChannel_i(value, ch)));
        writeChannel(convTxChannel_i(value, ch), ch);
    }

    void Drone::writeChannel(int16_t value, int8_t ch) {
        xSemaphoreTake(getTxChMutex(), portMAX_DELAY); // Get write locks to ensure no race conditions
        //getLogger()->verbose("Writechannel is setting sbus value to " + std::to_string(value) + " in ch " + std::to_string(ch));
        _sbusTxData[ch] = value;
        xSemaphoreGive(getTxChMutex());
        //getLogger()->verbose("Exiting writechannel!");
    }

    
    float Drone::clamp(float value, float lbound, float ubound) {
        return min(ubound, max(lbound, value));
    }

    DroneCommand Drone::dequeueCommand() {
        xSemaphoreTake(_semCommandQueueMutex, portMAX_DELAY);   // Ensures only one task accesses this queue at a time

        DroneCommand cmd = _droneCommandQueue.front();
        _droneCommandQueue.pop();

        // Log information on dequeued command
        _logger->inform("Dequeueing drone command: " + cmd.desc);
        _logger->verbose("Command <" + cmd.desc + "> - Type " + std::to_string(cmd.type));
        
        xSemaphoreGive(_semCommandQueueMutex);
        return cmd;
    }

    /* ---------------------------------- Public Member Methods ---------------------------------- */ 

    void Drone::enqueueCommand(DroneCommand cmd) {
        xSemaphoreTake(_semCommandQueueMutex, portMAX_DELAY);   // Ensures only one task accesses this queue at a time
        
        // Log information on enqueued command
        _logger->inform("Enqueing drone command: " + cmd.desc);
        _logger->verbose("Command <" + cmd.desc + "> - Type " + std::to_string(cmd.type));

        _droneCommandQueue.push(cmd);
        xSemaphoreGive(_semCommandQueueMutex);
    }

    // Absolute Channels are (0, 1) whereas normal channels are (-1, 1)
    void Drone::generateAbsChannels() {
        _sbusAbsChannels[0]  = false;
        _sbusAbsChannels[1]  = false;
        _sbusAbsChannels[2]  = true;    // Ch3 Throttle
        _sbusAbsChannels[3]  = false;
        _sbusAbsChannels[4]  = true;    // Ch5 Left Switch
        _sbusAbsChannels[5]  = false;
        _sbusAbsChannels[6]  = false;   // Ch7 Button
        _sbusAbsChannels[7]  = true;    // Ch6 Right Switch
        _sbusAbsChannels[8]  = false;
        _sbusAbsChannels[9]  = false;
        _sbusAbsChannels[10] = false;
        _sbusAbsChannels[11] = false;
        _sbusAbsChannels[12] = false;
        _sbusAbsChannels[13] = false;
        _sbusAbsChannels[14] = false;
        _sbusAbsChannels[15] = false;
    }

    void Drone::generateEStopTx() {
        _sbusEStopTx[0]  = convRxChannel_i(0.5f, 0);
        _sbusEStopTx[1]  = convRxChannel_i(0.5f, 1);
        _sbusEStopTx[2]  = 0;
        _sbusEStopTx[3]  = convRxChannel_i(0.5f, 3);
        _sbusEStopTx[4]  = convRxChannel_i(0.5f, 4);
        _sbusEStopTx[5]  = convRxChannel_i(0.5f, 5);
        _sbusEStopTx[6]  = convRxChannel_i(0.5f, 6);
        _sbusEStopTx[7]  = convRxChannel_i(0.5f, 7);
        _sbusEStopTx[8]  = convRxChannel_i(0.5f, 8);
        _sbusEStopTx[9]  = convRxChannel_i(0.5f, 9);
        _sbusEStopTx[10] = convRxChannel_i(0.5f, 10);
        _sbusEStopTx[11] = convRxChannel_i(0.5f, 11);
        _sbusEStopTx[12] = convRxChannel_i(0.5f, 12);
        _sbusEStopTx[13] = convRxChannel_i(0.5f, 13);
        _sbusEStopTx[14] = convRxChannel_i(0.5f, 14);
        _sbusEStopTx[15] = convRxChannel_i(0.5f, 15);

        _logger->inform("Generating E-Stop Tx Package");
        _logger->verbose("E-Stop Tx: " + formatSbusArray(_sbusEStopTx));
    }

    std::array<int16_t, NUM_CH> Drone::getEStopTx() {
        //_logger->verbose("Returning E-Stop TX");
        return _sbusEStopTx;
    }

    void Drone::enableOperatorControl() {
        if (!_enableOperatorControl) _logger->warn("Operator override enabled");
        _enableOperatorControl = true;
    }
    void Drone::disableOperatorControl() {
        if (_enableOperatorControl) _logger->warn("Operator override reset");
        _enableOperatorControl = false;
    }
    
    void Drone::emergencyStop() {
        if (!_enableEmergencyStop) _logger->fatal("Emergency stop enabled");
        _enableEmergencyStop = true;
    }
    void Drone::resetEmergencyStop() {
        if (_enableEmergencyStop) _logger->warn("Emergency stop reset");
        _enableEmergencyStop = false;
    }

    bool Drone::channelConfirm(int16_t channel, float threshold) {
        return readRxChannel_f(channel) > threshold;
    }

    std::string Drone::getSbusRxArray() {
        std::string formattedArray = formatSbusArray(_sbusRxData);
        _logger->verbose("Getting SBUS Rx Array: " + formattedArray);
        return formattedArray;
    }

    std::string Drone::getSbusTxArray() {
        std::string formattedArray = formatSbusArray(_sbusTxData);
        _logger->verbose("Getting SBUS Tx Array: " + formattedArray);
        return formattedArray;
    }

    

    Drone::Drone(Logger *logger, bfs::SbusRx* sbus_rx, bfs::SbusTx* sbus_tx) {
        _logger = logger;
        _sbusRx = sbus_rx;
        _sbusTx = sbus_tx;
        //_sbusRxData = sbusRxData;
        //_sbusTxData = sbusTxData;

        _semDroneEnableMutex = xSemaphoreCreateBinary();
        _semControlEnableMutex = xSemaphoreCreateBinary();
        _semCommandQueueMutex = xSemaphoreCreateBinary();
        _semTxChMutex = xSemaphoreCreateBinary();
        _semRxChMutex = xSemaphoreCreateBinary();

        xSemaphoreGive(_semDroneEnableMutex);
        xSemaphoreGive(_semControlEnableMutex);
        xSemaphoreGive(_semCommandQueueMutex);
        xSemaphoreGive(_semTxChMutex);
        xSemaphoreGive(_semRxChMutex);

        // Utility functions to initialise arrays
        initUpperLowerBoundArrays();    // Set the upper and lower bounds for all ch arrays
        generateAbsChannels();          // Set the default absolute channels
        generateEStopTx();              // Create packet for E-Stops
        _logger->inform("Drone Class Initialised.");
    }
}
