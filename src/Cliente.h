#ifndef CLIENTE_H
#define CLIENTE_H

#include <map>
#include "common.h"
#include "fifo.h"

class Cliente{
    private:
        Fifo *fifoEscritura;
        Fifo *fifoLectura;
        
    public:
        Cliente();
        virtual ~Cliente();
        void conectarAlServidor();
        void desconectar();
        int compartirArchivo(std::string pathArchivo);
        int dejarDeCompartirArchivo(std::string pathArchivo);
        std::map<TPID,ListaPaths*>* getCompartidos();
        int iniciarDescarga(std::string pathArchivo);


        int escribirMensajeAlServidor(TCOM tipo,std::string mensaje);
};

#endif
