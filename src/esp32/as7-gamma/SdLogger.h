/**
 * This is AS7's Logger. It prints to serial and SD card. 
 * The goal of this logger is to tie logging and comms into one location and delegate to a thread
 */
#pragma once
#ifndef AS7SDLOGGING_H
#define AS7SDLOGGING_H

#include <Arduino.h>
#include <queue>
#include <SPI.h>
#include <SD.h>

#include <map>

// Defines the maximum level of messages sent through the serial port
// e.g. a level of WARNING (3) will only allow warnings, errors, and fatal issues to be sent to the serial port.
#define LOG_LEVEL_SILENT    0
#define LOG_LEVEL_FATAL     1
#define LOG_LEVEL_ERROR     2
#define LOG_LEVEL_WARNING   3
#define LOG_LEVEL_INFORM    4
#define LOG_LEVEL_VERBOSE   5
#define CS_PIN 5

/// @brief Logger update rate in Hertz
#define LOGGER_FREQ 100      // Update rate in Hertz

#define PLOTTER_ENABLE false // only prints plots, for testing.

/// @brief Disables SD output for testing
#define SD_DISABLED false

namespace AS7 
{
    /**
     * @brief The threaded logging module for drone with both Serial and SD capability.
     * 
     * The logger is the primary method of data movement and management within the off-board software. 
     * Providing a common interface for messaging and handling, the logger handles SD File Management and data recording.
     * 
     * Messages sent to the logger are sent to the serial monitor based on verbosity and are logged into the SD card in batches.
     * The batch frequency can be set with with task priority to adjust the immediacy and impact of logging functionality.
     * 
     * The logger also maintains a flexible method for recording data into the SD card. Data is recorded onto a map and pushed to the SD card when requested.
     * 
     */
    class Logger
    {
    private:
    public:

        /// @brief Message Queue, acts as a buffer to hold messages before writing to SD
        std::queue<std::string> _msg_Queue;
        /// @brief Logging Queue, acts as a buffer to hold log messages before writing to SD 
        std::queue<std::string> _log_Queue;
        
        /// @brief Deprecated
        SemaphoreHandle_t _sem_log;
        SemaphoreHandle_t _sem_msg;
        
        /// @brief Mutex for Log Queue, ensures that the data is not overwritten and that the SD is only used by one thread at any one time
        SemaphoreHandle_t _sem_logQueueMutex;
        /// @brief Mutex for Message Queue, ensures that the data is not overwritten and that the SD is only used by one thread at any one time
        SemaphoreHandle_t _sem_msgQueueMutex;

        /// @brief Mutex for the data dictionary
        SemaphoreHandle_t _sem_dataMutex;
        /// @brief Enqueue mutex to protect message queue
        SemaphoreHandle_t _sem_dataEnqMutex;

        /// @brief Enables/disables the task
        SemaphoreHandle_t _sem_enableMutex;
        /// @brief Tracks if the thread is running or stopped. Written to by the thread.
        bool _running = false; // tracks if the thread is running or stopped
        bool _sdEnabled = false;
        bool _sdDetected = false;

        /// @brief Data dictionary to store the current data that is actively being written to
        std::map<std::string, float> _activeData;     // Data that is actively written to
        /// @brief Data dictionary to store the data before being written to SD
        std::map<std::string, float> _enqueuedData;   // Used as a buffer before being written
        bool _hasEnqueuedData = false;

        File _logFile;
        File _dataFile;
        File _configFile;

        File getLogFile();
        File getDataFile();
        File getConfigFile();

        /// @brief Opens the log file in Append mode
        void openLogFile();     // Opens Log file on SD in Append
        /// @brief Opens the dat afile in Append mode
        void openDataFile();    // Opens Data file on SD in Append
        void closeLogFile();    // Closes Log File
        void closeDataFile();   // Closes Data File

        /// @brief Location of the logging file, includes extension. Use .c_str() to for SD library
        const std::string _logFileLocation = "/as7.log";
        /// @brief Location of the logging file, includes extension. Use .c_str() to for SD library
        const std::string _dataFileLocation = "/data.csv";

        /// @brief Reference to the serial momnitor
        Print* _printer;

        Print* getPrinter();
        /// @brief Level of messages to be printed to the Serial Monitor. All messages are always printed to the SD card
        int _verbosity = LOG_LEVEL_INFORM;

        std::string getTestMessage();
        std::queue<std::string> getMsgQueue();
        std::queue<std::string> getLogQueue();

