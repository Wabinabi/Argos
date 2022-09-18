#include "SdLogger.h"

namespace AS7
{
    

    void Logger::mainTask(void * parameters) { 
        std::string msg;
        for (;;) {
            xSemaphoreTake(getSemEnableMutex(), portMAX_DELAY);
            openLogFile();

            // Check the log size
            xSemaphoreTake(getSemLogQueueMutex(), portMAX_DELAY);
            while (!getLogQueue().empty()) {
              msg = dequeueLog();
              

                getLogFile().println(msg.c_str());
                getPrinter()->println(msg.c_str());
                // write to SD card
            }
            xSemaphoreGive(getSemLogQueueMutex());
            
            xSemaphoreTake(getSemDataEnqMutex(), portMAX_DELAY);
            if (_hasEnqueuedData) {
                

            }
            xSemaphoreGive(getSemDataEnqMutex());
            xSemaphoreGive(getSemEnableMutex());
            closeLogFile();
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
        enqueueLog("[ "+std::to_string(millis())+" ms Inform] " + message, LOG_LEVEL_INFORM);
    }

    void Logger::warn(std::string message) {
        enqueueLog("[ "+std::to_string(millis())+" ms Warning] " + message, LOG_LEVEL_WARNING);
    }

    void Logger::error(std::string message) {
        enqueueLog("[ "+std::to_string(millis())+" ms Error] " + message, LOG_LEVEL_ERROR);
    }

    void Logger::fatal(std::string message) {
        enqueueLog("[ "+std::to_string(millis())+" ms Fatal] " + message, LOG_LEVEL_FATAL);
    }

    void Logger::verbose(std::string message) {
        enqueueLog("[ "+std::to_string(millis())+" ms Verbose] " + message, LOG_LEVEL_VERBOSE);
    }

    void Logger::plot(std::string message) {
        enqueueLog(message, LOG_LEVEL_VERBOSE);
    }

    void Logger::startTaskImpl(void* _this) {
    ((Logger*)_this)->mainTask(NULL);
    }

    void Logger::recordData(std::string key, int value) {
        xSemaphoreTake(_sem_dataMutex, portMAX_DELAY);
        _activeData[key] = value;
        xSemaphoreGive(_sem_dataMutex);
        
    }

    void Logger::pushData() {
        xSemaphoreTake(_sem_dataEnqMutex, portMAX_DELAY);
        _enqueuedData = _activeData;
        _hasEnqueuedData = true;
        xSemaphoreGive(_sem_dataEnqMutex);
    }

    void Logger::initialiseSD() {
        if (!SD_DISABLED) {
            if (!SD.begin(CS_PIN)) {
            fatal("SD initialisation failed, is the SD module loose or not connected?");
            verbose("SD.Begin(CS_PIN) failed to return a true value.");
            } else {
                _sdDetected = true;
                inform("SD card detected. SD Logging enabled.");
                inform("(C) 2022 Ecobat Project");

                // Clear previous logs
                File logFile = SD.open(_logFileLocation.c_str(), FILE_WRITE);

                if (logFile) {
                    logFile.println("AS7 Log file - (C) Ecobat Project 2022");
                    logFile.close();
                } else {
                    fatal("AS7 Log file could not be written to!");
                }

                // Clear and set CSV
                logFile = SD.open(_dataFileLocation.c_str(), FILE_WRITE);

                if (logFile) {
                    logFile.println("Test Version, DronePos_X, DronePos_Y, DronePos_Z, DroneHeading, US_Yp, US_Xp, US_Yn, US_Xn, US_Up, US_Down");
                    logFile.close();
                } else {
                    fatal("Data CSV could not be written to!");
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

    File Logger::getLogFile() {
        return _logFile;
    }
    File Logger::getConfigFile() {
        return _configFile;
    }
    File Logger::getDataFile() {
        return _dataFile;
    }

    void Logger::openLogFile() {
        _logFile = SD.open(_logFileLocation.c_str(), FILE_APPEND);
    }

    void Logger::closeLogFile() {
        _logFile.close();
    }

    void Logger::openDataFile() {
        _dataFile = SD.open(_dataFileLocation.c_str(), FILE_APPEND);
    }

    void Logger::closeDataFile() {
        _dataFile.close();
    }

    void Logger::disableSDLogging() {
        _sdEnabled = true;
        warn("SD logging has been disabled! Results will not be recorded to SD Card");
    }
    
    Logger::Logger(Print* output) {
        _printer = output;

        _sem_logQueueMutex = xSemaphoreCreateBinary();
        _sem_enableMutex = xSemaphoreCreateBinary();
        _sem_dataMutex = xSemaphoreCreateBinary();
        _sem_dataEnqMutex = xSemaphoreCreateBinary();
        
        xSemaphoreGive(_sem_logQueueMutex);
        xSemaphoreGive(_sem_dataMutex);
        xSemaphoreGive(_sem_dataEnqMutex);

        initialiseSD();
        
    }
}
