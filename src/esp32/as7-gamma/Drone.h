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
#define SBUS_CHANNEL_UPPER 2056     // Default upper bound for sbus channels
#define NUM_CH 16                   // Number of SBUS channels. Always 16. Equivalent to bfs::SbusRx::NUM_CH()
#define DOF 6                       // Degrees of freedom for the drone. 0-5 represent x, y, z, roll (rl), pitch (pt), yaw (yw) (Euler ZYX Convention)

// Thread update frequencies (to reduce starvation of the watchdog)
//  Feed your watchdogs, people! They do important household chores!
#define NAV_FREQ 250    // Navigation update rate, Hz (Default: 250 Hz)
#define CTL_FREQ 1000   // Controller update rate, Hz (Default: 250 Hz)

// Channel definitions
//  These channels index from ZERO. Ch[0] = CH1!
#define CH_THROTTLE     2   // Left stick y axis (starts from 0)
#define CH_YAW          3   // Left stick x axis
#define CH_STRAIGHT     1   // Right stick y axis
#define CH_STRAFE       0   // Right stick x axis
//#define CH_BUTTON1      6   // Button on middle of controller
#define CH_SW1          7   // Right toggle switch
#define CH_FLIGHTMODE   4   // Left toggle switch
#define CH_ESTOP        6   // Other button?

#define STATUS_UPDATE_DELAY 250 // Number of updates to wait before sending controller status

#define THROTTLE_LIMIT 0.5f

// Cv = Control Value, Pv = Present Value. Use CV to control PV
#define RAMPRATE_NONE       0   // No ramp rate.            Pv = Cv
#define RAMPRATE_LINEAR     1   // Linear ramp rate,        Pv += Constant until Pv > Cv
#define RAMPRATE_PROP       2   // Proportional Ramp Rate   Pv = (Cv - Pv) * Constant

namespace AS7 
{
    enum DroneCommandType {Blind, Guided, Landing, Arm};

