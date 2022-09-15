/**
 * TODO: THIS BANNER
 * 
 * This is AS7's Logger. It prints to serial and SD card. 
 * The goal of this logger is to tie logging and comms into one location and delegate to a thread
 * Read this for getters and setters inside freertos https://forums.freertos.org/t/freertos-task-in-a-c-class/13984
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
#define LOG_LEVEL_SILENT  0
#define LOG_LEVEL_FATAL   1
#define LOG_LEVEL_ERROR   2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_INFORM    4
#define LOG_LEVEL_VERBOSE 5
#define CS_PIN 5

#define LOGGER_FREQ 50       // Update rate in Hertz

#define PLOTTER_ENABLE false // only prints plots, for testing.
#define SD_DISABLED false

namespace AS7 
{
    class Logger
    {
    private:

        std::queue<std::string> _msg_Queue;
        std::queue<std::string> _log_Queue;
        // Used for reading the message Queues for the scribe task
        SemaphoreHandle_t _sem_log;
        SemaphoreHandle_t _sem_msg;
        
        // Mutex for the message Queues
        SemaphoreHandle_t _sem_logQueueMutex;
        SemaphoreHandle_t _sem_msgQueueMutex;

        // Sem for Enabling/Disabling Task
        SemaphoreHandle_t _sem_enableMutex;
        bool _running = false; // tracks if the thread is running or stopped
        bool _sdEnabled = false;
        bool _sdDetected = false;

        std::map<std::string, int> _data;

        
        Print* _printer;

        Print* getPrinter();
        int _verbosity = LOG_LEVEL_INFORM;

        std::string getTestMessage();
        std::queue<std::string> getMsgQueue();
        std::queue<std::string> getLogQueue();

        SemaphoreHandle_t getSemLog();
        SemaphoreHandle_t getSemMsg();
        SemaphoreHandle_t getSemLogQueueMutex();
        SemaphoreHandle_t getSemMsgQueueMutex();

        SemaphoreHandle_t getSemEnableMutex();


        // Adds a message to be recorded to the SD card
        // diagnotics probably? will flush out later
        void enqueueMsg(std::string message); 

        // Adds a message to be sent to the console and onto the SD card
        void enqueueLog(std::string message, int verbosity);

        // for PLY generation, will need to be flushed out.
        // String since we can also send header information
        void enqueuePnt(std::string points);

        std::string dequeueLog();

        // Implementation to start FreeRTOS tasks in classes
        static void startTaskImpl(void*);
        TaskHandle_t th_logger;
        void mainTask(void* parameters);

        void initialiseSD();

    public:
        Logger(Print* output);
        void start(int core=1, int priority=1, int verbosity=LOG_LEVEL_INFORM);
        void pause();
        void resume();

        bool running();
        int  verbosity();

        void setVerbosity(int verbosity);

        void recordData(std::string key, int value);
        void pushData();



        // The main logging tasks 
        void inform(std::string message);
        void warn(std::string message);
        void error(std::string message);
        void fatal(std::string message);
        void verbose(std::string message);

        void record(int x, int y, int z);
        void plot(std::string message);

        void disableSDLogging();        // Disables SD Logging, even if SD Card is attached.

    };
}



#endif
