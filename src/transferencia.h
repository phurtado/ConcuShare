#ifndef TRANSF_H_
#define TRANSF_H_

#include "MemoriaCompartida.h"
#include "buffermem.h"
#include "Semaforo.h"
#include <string>
#include <fstream>

/* Letra a usar en la memoria compartida */
#define LETRA 'c'

/* numero de semaforo que bloquea lectura y escritura respectivamente */
#define SEMLEER 0
#define SEMESCR 1


class Transferencia {
	private:
	MemoriaCompartida<BufferMem> *memoriaCompartida;
	std::string pathOrigen, pathDestino;
	Semaforo *semaforos;
	BufferMem buffer;
	std::ifstream *streamLectura;
	std::streampos longitudArchivo, bytesTransferidos;
	
	
	
	public:
	/* inicializa una transferencia.
	 * pathOrigen: path del archivo origen el cual se enviara
	 * pathDestino: path del archivo destino el cual se escribira
	 */
	Transferencia(std::string &pathOrigen, std::string &pathDestino);
	
	/* metodo para enviar datos a la memoria compartida.
	 * retorno: 
	 * 0 si salio todo OK y hay mas datos para enviar
	 * 1 si salio todo OK y no hay mas datos
	 */
	int enviar();
	
	/* metodo para recibir datos desde la memoria compartida y escribirlos
	 * en pathDestino.
	 * 0 si salio todo OK y hay mas datos a recibir
	 * 1 si salio todo OK y termino la transferencia
	 */
	int recibir();
	
	/* Destructor de la Transferencia */
	virtual ~Transferencia();
	

};

#endif /* TRANSF_H_ */
