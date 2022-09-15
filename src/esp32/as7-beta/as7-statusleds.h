/**
 * TODO: THIS BANNER
 * 
 * This is AS7's Logger. It prints to serial and SD card. 
 * The goal of this logger is to tie logging and comms into one location and delegate to a thread
 * Read this for getters and setters inside freertos https://forums.freertos.org/t/freertos-task-in-a-c-class/13984
 */
#pragma once
#ifndef AS7STATUSLED_H
#define AS7STATUSLED_H

#include <Arduino.h>


namespace AS7 
{
    class Logger
    {
    private:

    // Drone state for Finite State Machine

    /* Initialise - Drone starting up and checks all devices
    * Ready      - Drone is ready to arm and start mission, requires operator input
    * Armed      - Drone starts arming process and spins propellers
    * Flying     - Drone is in the air and flying. Drone uses Flight Modes for operation
    * Landing    - Drone will attempt to land by lowering throttle
    * Stopped    - Drone will cease all movement and read next mission
    * Faulted    - Similar to stopped, but does not reset even after powering on/off
    * Debug      - For unit testing
    */
    const int StraightLineDist = 100; // Aim to travel 100cm in AutoStraightLine mode

    // Enums for drone FSM
    enum DroneState {Initialise, Ready, Armed, Flying, Landing, Stopped, Faulted, Debug};
    enum DroneFlightMode {OperatorControl, AutoStraightLine, ArmOnly};
    enum DroneDebugTest {SBUS_COMMS, ARMING_DISARMING, SD_READ_WRITE, LED_RESPONSE}; // Unit testing scenarios

    DroneState currentState = Initialise;
DroneFlightMode currentFlightMode = ArmOnly;


    public:

    };
}



#endif