/**
 * TODO: THIS BANNER
 * 
 * AS7 Drone
 * This class encapsulates talking to AS7
 * 
 * The main program controls the state that AS7 is in
 * This class represents the inputs/outputs for AS7
 * 
 * 
 */


#pragma once
#ifndef AS7DRONE
#define AS7DRONE

#include <Arduino.h>
#include <sbus.h>     // SBUS Communication Library with FC

#include "SdLogger.h" // Use the SD Logger to share messages

namespace AS7 
{
    class Drone
    {
    private:
        TaskHandle_t th_drone;
        static void startTaskImpl(void*);
        void mainTask(void* parameters);

        bool _running = false;
        SemaphoreHandle_t _sem_enableMutex;
        SemaphoreHandle_t getSemEnableMutex();

        Logger* _logger;

        bfs::SbusRx* _sbus_rx;
        bfs::SbusTx* _sbus_tx;
        std::array<int16_t, bfs::SbusRx::NUM_CH()>* _sbus_data;

    public:
        Drone(Logger *logger, bfs::SbusRx* sbus_rx, bfs::SbusTx* sbus_tx, std::array<int16_t, bfs::SbusRx::NUM_CH()>* sbus_data);

        bool operatorAcknowledge(int channel=1);
        void eStop();

        void start(int core=1, int priority=configMAX_PRIORITIES);
        void pause();
        void resume();

    };
}


#endif