        SemaphoreHandle_t getSemLog();
        SemaphoreHandle_t getSemMsg();
        SemaphoreHandle_t getSemLogQueueMutex();
        SemaphoreHandle_t getSemMsgQueueMutex();

        inline SemaphoreHandle_t getSemDataMutex() {return _sem_dataMutex; }
        inline SemaphoreHandle_t getSemDataEnqMutex() {return _sem_dataEnqMutex; }

        SemaphoreHandle_t getSemEnableMutex();


        /**
         * @brief Adds a message to be recorded to the SD card
         * 
         * @param message String to be logged
         */
        void enqueueMsg(std::string message); 
        
        /**
         * @brief Adds a logging message to be logged to the SD card and sent to the serial monitor
         * 
         * The message may not be printed to the serial monitor if the verbosity level of the monitor is greater than the provided verbosity level.
         * 
         * This action enqueues the message to be written. Messages are buffered into the queue and then dequeued when the logger awakes every cycle.
         * The logging rate is determined by `LOGGER_FREQ`
         * 
         * @param message The message to be sent
         * @param verbosity The verbosity level. If unsure, use Logger.warn(), Logger.inform()...
         */
        void enqueueLog(std::string message, int verbosity);

        /// @brief For logging points to the PLY file. Deprecated
        /// @param points The point to log in string format
        void enqueuePnt(std::string points);

        /// @brief Dequeues a logging message from the log buffer
        /// @return The next logging message as a `std::string`
        std::string dequeueLog();

        /// @brief Implementation to start FreeRTOS tasks in classes
        static void startTaskImpl(void*);
        TaskHandle_t th_logger;
        void mainTask(void* parameters);

        /// @brief Initialises the SD card and clears previous files
        void initialiseSD();

    
        Logger(Print* output);
        /**
         * @brief Starts logger and intialises SD card.
         * 
         * @param core The core to run the logger on. Valid values are 0 and 1.
         * @param priority The priority of the logger. The higher, the more likely it is to be scheduled in front of other tasks. Default 1.
         * @param verbosity The verbosity level of the logger. If messages are less than the vebosity level, they are not written to the Serial Monitor.
         */
        void start(int core=1, int priority=1, int verbosity=LOG_LEVEL_INFORM);
        /// @brief Pauses the Logging thread. Logs can be enqueued but they are not acted on
        void pause();
        /// @brief Resumes the logging thread
        void resume();

        /// @brief  Returns the current health of the logger
        /// @return True if logger is running with no issues
        bool running();
        /// @brief Returns the current vebosity level of the logger
        /// @return Verbosity Level (Int)
        int  verbosity();

        /// @brief Sets the verbosity level of the logger
        /// @param verbosity Int, use definitions `LOG_LEVEL_INFORM`, LOG_LEVEL_WARN`... defined in sdlogger.h
        void setVerbosity(int verbosity);

        /// @brief Pushes a piece of data to be recorded as defined by the CSV mapping
        /// @param key The data column
        /// @param value The data value, as a float.
        void recordData(std::string key, float value);
        /// @brief Pushes data to be enqueued and written. Typically done by the most imporatnt recorded data, the US Sensors.
        void pushData();

        /**
         * @brief Logs an inform-level message
         * 
         * Messages of this level should be useful to know but are not critical or concerning to the operation of the drone
         * 
         * @param message The message to be enqueued
         */
        void inform(std::string message);
        /**
         * @brief Logs an warn-level message
         * 
         * Messages of this level should be concerning but otherwise do not have an immediate impact do the drone.
         * An example warning may be a thread being paused programmatically.
         * 
         * @param message The message to be enqueued
         */
        void warn(std::string message);
        /**
         * @brief Logs an error-level message
         * 
         * Messages of this level has a direct and immediate impact on the drone, however, may still allow for operation of unaffected modules.
         * An exmaple of an error would be a compass or accelerometer failure, though the drone will still collect data.
         * 
         * @param message The message to be enqueued
         */
        void error(std::string message);/**
         * @brief Logs an fatal-level message
         * 
         * Messages of this level indicate that the drone is in a fault and cannot be operated.
         * An example fatal error would be an emergency stop.
         * 
         * @param message The message to be enqueued
         */
        void fatal(std::string message);
        /**
         * @brief Logs an verbose-level message
         * 
         * Messages of this level may not be informative but indicate some sort of running or function of the drone.
         * An example verbose message may be the status logs showing the current state of the RX/TX Terminals
         * 
         * @param message The message to be enqueued
         */
        void verbose(std::string message);

        void plot(std::string message);

        /// @brief Disables SD Logging, even if SD Card is attached.
        void disableSDLogging();        

    };
}



#endif
