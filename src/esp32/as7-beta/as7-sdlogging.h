/**
 * TODO: THIS BANNER
 * 
 * This is AS7's Logger. It prints to serial and SD card. 
 * The goal of this logger is to tie logging and comms into one location and delegate to a thread
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
        std::stack<std::string> msg_stack;
        std::stack<std::string> log_stack;
        // Used for reading the message stacks for the scribe task
        SemaphoreHandle_t sem_log;
        SemaphoreHandle_t sem_msg;
        
        // Mutex for the message stacks
        SemaphoreHandle_t sem_logStackMutex;
        SemaphoreHandle_t sem_msgStackMutex;
        
        TaskHandle_t th_logger;

        Print* printer;

        void handleMessage(std::string message);

        


    public:
        Logger(Print* output);
        void start(int core, int priority);

        

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
