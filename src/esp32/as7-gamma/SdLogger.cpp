#include "SdLogger.h"

namespace AS7
{
    Logger::Logger(Print* output) {
        _printer = output;

        _sem_logStackMutex = xSemaphoreCreateBinary();
        _sem_enableMutex = xSemaphoreCreateBinary();

        
        xSemaphoreGive(_sem_logStackMutex);

        
    }

    void Logger::mainTask(void * parameters) { 
        std::string msg;
        for (;;) {
            xSemaphoreTake(getSemEnableMutex(), portMAX_DELAY);

            // Check the log size
            xSemaphoreTake(getSemLogStackMutex(), portMAX_DELAY);
            if (!getLogStack().empty()) {
              msg = popLogStack();
              
                getPrinter()->println(msg.c_str());
                // write to SD card
            }
            xSemaphoreGive(getSemLogStackMutex());
            
            xSemaphoreGive(getSemEnableMutex());
        }
    }

    std::string Logger::popLogStack() {
      std::string top = _log_stack.top();
      _log_stack.pop();
      return top;
    }


    void Logger::enqueueMsg(std::string message) {
        //_printer->println(message.c_str());

        //printer->println(message.c_str());
        // log onto SD card

        xSemaphoreGive(_sem_msgStackMutex);
    }

    void Logger::enqueueLog(std::string message, int verbosity) {
        xSemaphoreTake(_sem_logStackMutex, portMAX_DELAY);
        _log_stack.push(message);
        xSemaphoreGive(_sem_logStackMutex);
    }

    
    Print* Logger::getPrinter() {return _printer;}

    std::stack<std::string> Logger::getMsgStack() { return _msg_stack; }
    std::stack<std::string> Logger::getLogStack() { return _log_stack; }

    SemaphoreHandle_t Logger::getSemLog() { return _sem_log; }
    SemaphoreHandle_t Logger::getSemMsg() { return _sem_msg; }
    SemaphoreHandle_t Logger::getSemLogStackMutex() { return _sem_logStackMutex; }
    SemaphoreHandle_t Logger::getSemMsgStackMutex() { return _sem_msgStackMutex; }

    SemaphoreHandle_t Logger::getSemEnableMutex() { return _sem_enableMutex; }

    int Logger::verbosity() {return _verbosity; }
    void Logger::setVerbosity(int verbosity) {_verbosity = verbosity; };

    bool Logger::running() {return _running; }

    void Logger::inform(std::string message) {
        enqueueLog("[Inform] " + message, LOG_LEVEL_INFORM);
    }

    void Logger::warn(std::string message) {
        enqueueLog("[Warning] " + message, LOG_LEVEL_WARNING);
    }

    void Logger::error(std::string message) {
        enqueueLog("[Error] " + message, LOG_LEVEL_ERROR);
    }

    void Logger::fatal(std::string message) {
        enqueueLog("[Fatal] " + message, LOG_LEVEL_FATAL);
    }

    void Logger::verbose(std::string message) {
        enqueueLog("[Verbose] " + message, LOG_LEVEL_VERBOSE);
    }

    void Logger::startTaskImpl(void* _this) {
    ((Logger*)_this)->mainTask(NULL);
    }

    void Logger::initialiseSD() {
        if (!SD.begin(CS_PIN)) {
            //Serial.println("Error, SD Initialization Failed");
            fatal("SD initialisation failed, is the SD module loose or not connected?");
            verbose("SD.Begin(CS_PIN) failed to return a true value.");
        }

        File testFile = SD.open("/SDTest.txt", FILE_WRITE);
        if (testFile) {
            testFile.println("Hello ESP32 SD");
            testFile.close();
            //Serial.println("Success, data written to SDTest.txt");
        } else {
            //Serial.println("Error, couldn't not open SDTest.txt");
        }
    }


    // We're going with the assumption that there's only one logger
    //  For now we won't be implementing protection here since it could cause debugging issues
    void Logger::pause() {
        if (!_running) {
            // log a warning and do nothing
        } else {
            _running = false;
            xSemaphoreTake(getSemLogStackMutex(), portMAX_DELAY);
        }
    }

    void Logger::resume() {
        if (!_running) {
            _running = true;
            xSemaphoreGive(_sem_enableMutex);
        } else {
            // log a warning and do nothing
        }
        
    }

    void Logger::start(int core, int priority, int verbosity) {
        _verbosity = verbosity;
        
        xTaskCreatePinnedToCore(
        this->Logger::startTaskImpl,                /* Task function. */
        "US Task",              /* name of task. */
        8192,                   /* Stack size of task */
        this,                   /* parameter of the task */
        1, /* priority of the task */
        &th_logger,         /* Task handle to keep track of created task */
        1);                     /* pin task to core 1 */

        xSemaphoreGive(_sem_enableMutex);
    }

    

}
