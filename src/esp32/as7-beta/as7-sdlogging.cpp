#include "as7-sdlogging.h"

namespace AS7
{
    Logger::Logger(Print* output) {
        output->println("This is the logger!");
        _printer = output;

    }

    void Logger::mainTask(void * parameters) { 

        for (;;) {
            //Serial.println(this->getTestMessage().c_str());
            xSemaphoreTake(getSemLogStackMutex(), portMAX_DELAY);
            Serial.println("successfully taken mutex");
            delay(1000);
        }
    }


    void Logger::enqueueMessage(std::string message) {
        _printer->println(message.c_str());
      
        //printer->println(message.c_str());
        // log onto SD card
    }
    
     

    //std::string getTestMessage() {}
    std::stack<std::string> Logger::getMsgStack() { return _msg_stack; }
    std::stack<std::string> Logger::getLogStack() { return _log_stack; }

    SemaphoreHandle_t Logger::getSemLog() { return _sem_log; }
    SemaphoreHandle_t Logger::getSemMsg() { return _sem_msg; }
    SemaphoreHandle_t Logger::getSemLogStackMutex() { return _sem_logStackMutex; }
    SemaphoreHandle_t Logger::getSemMsgStackMutex() { return _sem_msgStackMutex; }

    void Logger::inform(std::string message) {
        enqueueMessage("[Inform] " + message);
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

    void Logger::startTaskImpl(void* _this) {
    ((Logger*)_this)->mainTask(NULL);
    }


    void Logger::start(int core, int priority) {

        _sem_logStackMutex = xSemaphoreCreateBinary();
        xSemaphoreGive(_sem_logStackMutex);

        xTaskCreatePinnedToCore(
        this->Logger::startTaskImpl,                /* Task function. */
        "US Task",              /* name of task. */
        8192,                   /* Stack size of task */
        this,                   /* parameter of the task */
        1, /* priority of the task */
        &th_logger,         /* Task handle to keep track of created task */
        1);                     /* pin task to core 1 */
    }

}
