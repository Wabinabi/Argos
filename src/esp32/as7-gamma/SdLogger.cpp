#include "SdLogger.h"

namespace AS7
{
    

    void Logger::mainTask(void * parameters) { 
        std::string msg;
        for (;;) {
            xSemaphoreTake(getSemEnableMutex(), portMAX_DELAY);

            // Check the log size
            xSemaphoreTake(getSemLogQueueMutex(), portMAX_DELAY);
            while (!getLogQueue().empty()) {
              msg = dequeueLog();
              
                getPrinter()->println(msg.c_str());
                // write to SD card
            }
            xSemaphoreGive(getSemLogQueueMutex());
            xSemaphoreGive(getSemEnableMutex());
            vTaskDelay((1000/LOGGER_FREQ) / portTICK_PERIOD_MS); // Allow other tasks to take control
        }
    }

    std::string Logger::dequeueLog() {
      std::string top = _log_Queue.front();
      _log_Queue.pop();
      return top;
    }


    void Logger::enqueueMsg(std::string message) {
        //_printer->println(message.c_str());

        //printer->println(message.c_str());
        // log onto SD card

        xSemaphoreGive(_sem_msgQueueMutex);
    }

    void Logger::enqueueLog(std::string message, int verbosity) {
        xSemaphoreTake(_sem_logQueueMutex, portMAX_DELAY);
        _log_Queue.push(message);
        xSemaphoreGive(_sem_logQueueMutex);
    }

    
    Print* Logger::getPrinter() {return _printer;}

    std::queue<std::string> Logger::getMsgQueue() { return _msg_Queue; }
    std::queue<std::string> Logger::getLogQueue() { return _log_Queue; }

    SemaphoreHandle_t Logger::getSemLog() { return _sem_log; }
    SemaphoreHandle_t Logger::getSemMsg() { return _sem_msg; }
    SemaphoreHandle_t Logger::getSemLogQueueMutex() { return _sem_logQueueMutex; }
    SemaphoreHandle_t Logger::getSemMsgQueueMutex() { return _sem_msgQueueMutex; }

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

    void Logger::plot(std::string message) {
        enqueueLog(message, LOG_LEVEL_VERBOSE);
    }

    void Logger::startTaskImpl(void* _this) {
    ((Logger*)_this)->mainTask(NULL);
    }

    void Logger::initialiseSD() {
        if (!SD_DISABLED) {
            if (!SD.begin(CS_PIN)) {
            fatal("SD initialisation failed, is the SD module loose or not connected?");
            verbose("SD.Begin(CS_PIN) failed to return a true value.");
            } else {
                _sdDetected = true;
                inform("SD card detected. SD Logging enabled.");

                File testFile = SD.open("/SDTest.txt", FILE_WRITE);
                if (testFile) {
                    testFile.println("Hello ESP32 SD");
                    testFile.close();
                    //Serial.println("Success, data written to SDTest.txt");
                } else {
                    //Serial.println("Error, couldn't not open SDTest.txt");
                }

            }

            
        } else {
            warn("SD Logging has been disabled globally! Data will not be recorded to SD");
        }
        
        
    }


    // We're going with the assumption that there's only one logger
    //  For now we won't be implementing protection here since it could cause debugging issues
    void Logger::pause() {
        if (!_running) {
            warn("Request to pause logger denied: Logger already paused.");
        } else {
            _running = false;
            xSemaphoreTake(getSemEnableMutex(), portMAX_DELAY);
        }
    }

    void Logger::resume() {
        if (!_running) {
            _running = true;
            xSemaphoreGive(_sem_enableMutex);
        } else {
            warn("Request to resume logger denied: Logger already running");
        }
        
    }

    void Logger::start(int core, int priority, int verbosity) {
        _verbosity = verbosity;
        
        xTaskCreatePinnedToCore(
        this->Logger::startTaskImpl,                /* Task function. */
        "Logger",              /* name of task. */
        8192,                   /* Queue size of task */
        this,                   /* parameter of the task */
        priority, /* priority of the task */
        &th_logger,         /* Task handle to keep track of created task */
        core);                     /* pin task to core 1 */

        _running = true;
        xSemaphoreGive(_sem_enableMutex);
    }

    void Logger::disableSDLogging() {
        _sdEnabled = true;
        warn("SD logging has been disabled! Results will not be recorded to SD Card");
        
    }
    
    Logger::Logger(Print* output) {
        _printer = output;

        _sem_logQueueMutex = xSemaphoreCreateBinary();
        _sem_enableMutex = xSemaphoreCreateBinary();
        xSemaphoreGive(_sem_logQueueMutex);

        //initialiseSD();
        
    }
}
