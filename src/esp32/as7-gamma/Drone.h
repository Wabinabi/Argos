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

#define SBUS_CHANNEL_LOWER 0        // Default lower bound for sbus channels
#define SBUS_CHANNEL_UPPER 4096     // Default upper bound for sbus channels

namespace AS7 
{
    class Drone
    {
    private:
        TaskHandle_t thDrone;
        static void startTaskImpl(void*);
        void mainTask(void* parameters);

        bool _running = false;
        SemaphoreHandle_t _semEnableMutex;
        SemaphoreHandle_t getSemEnableMutex();

        Logger* _logger;

        bfs::SbusRx* _sbusRx;
        bfs::SbusTx* _sbusTx;
        std::array<int16_t, bfs::SbusRx::NUM_CH()>* _sbusData;

        // Upper and Lower bounds for each channel.
        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusChLower;
        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusChUpper;

    public:
        Drone(Logger *logger, bfs::SbusRx* sbus_rx, bfs::SbusTx* sbus_tx, std::array<int16_t, bfs::SbusRx::NUM_CH()>* sbus_data);

        bool operatorAcknowledge(int channel=1);
        void eStop();

        void start(int core=1, int priority=configMAX_PRIORITIES);
        void pause();
        void resume();

        bool channelConfirm(int ch, int threshold);

    };
}


#endif
