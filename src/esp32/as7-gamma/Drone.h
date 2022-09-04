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


namespace AS7 
{
    class Drone
    {
    private:

    TaskHandle_t th_logger;
    static void startTaskImpl(void*);
    void mainTask(void* parameters);


    public:

    Drone();
    bool OperatorAcknowledge(int channel=1);
    void eStop();

    

    


    };
}


#endif
