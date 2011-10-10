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
        Fifo *fifoEscritura; ///< Fifo donde escribe el cliente al servidor
        Fifo *fifoLectura; ///< Fifo de donde escribe el servidor al cliente
        Semaforo* semEscritura;
        bool estaConectado;
		std::list<TPID> *listaHijos; ///< Lista de procesos hijos del proceso cliente
        
    public:
        /**
         * Inicializa un cliente desconectado, con una fifo de escritura abierta
         * y sin asociar a fifo de lectura. Crea el semáforo de escritura y la 
         * lista vacía de procesos hijos.
         */
        Cliente();
        
        /**
         * Libera los recursos de la instancia. Desconecta si está conectado, 
         * cierra ls fifos y borra la lista de hijos.
         */
        virtual ~Cliente();
        
        /**
         * "Conecta" al cliente al servidor escribiendole en una fifo un mensaje
         * solicitando el alta.
         */
        void conectarAlServidor();
        
        /**
         * Solicita la baja al servidor y cierra la fifo de lectura.
         */
        void desconectar();
        
        /**
         * Agrega un archivo a la lista de archivos compartidos del servidor.
         * @param pathArchivo La ruta relativa o absoluta del archivo a compartir.
         * @return 0 en caso de éxito, -1 si hubo error.
         */
        int compartirArchivo(std::string &pathArchivo);
        
        /**
         * Elimina un archivo de la lista de archivos compartidos del servidor.
         * @param pathArchivo La ruta relativa o absoluta del archivo a dejar de compartir.
         * @return 0 en caso de éxito, -1 si hubo error.
         */
        int dejarDeCompartirArchivo(std::string &pathArchivo);
        
        /**
         * Obtiene la lista de archivos compartidos del servidor.
         * @return El mapa de pares <Pid,Paths> que representa los archivos compartidos por cada cliente.
         */
        std::map<TPID,ListaPaths*>* getCompartidos();
       
        /**
         * Consulta el estado de conexión del cliente al servidor.
         * @return true si el cliente está conectado, false si no
         */
        bool conectado();
        
        /**
         * Envía un mensaje al servidor.
         * @pre{El cliente debe estar conectado al servidor.}
         * @param tipo El tipo de mensaje (miembro del Enum TCOM)
         * @param mensaje El mensaje a enviar.
         */
        int escribirMensajeAlServidor(TCOM tipo,std::string mensaje);
		
        /** 
         * Inicia el proceso para la recepción de un archivo.
         * @param destPath Directorio de destino (sin nombre de archivo)
         * @param sharePath Ruta del archivo compartido que se quiere transferir (exactamente como aparece en la lista de compartidos)
         * @pid El pid del cliente que comparte el archivo solicitado
         * @pre{Los paths deben estar validados (solo existencia)}
         * @return 0 en caso de éxito;-1 si el pid del cliente no existe; -2 si sharePath no está siendo compartido por el cliente pid.
         */
        int empezarTransferencia(string destPath, string sharePath, TPID pid);
        
        /**
         * Lee el fifo de lectura para obtener un mensaje enviado por el servidor
         * @param buffer Array inicializado donde se escribirá el resultado.
         * @param bufsize Tamaño del buffer.
         */
        void recibirMensajeDelServidor(char* buffer,int bufsize);

        /**
         * Obtiene la lista de hijos del proceso actual (transferencias activas).
         * @return Lista de PIDs de los procesos hijos.
         */
        std::list<TPID> *getListaHijos();
};

#endif
