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
#include <vector>

#include "SdLogger.h" // Use the SD Logger to share messages

#define SBUS_CHANNEL_LOWER 0        // Default lower bound for sbus channels
#define SBUS_CHANNEL_UPPER 4096     // Default upper bound for sbus channels
#define NUM_CH 16                   // Number of SBUS channels. Always 16. Equivalent to bfs::SbusRx::NUM_CH()
#define DOF 6 // Degrees of freedom for the drone. 0-5 represent x, y, z, roll (rl), pitch (pt), yaw (yw) (Euler ZYX Convention)


namespace AS7 
{
    enum DroneCommandType {Blind, Guided, Landing};

    // Drone Command Structure/Format
    //  Two types: Blind and Guided, set by enum DroneCommandType
    //  Blind commands only refer to the channel array and duration
    //  Guided commands will attempt to use on-board sensors to control the drone
    //
    // Landing is a special type where the drone will lower thrust just below its known weight to land
    //  If possible, it will use the bottom sensors to guide landing
    //  Landing is equivalent to setting v_y to some pre-defined value in blind mode.
    //
    // Drone commands are enqueued to the drone, and will be executed FIFO.
    //
    // Note: Blind Commands can also be used as buttons and inputs to the FC, not just for navigation
    //
    typedef struct {
        DroneCommandType type;              // Blind = Purely a drone command, Guided = Assisted with sensors
        std::string desc;                   // A description for the logger
        std::array<float, NUM_CH> channels; // A float for each channel from (-1, 1)
        int duration;                       // in ms

        // Prefix P = Position; V = Velocity. Units on per-member basis and are *convention* (not checked)
        int p_x;    // Position to hold (some distance unit tbc)
        int p_y;
        int p_z;
        float v_x;    // Velocity to hold (-1 to 1) floating point value
        float v_y;    // e.g. 0.1 is equivalent to 10% forward thrust
        float v_z;
        int p_rl;
        int p_pt;
        int p_yw;
        float v_rl;
        float v_pt;
        float v_yw;
    } DroneCommand;

    class Drone
    {
    private:
        TaskHandle_t thDrone;
        TaskHandle_t thRemote;
        static void startNavTask(void*);   // Task implementation for classes
        static void startCtlTask(void*);
        void navigationTask(void* parameters);    // The threaded task
        void controllerTask(void* parameters);

        std::queue<DroneCommand> _droneCommandQueue;

        bool _running = false;                  // Indicates current state of main drone task
        bool _enableOperatorControl = false;    // When enabled, remote control commands are passed directly to drone from RX to TX
        bool _enableEmergencyStop = false;      // When enabled, all TX channels are set to 0

        bool getEnableOperatorControl();
        bool getEnableEmergencyStop();

        SemaphoreHandle_t _semDroneEnableMutex;          // Enables/Disables main drone task
        SemaphoreHandle_t _semControlEnableMutex;        // Enables/Disables main drone task
        SemaphoreHandle_t getSemDroneEnableMutex();      // Returns the enable mutex
        SemaphoreHandle_t getSemControlEnableMutex();    // Returns the enable mutex

        SemaphoreHandle_t _semTxChMutex;  // Mutex Lock for the tx data array
        SemaphoreHandle_t getTxChMutex();   // Returns the write mutex for threading implementation

        SemaphoreHandle_t _semRxChMutex;  // Mutex Lock for the rx data array
        SemaphoreHandle_t getRxChMutex();   // Returns the write mutex for threading implementation

        SemaphoreHandle_t _semCommandQueueMutex;    // Mutex lock for drone command queue
        SemaphoreHandle_t getCommandQueueMutex();

        Logger* _logger;

        bfs::SbusRx* _sbusRx;   // SBUS Receive Channel Object
        bfs::SbusTx* _sbusTx;   // SBUS Transmit Channel Object

        std::array<int16_t, NUM_CH> _sbusRxData;    // Array of data received from the Radio Control
        std::array<int16_t, NUM_CH> _sbusTxData;    // Array of data to transmit to the Flight Controller

        // Channels that will be transmitted to the drone
        std::array<int16_t, NUM_CH> _sbusTxChLower;    // Lower bounds for SBUS Transmit channels
        std::array<int16_t, NUM_CH> _sbusTxChUpper;    // Upper bounds for SBUS Transmit Channels

        // Channels that are received from the transmitter
        std::array<int16_t, NUM_CH> _sbusRxChLower;    // Lower bounds for SBUS Receiver channels
        std::array<int16_t, NUM_CH> _sbusRxChUpper;    // Upper bounds for SBUS Receiver Channels

        std::array<int16_t, NUM_CH> _sbusEStopTx;      // Data to be written in case of an E-Stop. Not all channels are to be zeroed! 

        // Methods for getting and setting data for task theads
        std::array<int16_t, NUM_CH> getSbusRxData();
        std::array<int16_t, NUM_CH> getSbusTxData();
        void setSbusRxData(std::array<int16_t, NUM_CH> data);
        void setSbusTxData(std::array<int16_t, NUM_CH> data);

        void initUpperLowerBoundArrays();   // Sets UBound and LBound array to default

        void writeChannel(int16_t value, int8_t ch);    // writes the value into the sbus transmit channel
        int16_t readChannel(int16_t ch);                // Reads the value from the channel
        float readChannel_f(int16_t ch);                // Reads the floating point value from the channel, adjusted for upper and lower bounds

        DroneCommand dequeueCommand();  // Remove drone command, returns command from queue

        float clamp(float value, float lbound, float ubound);   // Returns values inside of upper bound and lower bound.

        std::string formatSbusArray(std::array<int16_t, NUM_CH> chData);    // Returns the channels in a formatted string  

        bfs::SbusRx* getSbusRx();
        bfs::SbusTx* getSbusTx();

    public:
        
        Drone(Logger *logger, bfs::SbusRx* sbus_rx, bfs::SbusTx* sbus_tx);
        bool channelConfirm(int16_t channel=1, float threshold=0.7f);    // Returns true if the channel above threshold. e.g. button press
        
        // Drone status is indicated by an int, though it could be indicated by an enum later on with DEFINEs
        //  Current statuses could be drone_starting, drone_waitin0gdrone_in_progress, drone_estop, drone_operator_over, drone_finished
        //  Not sure how the internal mechanism could work -- this could be a bunmch of bools with increasnig preference for noe another another? maybe we shouldn't even consider status inside the drone *command* class
        int droneStatus();

        // Main theread control
        //  Operates both the controller and navigator threads
        void start(int core=1, int priority=configMAX_PRIORITIES);
        void pause();
        void resume();

        void enqueueCommand(DroneCommand cmd);

        void enableOperatorControl();   // Enables pass-through from RX to TX. Latching
        void disableOperatorControl();

        void emergencyStop();
        void resetEmergencyStop();

        void generateEStopTx();
        std::array<int16_t, NUM_CH> getEStopTx();

        std::string getSbusRxArray();
        std::string getSbusTxArray();

        // Writes a floating point value (-1 to 1) to the SBUS Channel
        //  -1 represents the lower bound, 1 represents upper bound
        //  Any out-of-bound values will be clamped to (-1, 1)
        void setChannel(float value, int16_t channel);
        float getChannel(int16_t channel);
    };
}


#endif
