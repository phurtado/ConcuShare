#ifndef SERV_H
#define SERV_H
#include <iostream>
#include <map>
#include "common.h"

#include "parsercomandos.h"

#include "fifo.h"



#include "fifo.h"

class Servidor {
	private:
		Fifo *fifoLectura;
		std::map<TPID, Fifo*> *fifosEscritura;
		std::map<TPID, ListaPaths*> *mapaPaths;
		bool sigueEscuchando;
		
		char buffer[BUFSIZE];
		
		size_t getLongitudPath(char *buffer);
		std::string getPathArchivo(char *buffer, size_t longitud);

		
	public:
		/* Inicializa el servidor */
		Servidor();

		/* Mediante este método el servidor se bloquea a la espera
		 * de comandos de clientes que lleguen a traves de la fifo
		 * de lectura */
		void escucharComandos();

		/* metodo para dar de alta a un cliente
		 * pidCliente: pid del cliente a dar de alta
		 */
		int altaCliente(TPID pidCliente);

		/* metodo para dar de baja a un cliente
		 * pidCliente: pid del cliente a dar de baja
		 */
		int bajaCliente(TPID pidCliente);

		/* obtiene y escribe en la fifo del cliente cuyo PID es 
		 * pidCliente la lista de archivos compartidos
		 * pidCliente: cliente solicitante de la lista
		 */
		int enviarListaCompartidosACliente(TPID pidCliente);

		/* metodo para compartir un archivo
		 * pathArchivo: string conteniendo el path del archivo a compartir
		 * pidCliente: PID del cliente que quiere compartir el archivo
		 */
		int compartirArchivo(std::string &pathArchivo, TPID pidCliente);

		/* metodo para descompartir un archivo (borrarlo de la lista de
		 * compartidos)
		 * pathArchivo: string conteniendo el path del archivo a descompartir
		 * pidCliente: PID del cliente que quiere descompartir el archivo
		 */
		int descompartirArchivo(std::string &pathArchivo, TPID pidCliente);

		/* metodo para solicitar transferencia de un archivo, 
		 * el serviror notidicara al cliente transmisor y este 
		 * se encargara de la transferencia
		 * pathArchivo: path del archivo requerido
		 * pidClienteOrigen: PID del cliente que solicita el archivo
		 * pidClienteDestino: PID cliente que transmitira el archivo
		 */
		int transferirArchivo(std::string &pathArchivo, TPID pidClienteOrigen,
		 TPID pidClienteDestino);
		
		/* destructor del Servidor */
		virtual ~Servidor();

        size_t obtenerTamanioLista();

};

#endif /* SERV_H */
