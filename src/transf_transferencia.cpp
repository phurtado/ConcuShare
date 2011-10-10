#include "transferencia.h"
	
#include <iostream>
#include <stdio.h>

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
	//cout << "Leidos de archivo: " << this->buffer.getBuffer() << endl;
	streampos posActual = this->streamLectura->tellg();
	
	if(posActual == EOF) {
		this->buffer.setFin(true);
		posActual = this->longitudArchivo;
	}
	
	this->buffer.setTamEscrito(posActual - this->bytesTransferidos);
	this->bytesTransferidos = posActual;
	
	//cout << "Escritura: Bloqueo escritura" << endl;
	// bloqueo la escritura hasta que se termine la lectura
	this->semaforos->p(SEMESCR);
	//cout << "Escritura: ENVIO: Voy a enviar, YA" << endl;
	this->memoriaCompartida->escribir(buffer);
	// desbloqueo la lectura
	this->semaforos->v(SEMLEER);
	
	//cout << "Escritura: YA envie" << endl;
	//cout << "Escritura: posActual: " << posActual << endl;
	//cout << "tamArch: " << this->longitudArchivo << endl;
	
	return ((posActual >= this->longitudArchivo) or (posActual == EOF)) ? 1 : 0;
}

int Transferencia::recibir() {
	
	fstream escritura;
	escritura.open(this->pathDestino.c_str(), fstream::binary | fstream::in | fstream::out | fstream::app);
	
	//cout << "Lectura: Bloqueo lectura" << endl;
	// bloqueo la lectura
	this->semaforos->p(SEMLEER);
	BufferMem b = this->memoriaCompartida->leer();
	//cout << "Leo: " << b.getBuffer() << endl;
	streampos pos = b.getTamEscrito();
	this->bytesTransferidos += pos;
	// desbloqueo la escritura
	this->semaforos->v(SEMESCR);
	//cout << "Lectura: termine" << endl;
	escritura.write(b.getBuffer(), b.getTamEscrito());
	
	//cout << "Lectura: TamArch: " << b.getTamArchivo() << ", transferidos: " << this->bytesTransferidos << endl;
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
