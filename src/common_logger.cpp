#include "logger.h"
#include <ctime>
using namespace std;

void Logger::log(std::string msj){
    if(isOpen()){
        time_t theTime;
        time(&theTime);
        (*logFile) << "[" << ctime(&theTime) << "] | " << msj << endl;
    }
    cout << msj << endl;
}

void Logger::setLogFile(std::string path){
    logPath = path;
    if(logFile == NULL) logFile = new ofstream();
    
}

int Logger::open(){
    if(logPath.length()>0){
        logFile->open(logPath.c_str());
        if(logFile->fail()){
            delete logFile;
            logFile = NULL;
            return -1;
        }
        return 0; 
    }
    else return -2;
    
}

void Logger::close(){
    if(isOpen()){ 
        logFile->close();
        delete logFile;
        logFile = NULL;
    }
}

bool Logger::isOpen(){
    return (logFile != NULL && logFile->is_open());
}

ofstream* Logger::logFile = NULL;
std::string Logger::logPath;
