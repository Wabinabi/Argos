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

        SemaphoreHandle_t _semEnableMutex;          // Enables/Disables main drone task
        SemaphoreHandle_t getSemEnableMutex();      // Returns the enable mutex

        SemaphoreHandle_t _semWriteChannelMutex();  // Mutex Lock for the tx data array
        SemaphoreHandle_t getWriteChannelMutex();   // Returns the write mutex for threading implementation

        Logger* _logger;

        bfs::SbusRx* _sbusRx;   // SBUS Receive Channel Object
        bfs::SbusTx* _sbusTx;   // SBUS Transmit Channel Object

        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusRxData;    // Array of data received from the Radio Control
        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusTxData;    // Array of data to transmit to the Flight Controller

        // Channels that will be transmitted to the drone
        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusTxChLower;    // Lower bounds for SBUS Transmit channels
        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusTxChUpper;    // Upper bounds for SBUS Transmit Channels

        // Channels that are received from the transmitter
        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusRxChLower;    // Lower bounds for SBUS Receiver channels
        std::array<int16_t, bfs::SbusRx::NUM_CH()> _sbusRxChUpper;    // Upper bounds for SBUS Receiver Channels

        void initUpperLowerBoundArrays();   // Sets UBound and LBound array to default

        void writeChannel(int16_t value, int8_t ch);    // writes the value into the sbus transmit channel
        int16_t readChannel(int16_t ch);                // Reads the value from the channel
        float readChannel_f(int16_t ch);                // Reads the floating point value from the channel, adjusted for upper and lower bounds

        float clamp(float value, float lbound, float ubound);   // Returns values inside of upper bound and lower bound.

        std::string formatSbusArray(std::array<int16_t, bfs::SbusRx::NUM_CH()> chData);    // Returns the channels in a formatted string        

    public:
        
        Drone(Logger *logger, bfs::SbusRx* sbus_rx, bfs::SbusTx* sbus_tx);

        bool operatorAcknowledge(int channel=1);
        void eStop();

        void start(int core=1, int priority=configMAX_PRIORITIES);
        void pause();
        void resume();

        bool channelConfirm(int16_t ch, int16_t threshold);

        std::string getSbusRxArray();
        std::string getSbusTxArray();

        // Writes a floating point value (-1 to 1) to the SBUS Channel
        //  -1 represents the lower bound, 1 represents upper bound
        //  Any out-of-bound values will be clamped to (-1, 1)
        void setChannel(float value, int16_t channel);
    };
}


#endif
