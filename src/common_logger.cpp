#include "logger.h"

using namespace std;


Logger *Logger::inst = NULL;
string Logger::cadena = "";

Logger& el (Logger& l) {
	l.log();
	return l;
}

void Logger::log() {
			Logger::log(Logger::cadena);
			Logger::cadena.clear();
}

char *sacarNuevaLinea(char *cad) {
	cad[strlen(cad) - 1] = 0;
	return cad;
}

void Logger::log(std::string msj){
    if(isOpen()){
        time_t theTime;
        time(&theTime);
        (*logFile) << "[" << sacarNuevaLinea(ctime(&theTime)) << "] | " << msj << endl;
    }
}

Logger Logger::instancia() {
	if(inst == NULL)
		inst = new Logger();
	return *inst;
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

Logger& Logger::operator << (const string &cad) {
	Logger::log(cad);
	return *this;
}

Logger& Logger::operator << (const int num) {
	stringstream ss;
	ss << num;
	Logger::cadena += ss.str();
	return *this;
}

Logger& Logger::operator << (const char * cad) {
	Logger::cadena += cad;
	return *this;
}

Logger& Logger::operator << (const char car) {
	
	Logger::cadena += car;
	return *this;
}

Logger& Logger::operator << (const off_t num) {
	stringstream ss;
	ss << num;
	Logger::cadena += ss.str();
	return *this;
}

Logger& Logger::operator<< (Logger& ( *pf )(Logger&)) {
	return pf(*this);
}

void Logger::liberar() {
	Logger::close();
	if(Logger::inst)
		delete Logger::inst;
	Logger::inst = NULL;
}