    /**
     * @brief Ecapsulates commands to be carried out by drone
     * 
     * Characterised by type `{Blind, Guided, Landing, Arm}`, drone commands make up the basic unit for controlling the connected drone.
     * 
     * ### Attribute Characteristics
     * 
     * Under `blind` commands, only the velocities of the controllable channels are used. These values are written directly to the channel. Command convention is prefixing `v_` for velocity control and `p_` for position control. The available axes of freedom are:
     * 
     * 
     * Cartesian co-ordinates where the origin is the starting point of the drone
     * * `x`, `y`, `z`
     * 
     * Axes of rotation -- pitch (tilt forward/back), roll (wiggle side to side), yaw (turn normally)
     * * `rl`, `pt`, `yw`
     * 
     * Hence, to control the forward velocity of the drone, the command is:
     * 
     * ```
     * DroneCommand aCommand;
     * aCommand.v_x = 0.5f
     * ```
     * 
     * ### Command Ramping and Ramp Types
     * 
     * For all channels, `drone.cpp` defines a set 'ramp rate' depending on the application and control desired for the drone. Ramp rates are often found in motor applications where a motor has to be speed up at a certain rate to reduce damage or shock. In the case of drones, a linear ramp rate helps with ensuring the commands are carried out smoothly. Additionally, they allow for less rigid movement of the drone as as the channels transition smoothly and thus keep the channels alive, rather than emulating a disconnect. This has the added advantage of ensuring that the frame is not dropped due to suddden shifts in values.
     * Ramp rates are defined by the `RAMPRATE` enumeration. The following ramping methods have been implemented:
     * 
     * * `RAMPRATE_NONE` - The channel is instantly set to the target value
     * * `RAMPRATE_LINEAR` - The channel is increased linearly by the ramping value
     * * `RAMPRATE_PROP` - Tha channel is increased proportionately to the amount of travel left. If the target is far, the ramping is faster. If the target is close, the ramping is slower.
     * 
     * ### Channel values and non-pure values
     * 
     * In testing, it was noted that the best results came from using channel values that were between 0.1f and 0.9f. User discretion is advised when setting channel values to write. 
     * 
     * ### Blind Commands
     * 
     * ```
     * DroneCommand.v_z = 0.1f
     * ```
     * 
     * This will emulate the user pulling up the 'throttle' stick to about 10%.
     * 
     * ### Guided Commands
     * 
     * Guided commands aim to position-control the drone using the on-board ultrasonic sensors.
     * 
     * In the case of AS7, the left-right channels will aim to avoid obstacles whilst the z (height) channel will aim to keep the drone a constant height about the ground.
     * 
     * #### Proportional Control Guided Commands
     * 
     * The current implementation of the z-height guided command is a form of proportional control from control theory. The amount of thrust applied to the TX Thrust Channel is proprtional to `target-current_value` or error measured in the Z-axis. This is multiplied by a proportionality constant and re-evaluated in the next loop.
     * Fine-tuning of the proportionality constant is recommended for different propellers and drone configurations
     * 
     * ### Channels not controllable by drone
     * 
     * * Roll
     * * Pitch
     * 
     */
    typedef struct {
        /// @brief  Defines the main function of the command
        DroneCommandType type;              // Blind = Purely a drone command, Guided = Assisted with sensors
        /// @brief A description for logging and verbosity
        std::string desc;                   // A description for the logger
        /// @brief A float for each channel from (-1 1) for normal channels and (0, 1) for absolute channels
        std::array<float, NUM_CH> channels; // A float for each channel from (-1, 1)
        /// @brief  The duration of the command in milliseconds
        int duration;                       // in ms
        /// @brief The rate multiplier to adjust ramping speed if applicable.
        float rateMultiplier = 1.0f;          // Can be thought of as "aggressiveness" of controls

        // Prefix P = Position; V = Velocity. Units on per-member basis and are *convention* (not checked)

        /// @brief Position control in the forward axis
        float p_x;        // Position to hold (some distance unit tbc)
        /// @brief Position control in the sideways axis
        float p_y;
        /// @brief Position control of the height of the drone
        float p_z;
        /// @brief Velocity control in the forward axis
        float v_x;      // Velocity to hold (-1 to 1) floating point value
        /// @brief Velocity control in the sideways axis
        float v_y;      // e.g. 0.1 is equivalent to 10% forward thrust
        /// @brief Upwards thrust
        float v_z;
        /// @brief Position control in the roll axis. Nod directly controllable on a drone.
        float p_rl;       // Not directly controllable on a drone
        /// @brief Position control in the pitch axis. Nod directly controllable on a drone.
        float p_pt;       // Not directly controllable on a drone
        /// @brief Position control in the yaw axis. Uses the compass to control heading
        float p_yw;
        /// @brief Velocity control in the roll axis. Nod directly controllable on a drone.
        float v_rl;     // Not directly controllable on a drone
        /// @brief Velocity control in the pitch axis. Nod directly controllable on a drone.
        float v_pt;     // Not directly controllable on a drone
        /// @brief Velcocity control in the yaw axis. Rotates drone left/right.
        float v_yw;
        /// @brief Indicates if the drone should be recording data in this command.
        bool dataRecording; // indicates if the drone should record data, used to synchronise sensors and current drone state
    } DroneCommand;

    class Drone
    {
    private:

    public:
        TaskHandle_t thDrone;
        TaskHandle_t thRemote;

        /// @brief Task implementation for classes. Only used by the task thread
        static void startNavTask(void*);   // Task implementation for classes
        /// @brief Task implementation for classes. Only used by the task thread
        static void startCtlTask(void*);
        /// @brief Task implementation for classes. Only used by the task thread
        void navigationTask(void* parameters);    // The threaded task
        /// @brief Task implementation for classes. Only used by the task thread
        void controllerTask(void* parameters);

        std::queue<DroneCommand> _droneCommandQueue;

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
        Logger* getLogger();

        bfs::SbusRx* _sbusRx;   // SBUS Receive Channel Object
        bfs::SbusTx* _sbusTx;   // SBUS Transmit Channel Object

