#ifndef TRANSF_H_
#define TRANSF_H_


#include <string>
#include "MemoriaCompartida.h"
#include "buffermem.h"
#include "Semaforo.h"
#include "LockFile.h"
#include "buffermem.h"

/* Letra a usar en la memoria compartida */
#define LETRA 'c'

/* numero de semaforo que bloquea lectura y escritura respectivamente */
#define SEMLEER 0
#define SEMESCR 1

typedef enum {ENVIAR, RECIBIR} TRMODO;


class Transferencia {
	private:
	MemoriaCompartida<BufferMem> *memoriaCompartida;
	std::string pathOrigen, pathDestino;
	Semaforo *semaforos;
	BufferMem buffer;
	LockFile *archivo;
	off_t longitudArchivo, bytesTransferidos;
	
	TRMODO modo;
	
	public:
	/**
     * Inicializa una transferencia.
	 * @param pathOrigen Path del archivo origen el cual se enviara
	 * @param pathDestino Path del archivo destino el cual se escribira
	 */
	Transferencia(std::string &pathOrigen, std::string &pathDestino, TRMODO modo);
	
	/** 
     * Envía datos a la memoria compartida.
	 * @return 0 si salio todo OK y hay mas datos para enviar, 1 si salio todo OK y no hay mas datos
	 */
	int enviar();
	
	/**
     * Recibe datos desde la memoria compartida y los escribe en pathDestino.
	 * @return 0 si salio todo OK y hay mas datos a recibir, 1 si salio todo OK y termino la transferencia
	 */
	int recibir();
	
	/**
     * Destructor de la Transferencia.
     */
	virtual ~Transferencia();
	

};

#endif /* TRANSF_H_ */
