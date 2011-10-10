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

void BufferMem::setTamArchivo(off_t tam) {
	this->tamanoArchivo = tam;
}

off_t BufferMem::getTamArchivo() {
	return this->tamanoArchivo;
}

void BufferMem::setTamEscrito(off_t tam) {
	this->bytesEscritos = tam;
}

off_t BufferMem::getTamEscrito() {
	return this->bytesEscritos;
}

bool BufferMem::esFin() {
	return this->esElFin;
}

void BufferMem::setFin(bool fin) {
	this->esElFin = fin;
}
