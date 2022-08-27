/**
 * TODO: THIS BANNER
 * 
 * This is AS7's Logger. It prints to serial and SD card. 
 * The goal of this logger is to tie logging and comms into one location and delegate to a thread
 */
#pragma once
#ifndef AS7DRONE_H
#define AS7DRONE_H

#include <Arduino.h>

namespace AS7 
{
    class Drone 
    {
    private:

    public:
        Drone();
        void run();
    };
}





#endif