        std::array<int16_t, NUM_CH> _sbusRxData;    // Array of data received from the Radio Control
        std::array<int16_t, NUM_CH> _sbusTxData;    // Array of data to transmit to the Flight Controller

        std::array<bool, NUM_CH> _sbusAbsChannels;      // When true, the channel is (0, 1). Otherwise channels default to (-1, 1). 
        std::array<bool, NUM_CH> getSbusAbsChannels();  // Returns an array which defines if a channel is absolute (true) or not (false).
        void generateAbsChannels();                     // Sets the default Absolute Channels
        
        int _controllerStatusCount = 0;
        bool getControllerStatusCount();


        // Channels that will be transmitted to the drone
        std::array<int16_t, NUM_CH> _sbusTxChLower;    /** Lower bounds for SBUS Transmit channels */
        std::array<int16_t, NUM_CH> _sbusTxChUpper;    /** Upper bounds for SBUS Transmit Channels */

        // Channels that are received from the transmitter
        std::array<int16_t, NUM_CH> _sbusRxChLower;    // Lower bounds for SBUS Receiver channels
        std::array<int16_t, NUM_CH> _sbusRxChUpper;    // Upper bounds for SBUS Receiver Channels

        std::array<int16_t, NUM_CH> _sbusEStopTx;      // Data to be written in case of an E-Stop. Not all channels are to be zeroed! 

        // Methods for getting and setting data for task theads
        inline std::array<int16_t, NUM_CH> getSbusRxData() {return _sbusRxData; }
        inline std::array<int16_t, NUM_CH> getSbusTxData() {return _sbusTxData; }
        void setSbusRxData(std::array<int16_t, NUM_CH> data);
        void setSbusTxData(std::array<int16_t, NUM_CH> data);

        void initUpperLowerBoundArrays();   // Sets UBound and LBound array to default

        void writeChannel(int16_t value, int8_t ch);    // writes the value into the sbus transmit channel
        void writeRxChannel_f(float value, int8_t ch);    // writes the value into the sbus transmit channel
        void writeTxChannel_f(float value, int8_t ch);    // writes the value into the sbus transmit channel

        int16_t convRxChannel_i(float value, int8_t ch);  // Returns the adjusted int16_t value for that channel
        float convRxChannel_f(int16_t value, int8_t ch);  // Returns the adjusted int16_t value for that channel

        int16_t convTxChannel_i(float value, int8_t ch);  // Returns the adjusted int16_t value for that channel
        float convTxChannel_f(int16_t value, int8_t ch);  // Returns the adjusted int16_t value for that channel

        int16_t readRxChannel(int16_t ch);              // Reads the value from the channel
        float readRxChannel_f(int16_t ch);              // Reads the floating point value from the channel, adjusted for upper and lower bounds

        int16_t readTxChannel(int16_t ch);              // Reads the current value being written to the controller
        float readTxChannel_f(int16_t ch);              // Returns the current value as a floating point number

        // Helper/Utility functions
        float clamp(float value, float lbound, float ubound);                           // Returns values inside of upper bound and lower bound.
        std::string formatSbusArray(std::array<int16_t, NUM_CH> chData);                // Returns the channels in a formatted string  
        float rampValue(float value, float target = 0, float rate = 0, int rampRateType = RAMPRATE_LINEAR);   // Returns the next ramped value depending on ramp type

        // Combines rampValue with current channel data for ramping.
        void rampChannel(float target, int8_t ch, float rate, int rampRateType = RAMPRATE_LINEAR);

        bfs::SbusRx* getSbusRx();   // Returns SBUS RX object for task implementation
        bfs::SbusTx* getSbusTx();   // Returns SBUS TX object for task implementation

        
        bool _hasArmed = false;         // Remembers if the drone has undergone an arming process
        bool _armingAllowed = false;    // Set by main program. Once allowed, drone will start processing instructions
        inline void setDroneHasArmed() {_hasArmed = true;}
        inline bool getDroneHasArmed() {return _hasArmed; }

