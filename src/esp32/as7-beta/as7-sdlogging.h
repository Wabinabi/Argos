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

const int LOG_LEVEL_FATAL = 0;
const int LOG_LEVEL_ERROR = 1;
const int LOG_LEVEL_WARNING = 2;
const int LOG_LEVEL_INFO = 3;
const int LOG_LEVEL_NOTICE = 4;

// Used for reading the message stacks for the scribe task
SemaphoreHandle_t sem_log;
SemaphoreHandle_t sem_msg;

// Mutex for the message stacks
SemaphoreHandle_t sem_logStackMutex;
SemaphoreHandle_t sem_msgStackMutex;

TaskHandle_t th_logger;


namespace AS7 
{
    class Logger
    {
    private:

    public:
        Logger();
        void run();
    };
}



#endif