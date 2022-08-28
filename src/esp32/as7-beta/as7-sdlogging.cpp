#include "as7-sdlogging.h"

namespace AS7
{



    Logger::Logger(Print* output) {
        output->println("This is the logger!");

    }

    void Logger::start(int core, int priority) {
        
    }

}
