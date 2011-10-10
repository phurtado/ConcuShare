#ifndef SERV_H
#define SERV_H
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

#include "common.h"
#include "Semaforo.h"
#include "parsercomandos.h"
#include "fifo.h"

using namespace std;

class Servidor {
	private:
		Fifo *fifoLectura;
		Semaforo* semLectura;
		std::map<TPID, Fifo*> *fifosEscritura;
		std::map<TPID, ListaPaths*> *mapaPaths;
		std::list<TPID> *listaHijos;
		bool sigueEscuchando;
		
		char *buffer;
		
		size_t getLongitudPath(char *buffer);
		std::string getPathArchivo(char *buffer, size_t longitud);
		int leerComando(ParserComandos &parser);
		int archivoCompartidoActualmente(std::string &pathArchivo);
		
	public:
		/** 
         * Inicializa el servidor
         */
		Servidor();

		/**
         * Bloquea al servidor a la espera de comandos de clientes 
         * que lleguen a través de la fifo de lectura.
         */
		void escucharComandos();

		/**
         * Da de alta a un cliente
		 * @param pidCliente pid del cliente a dar de alta
         * @return 0 en caso exitoso, -1 si hubo error.
		 */
		int altaCliente(TPID pidCliente);

		/**
         * metodo para dar de baja a un cliente
		 * pidCliente: pid del cliente a dar de baja
         * @return 0 en caso exitoso, -1 en caso fallido.
		 */
		int bajaCliente(TPID pidCliente);

		/**
         * Obtiene y escribe la lista de archivos compartidos 
         * en la fifo de lectura de un cliente.
		 * @param pidCliente Cliente solicitante de la lista
         * @return 0 en caso exitoso, -1 en caso fallido. 
		 */
		int enviarListaCompartidosACliente(TPID pidCliente);

		/**
         * Agrega un archivo a los compartidos.
		 * @param pathArchivo String conteniendo el path del archivo a compartir
		 * @param pidCliente PID del cliente que quiere compartir el archivo
		 * @return 0 en caso de éxito, -1 si no encuentra el archivo entre los compartidos.
         */
		int compartirArchivo(std::string &pathArchivo, TPID pidCliente);

		/**
         * Quita un archivo de la lista de compartidos.
		 * @param pathArchivo string conteniendo el path del archivo a descompartir
		 * @param PID del cliente que quiere descompartir el archivo
		 * @return 0 en caso exitoso, -1 si falló el fork. 
         */
		int descompartirArchivo(std::string &pathArchivo, TPID pidCliente);

		/**
         * Solicita la transferencia de un archivo, 
		 * el servidor notificará al cliente transmisor y este 
		 * se encargará de la transferencia.
		 * @param pathArchivo Path del archivo requerido
		 * @param pathDestino Path del archivo destino de la transferencia
		 * @param pidClienteDestino PID cliente que transmitira el archivo
         * @return 0 en caso exitoso, -1 en caso fallido.
		 */
		int transferirArchivo(std::string &pathArchivo, std::string &pathDestino, 
				TPID pidClienteDestino, TPID pidClienteDuenioArchivo);
		
		/**
         * Destructor del Servidor 
         */
		virtual ~Servidor();
		
		/**
         * Setea el indicador de continuación de escucha de comandos
		 * del servidor
		 * @param continua Boolean indicando si continua (true) o no (false)
		 */  
        void setContinua(bool continua);

};

#endif /* SERV_H */
