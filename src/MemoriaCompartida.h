#ifndef MEMORIACOMPARTIDA_H_
#define MEMORIACOMPARTIDA_H_

#define SHM_OK			0
#define	ERROR_FTOK		-1
#define ERROR_SHMGET	-2
#define	ERROR_SHMAT		-3

#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/shm.h>
#include <stdlib.h>

#include "logger.h"

template <class T> class MemoriaCompartida {

private:

	int 	shmId;
	T*		ptrDatos;

	int cantidadProcesosAdosados ();

public:

	MemoriaCompartida ();
	~MemoriaCompartida ();
	int crear ( const char *archivo,char letra );
	void liberar ();
	void escribir ( T &dato );
	T leer ();

};

template <class T> MemoriaCompartida<T> :: MemoriaCompartida () {
}

template <class T> MemoriaCompartida<T> :: ~MemoriaCompartida () {
}

template <class T> int MemoriaCompartida<T> :: crear (const char *archivo,char letra ) {

	// generacion de la clave
	key_t clave = ftok ( archivo,letra );
	if ( clave == -1 )
		return ERROR_FTOK;
	else {
		// creacion de la memoria compartida
		this->shmId = shmget ( clave,sizeof(T),0644|IPC_CREAT );

		if ( this->shmId == -1 )
			return ERROR_SHMGET;
		else {
			// attach del bloque de memoria al espacio de direcciones del proceso
			void* ptrTemporal = shmat ( this->shmId, NULL, 0 );
			
			if ( ptrTemporal == (void *) -1 ) {
				return ERROR_SHMAT;
			} else {
				this->ptrDatos = (T *) ptrTemporal;
				Logger::instancia() << "Creando memoria compartida " << this->shmId << el;
				return SHM_OK;
			}
		}
	}
}


template <class T> void MemoriaCompartida<T> :: liberar () {
	// detach del bloque de memoria
	shmdt ( (void *) this->ptrDatos );
	
	int procAdosados = this->cantidadProcesosAdosados ();
	
	if ( procAdosados == 0 ) {
		shmctl ( this->shmId,IPC_RMID,NULL );
	}
	Logger::instancia() << "Liberando memoria compartida " << this->shmId << 
	". Procesos adosados: " << procAdosados << el;
	
}

template <class T> void MemoriaCompartida<T> :: escribir ( T &dato ) {
	Logger::instancia() << "Escribiendo memoria compartida " << this->shmId << el;
	* (this->ptrDatos) = dato;
}

template <class T> T MemoriaCompartida<T> :: leer () {
	Logger::instancia() << "Leyendo memoria compartida " << this->shmId << el;
	return ( *(this->ptrDatos) );
}


template <class T> int MemoriaCompartida<T> :: cantidadProcesosAdosados () {
	shmid_ds estado;
	shmctl ( this->shmId,IPC_STAT,&estado );
	return estado.shm_nattch;
}


#endif /* MEMORIACOMPARTIDA_H_ */
