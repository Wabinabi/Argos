#include "as7-sdlogging.h"

namespace AS7
{
    Logger::Logger(Print* output) {
        output->println("This is the logger!");
        printer = output;

    }

    void Logger::handleMessage(std::string message) {
        printer->println(message.c_str());
      
        //printer->println(message.c_str());
        // log onto SD card
    }

    void Logger::inform(std::string message) {
        handleMessage("[Inform] " + message);
    }

    void Logger::notice(std::string message) {

    }

    void Logger::warn(std::string message) {

    }

    void Logger::error(std::string message) {

    }

    void Logger::fatal(std::string message) {

    }

    void Logger::verbose(std::string message) {

    }

    void Logger::start(int core, int priority) {
        
    }

}
