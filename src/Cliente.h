#ifndef CLIENTE_H
#define CLIENTE_H

#include <map>
#include <string>
#include <libgen.h>
#include "common.h"
#include "fifo.h"
#include "Semaforo.h"
#include "parsercomandos.h"
using namespace std;

class Cliente{
    private:
        Fifo *fifoEscritura;
        Fifo *fifoLectura;
				Semaforo* semEscritura;
				bool estaConectado;
		std::list<TPID> *listaHijos;
        
    public:
        Cliente();
        virtual ~Cliente();
        void conectarAlServidor();
        void desconectar();
        int compartirArchivo(std::string &pathArchivo);
        int dejarDeCompartirArchivo(std::string &pathArchivo);
        std::map<TPID,ListaPaths*>* getCompartidos();
        int iniciarDescarga(std::string pathArchivo);
				bool conectado();
        int escribirMensajeAlServidor(TCOM tipo,std::string mensaje);
				/* Pre: Los paths deben estar validados (solo existencia)
 				 * Post: Retorna 0 en caso de éxito. 
 				 *       Retorna -1 si el pid del cliente no existe.
 				 *       Retorna -2 si sharePath no está siendo compartido por el
 				 *       cliente pid.
 				 */
				int empezarTransferencia(string destPath, string sharePath, TPID pid);
        void recibirMensajeDelServidor(char* buffer,int bufsize);
};

#endif
