#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
class Logger{
    private:
        static std::string logPath; ///> Ruta del archivo de log a escribir.
        static std::ofstream* logFile;

    public:
        /**
         * Setea el archivo donde se escribirá el log.
         * @param path Ruta del archivo de log.
         */
        static void setLogFile(std::string path);

        /**
         * Escribe un mensaje al archivo de log.
         * Si no hay archivo de log abierto, el método no hace nada.
         * @param msj El mensaje a escribir.
         */
        static void log(std::string msj);

        /**
         * Abre el archivo de log. Si no existe, lo crea.
         * @return 0 en caso exitoso. -1 si no pudo abrirse el archivo. -2 si no está seteado el archivo de log.
         */
        static int open();
        
        /**
         * Cierra el archivo del log.
         */
        static void close();

        /**
         * Consulta si el archivo de log está abierto.
         * @return true si está abierto. false si no.
         */
        static bool isOpen();
};

#endif
