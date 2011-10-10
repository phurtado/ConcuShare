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
			this->buffer.setTamArchivo(this->longitudArchivo);
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
	

int Transferencia::enviar() {
	stringstream ss;
	if(! this->archivo->archivoLockeado()) {
		ss.clear();
		ss << "Lockeando el archivo " << pathOrigen << " para lectura" << endl;
		Logger::log(ss.str());
		this->archivo->tomarLock();
	}
	
	off_t posActual = this->archivo->leer(this->buffer.getBuffer(), BUFMEMSIZE);
	
	this->buffer.setTamEscrito(posActual);
	this->bytesTransferidos += posActual;
	
	// bloqueo la escritura hasta que se termine la lectura
	this->semaforos->p(SEMESCR);
	this->memoriaCompartida->escribir(buffer);
	// desbloqueo la lectura
	this->semaforos->v(SEMLEER);
	
    ss<<"Escritura: Bytes leidos de archivo: " << posActual;
    Logger::log(ss.str());
    
    ss.str("");
	
	bool llegueAlFin = this->bytesTransferidos >= this->longitudArchivo;
	if(llegueAlFin) {
		ss.clear();
		ss.str("");
		ss << "Liberando archivo " << pathOrigen << endl;
		Logger::log(ss.str());
		archivo->liberarLock();
	}
	return (llegueAlFin) ? 1 : 0;
}

int Transferencia::recibir() {
	
	stringstream ss;
	if(! archivo->archivoLockeado()) {
		archivo->tomarLock();
		ss << "Lockeando el archivo " << pathDestino << " para escritura" << endl;
		Logger::log(ss.str());
		ss.clear();
	}
	
    //Logger::log("Lectura: Bloqueo lectura ");
	// bloqueo la lectura
	this->semaforos->p(SEMLEER);
	BufferMem b = this->memoriaCompartida->leer();
    
	off_t pos = b.getTamEscrito();
	this->bytesTransferidos += pos;
	// desbloqueo la escritura
	this->semaforos->v(SEMESCR);
    //Logger::log("Lectura: terminada.");
	archivo->escribir(b.getBuffer(), b.getTamEscrito());
	
    stringstream ss2;
    ss2 << "Lectura: Tamaño del Archivo: " << b.getTamArchivo() << ", transferidos: " << this->bytesTransferidos;
    Logger::log(ss2.str());
    bool archivoCompleto = this->bytesTransferidos >= b.getTamArchivo();
    if(archivoCompleto) {
		ss.clear();
		ss << "Liberando el archivo " << pathDestino << endl;
		Logger::log(ss.str());
		archivo->liberarLock();
		ss.str("");
    
		ss<<"Tamaño del archivo: " << this->longitudArchivo;
    Logger::log(ss.str());
	}
    
	return ((archivoCompleto) || (b.esFin())) ? 1 : 0;
}


Transferencia::~Transferencia() {
	this->memoriaCompartida->liberar();
	delete this->memoriaCompartida;
	
	this->semaforos->eliminar();
	delete this->semaforos;
	
	delete this->archivo;
}
