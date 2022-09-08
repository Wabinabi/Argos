#include "Drone.h"


namespace AS7 
{
    /* ---------------------------------- Task Methods ---------------------------------- */ 
    
    void Drone::startNavTask(void* _this) {
        ((Drone*)_this)->navigationTask(NULL);
    }

    void Drone::navigationTask(void * parameters) { 
        for (;;) {
            xSemaphoreTake(getSemEnableMutex(), portMAX_DELAY);
            
            xSemaphoreGive(getSemEnableMutex());
        }
    }

    void Drone::startCtlTask(void* _this) {
        ((Drone*)_this)->controllerTask(NULL);
    }

    void Drone::controllerTask(void * parameters) { 
        for (;;) {
        }
    }

    

    // hello
    // add enqueue
    // and stack
    // add reader
    // add sbus writer (always writing to sbus)
    // add things that write to sbus -- sbus writer therefore needs semaphore for WRITE not READ
    // add functions to change whats writetn to sbus, syz and ch
    // and then simpler fuunctions like things to do (raise up and fly)
    // add in tuning for flight mode and stuff

    // add amounts and variables
    // add tuning adjustments for min-maxing the amount
    //  -> as in scalaing

    // add operator override
    // add operttor estop

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
            // log a warning and do nothing
        } else {
            _running = false;
            xSemaphoreTake(getSemEnableMutex(), portMAX_DELAY);
        }
    }

    void Drone::resume() {
        if (!_running) {
            _running = true;
            xSemaphoreGive(_semEnableMutex);
        } else {
            // log a warning and do nothing
        }
        
    }

    SemaphoreHandle_t Drone::getSemEnableMutex() { return _semEnableMutex; }

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

    std::string Drone::formatSbusArray(std::array<int16_t, NUM_CH> chData) {
        std::string formattedData = "";
        for (int i = 0; i < NUM_CH; i++) {
            formattedData += " " + std::to_string(chData[i]);
        }
        return formattedData;
    }

    int16_t Drone::readChannel(int16_t ch) {
        return _sbusRxData[ch];
    }
    float Drone::readChannel_f(int16_t ch) {
        float chValue = (readChannel(ch) - _sbusRxChLower[ch]) / (_sbusRxChUpper[ch] - _sbusRxChLower[ch]);
        return chValue;
    }

    void Drone::setChannel(float value, int16_t channel) {
        float _value = clamp(value, -1, 1);
    }
    
    float Drone::clamp(float value, float lbound, float ubound) {
        return min(ubound, max(lbound, value));
    }

    /* ---------------------------------- Public Member Methods ---------------------------------- */ 

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

    std::string Drone::getSbusRxArray() {
        std::string formattedArray = formatSbusArray(_sbusRxData);
        _logger->verbose("SBUS Rx Array: " + formattedArray);
        return formattedArray;
    }

    std::string Drone::getSbusTxArray() {
        std::string formattedArray = formatSbusArray(_sbusTxData);
        _logger->verbose("SBUS Tx Array: " + formattedArray);
        return formattedArray;
    }

    void Drone::writeChannel(int16_t value, int8_t ch) {
        xSemaphoreTake(getWriteChannelMutex(), portMAX_DELAY); // Get write locks to ensure no race conditions
        _sbusTxData[ch] = value;
        xSemaphoreGive(getWriteChannelMutex());
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

        _semEnableMutex = xSemaphoreCreateBinary();
        initUpperLowerBoundArrays();
    }
}