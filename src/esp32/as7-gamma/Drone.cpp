#include "Drone.h"


namespace AS7 
{
    SemaphoreHandle_t Drone::getSemEnableMutex() { return _sem_enableMutex; }

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
            xSemaphoreGive(_sem_enableMutex);
        } else {
            // log a warning and do nothing
        }
        
    }


    void Drone::start(int core, int priority) {
        
        xTaskCreatePinnedToCore(
        this->Drone::startTaskImpl,                /* Task function. */
        "Drone",              /* name of task. */
        8192,                   /* Stack size of task */
        this,                   /* parameter of the task */
        1, /* priority of the task */
        &th_drone,         /* Task handle to keep track of created task */
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