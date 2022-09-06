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
        static void startTaskImpl(void*);   // Task implementation for classes
        void mainTask(void* parameters);    // The threaded taskk  

        bool _running = false;

        SemaphoreHandle_t _semEnableMutex;      // Enables/Disables main drone task
        SemaphoreHandle_t getSemEnableMutex();  // Returns the enable mutex

        Logger* _logger;

        bfs::SbusRx* _sbusRx;   // SBUS Receive Channel Object
        bfs::SbusTx* _sbusTx;   // SBUS Transmit Channel Object
        std::array<int16_t, bfs::SbusRx::NUM_CH()>* _sbusData;  // Array containing sbus data

        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusChLower;    // Lower bounds for each SBUS channel
        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusChUpper;    // Upper bounds for each SBUS Channel

        void initUpperLowerBoundArrays();   // Sets UBound and LBound array to default

        // Writes a floating point value (-1 to 1) to the SBUS Channel
        //  -1 represents the lower bound, 1 represents upper bound
        //  Any out-of-bound values will be clamped to (-1, 1)
        int writeChannel(float value, int16_t channel);

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
