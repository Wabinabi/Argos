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
    //

    void Drone::navigationTask(void * parameters) { 
        unsigned long finishTime = millis(); // Keeps track of the end time of our current command
        DroneCommand currentCommand;
        for (;;) {
                  
            xSemaphoreTake(getSemDroneEnableMutex(), portMAX_DELAY);

            // Main Block
            if (getHasActiveComamnd()) {
                // Process Command block
                
                switch(currentCommand.type) {

                    case Blind:

                        break;

                    case Guided:
                        break;

                    case Landing:
                        break;

                    case Arm:
                        break;


                }




                setHasActiveCommand(millis() > finishTime); // If we've passed our command duration, we unset the active command
            } else {
                // Get Command Block
                //  As there is no active command, we will attempt to get one and set it up

                if (nextCommandAvailable()) {          // Check if there's a command available
                    if (droneAllowedToFly()) {         // Check if the drone is allowed to fly
                        currentCommand = dequeueCommand();
                        finishTime = currentCommand.duration + millis();
                        getLogger()->inform("Starting new command: " + currentCommand.desc + " for " + std::to_string(currentCommand.duration) + "ms");
                        getLogger()->verbose("Command to finish at " + std::to_string(finishTime) + "ms");

                    } else { // Drone is not allowed to fly yet
                        //_logger->verbose("Drone is not armed yet, waiting for arming...");
                        // This thread runs often and therefore would flood the verbose serial port.
                    }
                    setDroneCommandsCompleted(); // indicate that drone has no commands
                    // When the drone has no commands, we send the e-stop tx packet to the FC, unless there's another one we want to send
                    setSbusTxData(getEStopTx());
                }
            }
            xSemaphoreGive(getSemDroneEnableMutex());
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
        for (;;) {
            xSemaphoreTake(getSemControlEnableMutex(), portMAX_DELAY);

            // If there is available data, update the internally received data
            if (_sbusRx->Read()) {
                setSbusRxData(_sbusRx->ch());
            }

            // Estop and Override Check
            // This may differ depending on your controller
            if (readChannel_f(CH_ESTOP) > 0.4f) { // EStop Threshold
                emergencyStop(); // Toggle EStop
            } else if (readChannel(CH_FLIGHTMODE) > 0.7f) {
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

            // Transmit data to drone
            getSbusTx()->Write();
            xSemaphoreGive(getSemControlEnableMutex());
        }
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
        xTaskCreatePinnedToCore(
        this->Drone::startNavTask,
        "Navigation",  
        8192,
        this,
        1,
        &thDrone,
        configMAX_PRIORITIES);

        xTaskCreatePinnedToCore(
        this->Drone::startCtlTask,
        "Controller",  
        8192,
        this,
        1,
        &thRemote,
        configMAX_PRIORITIES);

        _running = true;
        _logger->verbose("Drone class started");
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
    void Drone::setSbusTxData(std::array<int16_t, NUM_CH> data) {
        xSemaphoreTake(getTxChMutex(), portMAX_DELAY);
            _sbusRxData = data;
        xSemaphoreGive(getTxChMutex());
    }

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
            formattedData += " " + std::to_string(chData[i]);
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

        switch(rampRateType) {

            case RAMPRATE_LINEAR:
                if (min(value + rate, target) < target) {
                    _returnValue = min(value + rate, target);
                } else {
                    _returnValue = target;
                }

                break;

            case RAMPRATE_PROP:
                    _returnValue = (target - value) * rate;
                break;

            default: // Default to no ramping or RAMPRATE_NONE
                _returnValue = value;
        }

        return _returnValue;
    }

    // Returns the corrected integer value for this channel (adjusted for min/max and abs of the channel)
    //  This will only use the values for the TX channels
    int16_t Drone::convChannel_i(float value, int8_t ch) {
        float _value;
        int16_t _adjustedValue;
        if (_sbusAbsChannels[ch]) { // This is an absolute channel. Accepts (0, 1)
            _value = clamp(value, 0, 1);
            _adjustedValue = _sbusTxChLower[ch] + (_sbusTxChUpper[ch] - _sbusTxChLower[ch]) * _value;
        } else {
            _value = clamp(value, -1, 1);
            _value += 1; // Range is now (0, 2)
            _value /= 2; // Range is now (0, 1), we can use the same formula as above.
            _adjustedValue = _sbusTxChLower[ch] + (_sbusTxChUpper[ch] - _sbusTxChLower[ch]) * _value;
        }

        return _adjustedValue;
    }

    // Returns the corrected floating point value for this channel (adjusted for min/max and abs of the channel)
    //  This will only use the values for the RX channels
    float Drone::convChannel_f(int16_t value, int8_t ch) {
        int16_t _value = clamp(value, _sbusRxChUpper[ch], _sbusRxChLower[ch]);
        int16_t _adjustedValue;

        if (_sbusAbsChannels[ch]) { // This is an absolute channel. Returns (0, 1)
            _adjustedValue = (_value - _sbusRxChLower[ch]) / (_sbusRxChUpper[ch] - _sbusRxChLower[ch]);
        } else { // Returns (-1, 1)
            _adjustedValue = (_value - (_sbusRxChLower[ch] * 1.5)) / ((_sbusRxChUpper[ch] - _sbusRxChLower[ch])/2);
        }

        return _adjustedValue;
    }

    int16_t Drone::readChannel(int16_t ch) {
        return _sbusRxData[ch];
    }
    
    float Drone::readChannel_f(int16_t ch) {
        float chValue =convChannel_f(readChannel(ch), ch);
        return chValue;
    }

    void Drone::setChannel(float value, int16_t channel) {
        
        //TODO implement channel setting with semaohore and correct setting
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
    }

    /* ---------------------------------- Public Member Methods ---------------------------------- */ 

    void Drone::enqueueCommand(DroneCommand cmd) {
        xSemaphoreTake(_semCommandQueueMutex, portMAX_DELAY);   // Ensures only one task accesses this queue at a time
        
        // Log information on enqueued command
        _logger->inform("Dequeueing drone command: " + cmd.desc);
        _logger->verbose("Command <" + cmd.desc + "> - Type " + std::to_string(cmd.type));

        _droneCommandQueue.push(cmd);
        xSemaphoreGive(_semCommandQueueMutex);
    }

    void Drone::generateAbsChannels() {
        _sbusAbsChannels[0]  = true;    // Throttle
        _sbusAbsChannels[1]  = false;
        _sbusAbsChannels[2]  = false;
        _sbusAbsChannels[3]  = false;
        _sbusAbsChannels[4]  = false;
        _sbusAbsChannels[5]  = false;
        _sbusAbsChannels[6]  = false;
        _sbusAbsChannels[7]  = true;
        _sbusAbsChannels[8]  = true;
        _sbusAbsChannels[9]  = true;
        _sbusAbsChannels[10] = true;
        _sbusAbsChannels[11] = true;
        _sbusAbsChannels[12] = true;
        _sbusAbsChannels[13] = true;
        _sbusAbsChannels[14] = true;
        _sbusAbsChannels[15] = true;
    }

    void Drone::generateEStopTx() {
        _sbusEStopTx[0]  = 0;
        _sbusEStopTx[1]  = convChannel_i(0.5f, 1);
        _sbusEStopTx[2]  = convChannel_i(0.5f, 2);
        _sbusEStopTx[3]  = convChannel_i(0.5f, 3);
        _sbusEStopTx[4]  = convChannel_i(0.5f, 4);
        _sbusEStopTx[5]  = convChannel_i(0.5f, 5);
        _sbusEStopTx[6]  = 0;
        _sbusEStopTx[7]  = 0;
        _sbusEStopTx[8]  = 0;
        _sbusEStopTx[9]  = 0;
        _sbusEStopTx[10] = 0;
        _sbusEStopTx[11] = 0;
        _sbusEStopTx[12] = 0;
        _sbusEStopTx[13] = 0;
        _sbusEStopTx[14] = 0;
        _sbusEStopTx[15] = 0;

        _logger->inform("Generating E-Stop Tx Package");
        _logger->verbose("E-Stop Tx: " + formatSbusArray(_sbusEStopTx));
    }

    std::array<int16_t, NUM_CH> Drone::getEStopTx() {
        _logger->verbose("Returning E-Stop TX");
        return _sbusEStopTx;
    }

    void Drone::enableOperatorControl() {
        _logger->warn("Operator override enabled");
        _enableOperatorControl = true;
    }
    void Drone::disableOperatorControl() {
        _logger->warn("Operator override reset");
        _enableOperatorControl = false;
    }
    
    void Drone::emergencyStop() {
        _logger->fatal("Emergency stop enabled");
        _enableEmergencyStop = true;
    }
    void Drone::resetEmergencyStop() {
        _logger->warn("Emergency stop reset");
        _enableEmergencyStop = false;
    }

    bool Drone::channelConfirm(int16_t channel, float threshold) {
        return readChannel_f(channel) > threshold;
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

    void Drone::writeChannel(int16_t value, int8_t ch) {
        xSemaphoreTake(getTxChMutex(), portMAX_DELAY); // Get write locks to ensure no race conditions
        _sbusTxData[ch] = value;
        xSemaphoreGive(getTxChMutex());
    }

    float Drone::getChannel(int16_t channel) {
        return readChannel_f(channel);
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