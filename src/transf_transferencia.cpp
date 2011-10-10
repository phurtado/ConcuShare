#include "transferencia.h"
	
#include <iostream>
#include <stdio.h>
#include "logger.h"
#include <sstream>

using namespace std;

Transferencia::Transferencia(string &pathOrigen, string &pathDestino) {
	// creo el archivo
	FILE *a = fopen(pathDestino.c_str(), "wb");
	fclose(a);
	
	this->memoriaCompartida = new MemoriaCompartida<BufferMem>();
	this->memoriaCompartida->crear(pathDestino.c_str(), LETRA);
	this->pathOrigen = pathOrigen;
	this->pathDestino = pathDestino;
	this->longitudArchivo = this->bytesTransferidos = 0;
	
	this->streamLectura = new ifstream(this->pathOrigen.c_str(), ifstream::binary);
	// obtengo la longitud del archivo a enviar
	this->streamLectura->seekg(0, ios::end);
	this->longitudArchivo = this->streamLectura->tellg();
	this->streamLectura->seekg(0, ios::beg);
	this->buffer.setTamArchivo(this->longitudArchivo);
	
	
	int valoresIniciales[2];
	// el semaforo de leer debe estar bloqueado inicialmente
	valoresIniciales[SEMLEER] = 0;
	// el de escribir, desbloqueado inicialmente
	valoresIniciales[SEMESCR] = 1;
	
	this->semaforos = new Semaforo(pathDestino.c_str(), 2, valoresIniciales);
}
	

int Transferencia::enviar() {
	
	this->streamLectura->read(this->buffer.getBuffer(), BUFMEMSIZE);
    stringstream ss;
    ss<<"Leidos de archivo: "<<this->buffer.getBuffer();
    Logger::log(ss.str());
	streampos posActual = this->streamLectura->tellg();
	
	if(posActual == EOF) {
		this->buffer.setFin(true);
		posActual = this->longitudArchivo;
	}
	
	this->buffer.setTamEscrito(posActual - this->bytesTransferidos);
	this->bytesTransferidos = posActual;
	
	// bloqueo la escritura hasta que se termine la lectura
	this->semaforos->p(SEMESCR);
    Logger::log("Escritura: Bloqueo escritura");
    Logger::log("Escritura: ENVIO: Voy a enviar");
	this->memoriaCompartida->escribir(buffer);
	// desbloqueo la lectura
	this->semaforos->v(SEMLEER);
	
    Logger::log("Escritura: YA envie");
    stringstream ss2;
    ss2<<"Escritura: posActual: " << posActual;
    Logger::log(ss2.str());
    stringstream ss3;
    ss3<<"Tamaño del archivo: " << this->longitudArchivo;
    Logger::log(ss3.str());
	
	return ((posActual >= this->longitudArchivo) or (posActual == EOF)) ? 1 : 0;
}

int Transferencia::recibir() {
	
	fstream escritura;
	escritura.open(this->pathDestino.c_str(), fstream::binary | fstream::in | fstream::out | fstream::app);
	
    Logger::log("Lectura: Bloqueo lectura");
	// bloqueo la lectura
	this->semaforos->p(SEMLEER);
	BufferMem b = this->memoriaCompartida->leer();
    stringstream ss;
    ss<<"Leido: "<<b.getBuffer()<<endl;
    Logger::log(ss.str());
	streampos pos = b.getTamEscrito();
	this->bytesTransferidos += pos;
	// desbloqueo la escritura
	this->semaforos->v(SEMESCR);
    Logger::log("Lectura: terminada.");
	escritura.write(b.getBuffer(), b.getTamEscrito());
	
    stringstream ss2;
    ss2 << "Lectura: Tamaño del Archivo: " << b.getTamArchivo() << ", transferidos: " << this->bytesTransferidos;
    Logger::log(ss2.str());
    escritura.close();
	return ((this->bytesTransferidos >= b.getTamArchivo()) || (b.esFin())) ? 1 : 0;
}


Transferencia::~Transferencia() {
	this->memoriaCompartida->liberar();
	delete this->memoriaCompartida;
	
	this->semaforos->eliminar();
	delete this->semaforos;
	
	if(this->streamLectura) {
		this->streamLectura->close();
		delete this->streamLectura;
	}
}
