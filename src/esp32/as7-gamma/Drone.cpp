#include "Drone.h"


namespace AS7 
{
    SemaphoreHandle_t Drone::getSemEnableMutex() { return _semEnableMutex; }

    void Drone::startTaskImpl(void* _this) {
        ((Drone*)_this)->mainTask(NULL);
    }

    void Drone::mainTask(void * parameters) { 
        for (;;) {
            xSemaphoreTake(getSemEnableMutex(), portMAX_DELAY);
            
            xSemaphoreGive(getSemEnableMutex());
        }
    }

    // We're going with the assumption that there's only one logger
    //  For now we won't be implementing protection here since it could cause debugging issues
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
        this->Drone::startTaskImpl,                /* Task function. */
        "Drone",              /* name of task. */
        8192,                   /* Stack size of task */
        this,                   /* parameter of the task */
        1, /* priority of the task */
        &thDrone,         /* Task handle to keep track of created task */
        1);                     /* pin task to core 1 */

        _running = true;

    }

    Drone::Drone(Logger* logger,bfs::SbusRx* sbus_rx, bfs::SbusTx* sbus_tx, std::array<int16_t, bfs::SbusRx::NUM_CH()>* sbus_data) {
        _logger = logger;
        _sbus_rx = sbus_rx;
        _sbus_tx = sbus_tx;
        _sbus_data = sbus_data;

    }

}