        bool _droneCommandsStarted = false;     // Indicates if the drone has started processing commands
        bool _droneCommandsCompleted = false;   // Indicates that there are no commands left (queue is empty)
        bool _droneHasActiveCommand = false;
        inline bool getHasActiveComamnd() const {return _droneHasActiveCommand;}
        inline void setHasActiveCommand(bool value) {_droneHasActiveCommand = value;}
        inline void setDroneCommandsStarted() {_droneCommandsStarted = true;}           // Latching check that the drone has started commands
        inline void setDroneCommandsCompleted() {_droneCommandsCompleted = true;}       // Latching check that the rone has completed commands. If commands haven't stareted, then this indicates empty.
        inline bool nextCommandAvailable() {return _droneCommandQueue.size() != 0;}     // Checks if command queue size is not equal to zero.

        bool _running = false;                  // Indicates current state of main drone task
        bool _enableOperatorControl = false;    // When enabled, remote control commands are passed directly to drone from RX to TX
        bool _enableEmergencyStop = false;      // When enabled, all TX channels are set to 0
        bool _armingComplete = false;

    
        
        Drone(Logger *logger, bfs::SbusRx* sbus_rx, bfs::SbusTx* sbus_tx);
        bool channelConfirm(int16_t channel=1, float threshold=0.7f);    // Returns true if the channel above threshold. e.g. button press
        
        // Drone status is indicated by an int, though it could be indicated by an enum later on with DEFINEs
        //  Current statuses could be drone_starting, drone_waitin0gdrone_in_progress, drone_estop, drone_operator_over, drone_finished
        //  Not sure how the internal mechanism could work -- this could be a bunmch of bools with increasnig preference for noe another another? maybe we shouldn't even consider status inside the drone *command* class
        int droneStatus();

        inline bool getEnableOperatorControl() {return _enableOperatorControl; }        // Returns operator control bit
        inline bool getEnableEmergencyStop() {return _enableEmergencyStop; }          // Returns e-stop bit
        inline bool getDroneArmComplete() {return _armingComplete; }

        // Assume everything is far so we don't shock the drone into a position
        
        float usFront = 200;     // Distance measurement from the front US sensor
        float usBack = 200;      // Distance measurement from the back US sensor
        float usLeft = 200;      // Distance measurement from the left US sensor
        float usRight = 200;     // Distance measurement from the right US sensor
        float usUp = 200;        // Distance measurement from the upwards US sensor
        float usDown = 200;      // Distance measurement from the downwards US sensor

        inline float getUsFront() {return usFront; };
        inline float getUsBack() {return usBack; };
        inline float getUsLeft() {return usLeft; };
        inline float getUsRight() {return usRight; };
        inline float getUsUp() {return usUp; };
        inline float getUsDown() {return usDown; };

        float compassHeading;
        inline float getCompassHeading() {return compassHeading; };
        


        // Main thread control
        //  Operates both the controller and navigator threads
        void start(int core=1, int priority=3);
        void pause();
        void resume();

        void enqueueCommand(DroneCommand cmd);  // Adds command to drone queue
        DroneCommand dequeueCommand();          // Remove drone command, returns command from queue

        inline void allowArming() {_armingAllowed = true;}  // Allows drone to start processing commands
        inline bool droneAllowedToFly() const {return _armingAllowed;} // Returns _armingAllowed bit
        inline bool droneHasArmed() const {return _hasArmed;}          // Returns if dorn has armed previously

        void enableOperatorControl();   // Enables pass-through from RX to TX. Latching
        void disableOperatorControl();

        void emergencyStop();
        void resetEmergencyStop();

        bool _dataGatheringEnabled = false;
        inline void setDataGathering(bool value) {_dataGatheringEnabled = value; }
        inline bool recordingEnabled() {return _dataGatheringEnabled;} // Returns true if the current drone command is requesting data gathering

        void generateEStopTx();
        std::array<int16_t, NUM_CH> getEStopTx();

        std::string getSbusRxArray();
        std::string getSbusTxArray();
    };
}


#endif
