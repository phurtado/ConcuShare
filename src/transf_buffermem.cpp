#include "buffermem.h"

using namespace std;

BufferMem::BufferMem() {
	this->esElFin = false;
}
		

char *BufferMem::getBuffer() {
	return this->buffer;
}

BufferMem::~BufferMem() {
}

void BufferMem::setTamArchivo(streampos tam) {
	this->tamanoArchivo = tam;
}

streampos BufferMem::getTamArchivo() {
	return this->tamanoArchivo;
}

void BufferMem::setTamEscrito(streampos tam) {
	this->bytesEscritos = tam;
}

streampos BufferMem::getTamEscrito() {
	return this->bytesEscritos;
}

bool BufferMem::esFin() {
	return this->esElFin;
}

void BufferMem::setFin(bool fin) {
	this->esElFin = fin;
}
