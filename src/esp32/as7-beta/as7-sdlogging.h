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
#include <stack>      // For logging

const int LOG_LEVEL_FATAL = 0;
const int LOG_LEVEL_ERROR = 1;
const int LOG_LEVEL_WARNING = 2;
const int LOG_LEVEL_INFO = 3;
const int LOG_LEVEL_NOTICE = 4;



namespace AS7 
{
    class Logger
    {
    private:
        std::stack<std::string> _msg_stack;
        std::stack<std::string> _log_stack;
        // Used for reading the message stacks for the scribe task
        SemaphoreHandle_t _sem_log;
        SemaphoreHandle_t _sem_msg;
        
        // Mutex for the message stacks
        SemaphoreHandle_t _sem_logStackMutex;
        SemaphoreHandle_t _sem_msgStackMutex;

        // Sem for Enabling/Disabling Task
        SemaphoreHandle_t _sem_enableMutex;
        bool _running = false; // tracks if the thread is running or stopped

        TaskHandle_t th_logger;
        Print* _printer;

        Print* getPrinter();


        // figure out how to init stacks -> https://iq.opengenus.org/stack-initialization-cpp-stl/
        std::string getTestMessage();
        std::stack<std::string> getMsgStack();
        std::stack<std::string> getLogStack();

        SemaphoreHandle_t getSemLog();
        SemaphoreHandle_t getSemMsg();
        SemaphoreHandle_t getSemLogStackMutex();
        SemaphoreHandle_t getSemMsgStackMutex();

        SemaphoreHandle_t getSemEnableMutex();

        void mainTask(void* parameters);

        // Adds a message to be recorded to the SD card
        // diagnotics probably? will flush out later
        void enqueueMsg(std::string message); 

        // Adds a message to be sent to the console and onto the SD card
        void enqueueLog(std::string message);

        // for PLY generation, will need to be flushed out.
        // String since we can also send header information
        void enqueuePnt(std::string points);

        std::string popLogStack();

        // Implementation to start FreeRTOS tasks in classes
        static void startTaskImpl(void*);

    public:
        Logger(Print* output);
        void start(int core=1, int priority=1);
        void pause();
        void resume();


        // The main logging tasks 
        void inform(std::string message);
        void notice(std::string message);
        void warn(std::string message);
        void error(std::string message);
        void fatal(std::string message);
        void verbose(std::string message);

        void record(int x, int y, int z);


    };
}



#endif
