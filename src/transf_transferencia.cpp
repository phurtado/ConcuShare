#include "transferencia.h"
	
#include <iostream>
#include <stdio.h>
#include "logger.h"
#include <sstream>

using namespace std;

Transferencia::Transferencia(string &pathOrigen, string &pathDestino, TRMODO modo) {
	switch(modo) {
		case ENVIAR:
			this->archivo = new LockFile(pathOrigen.c_str(), LECTURA);
			this->longitudArchivo = this->archivo->obtenerTamanioArchivo();
			this->bytesTransferidos = 0;
			break;
		case RECIBIR:
			this->archivo = new LockFile(pathDestino.c_str(), ESCRITURA);
			this->longitudArchivo = this->bytesTransferidos = 0;
			break;
	}
	
	this->memoriaCompartida = new MemoriaCompartida<BufferMem>();
	this->memoriaCompartida->crear(pathDestino.c_str(), LETRA);
	this->pathOrigen = pathOrigen;
	this->pathDestino = pathDestino;
	
	int valoresIniciales[2];
	// el semaforo de leer debe estar bloqueado inicialmente
	valoresIniciales[SEMLEER] = 0;
	// el de escribir, desbloqueado inicialmente
	valoresIniciales[SEMESCR] = 1;
	this->semaforos = new Semaforo(pathDestino.c_str(), 2, valoresIniciales);
}
	
void cualeselerror(int e) {
	cout << "Error en una operacion de semaforo: ";
	switch(e) {
		case E2BIG:
			cout << "E2BIG"  << endl;
			break;
		case EACCES:
			cout << "EACCES" << endl;
			break;
		case EAGAIN:
			cout << "EAGAIN" << endl;
			break;
		case EFAULT:
			cout << "EFAULT" << endl;
			break;
		case EFBIG:
			cout << "EFBIG" << endl;
			break;
		case EIDRM:
			cout << "IEDRM" << endl;
			break;
		case EINTR:
			cout << "EINTR" << endl;
			break;
		case EINVAL:
			cout << "EINVAL" << endl;
			break;
		case ENOMEM:
			cout << "ENOMEM" << endl;
			break;
		case ERANGE:
			cout << "ERANGE" << endl;
			break;
	}
	cout << "Verifique su manual de semaforos para saber de qué se trata." << endl;
}


int Transferencia::enviar() {
	stringstream ss;
	int res;
	char bufferLocal[BUFMEMSIZE];
	if(! this->archivo->archivoLockeado()) {
		Logger::instancia() << "Lockeando el archivo " << pathOrigen << " para lectura" << el;
		this->archivo->tomarLock();
	}
	
	off_t posActual = this->archivo->leer(bufferLocal, BUFMEMSIZE);
	this->buffer.setDatos(bufferLocal);
	
	this->buffer.setTamArchivo(this->longitudArchivo);
	this->buffer.setTamEscrito(posActual);
	this->bytesTransferidos += posActual;
	
	
	// bloqueo la escritura hasta que se termine la lectura
	if((res = this->semaforos->p(SEMESCR)) != 0) {
		cualeselerror(errno);
		exit(1);
	}
	
	this->memoriaCompartida->escribir(buffer);
	
	// desbloqueo la lectura
	if((res = this->semaforos->v(SEMLEER)) != 0) {
		cualeselerror(errno);
		exit(2);
	}
	
    Logger::instancia()<<"Escritura: Bytes leidos de archivo: " << posActual << el;
    
    
	
	bool llegueAlFin = this->bytesTransferidos >= this->longitudArchivo;
	if(llegueAlFin) {
		Logger::instancia() << "Liberando archivo " << pathOrigen << el;
		archivo->liberarLock();
	}
	return (llegueAlFin) ? 1 : 0;
}

int Transferencia::recibir() {
	int res;
	stringstream ss;
	if(! archivo->archivoLockeado()) {
		archivo->tomarLock();
		Logger::instancia() << "Lockeando el archivo " << pathDestino << " para escritura" << el;
		
	}
	
    
    Logger::instancia() << "Lectura: Bloqueo lectura " << el;
	
	// bloqueo la lectura
	if((res = this->semaforos->p(SEMLEER)) != 0) {
		cualeselerror(errno);
		exit(3);
	}
	
	BufferMem b = this->memoriaCompartida->leer();
	
	off_t pos = b.getTamEscrito();
	this->bytesTransferidos += pos;
	
	// desbloqueo la escritura
	if((res = this->semaforos->v(SEMESCR)) != 0) {
		cualeselerror(errno);
		exit(4);
	}
	
	archivo->escribir(b.getBuffer(), b.getTamEscrito());
    Logger::instancia() << "Lectura: Tamaño del Archivo: " << 
    b.getTamArchivo() << ", transferidos: " << this->bytesTransferidos << el;
    
    
    
    bool archivoCompleto = this->bytesTransferidos >= b.getTamArchivo();
    if(archivoCompleto) {
		
		Logger::instancia() << "Liberando el archivo " << pathDestino << el;
		
		archivo->liberarLock();
	}
    
	return (archivoCompleto) ? 1 : 0;
}


Transferencia::~Transferencia() {
	this->memoriaCompartida->liberar();
	delete this->memoriaCompartida;
	
	this->semaforos->p(SEMESCR);
	this->semaforos->eliminar();
	delete this->semaforos;
	
	delete this->archivo;
}
