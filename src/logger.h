#include <iostream>
#include <fstream>
class Logger{
    private:
        static std::string logPath;
        static std::ofstream* logFile;

    public:
        static void setLogFile(std::string path);
        static void log(std::string msj);
        static int open();
        static void close();
        static bool isOpen();
};
