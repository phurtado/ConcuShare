#include "parsercomandos.h"

using namespace std;

ParserComandos::ParserComandos() {
	setBuffer(NULL);
}

ParserComandos::ParserComandos(char *buffer) {
	setBuffer(buffer);
}
		

void ParserComandos::setBuffer(char *buffer) {
	this->buffer = buffer;
}


TCOM ParserComandos::getComando() {
	TCOM comando = *((TCOM *) this->buffer);
	return comando;
}

TPID ParserComandos::getPid() {
	TPID pidProceso = *((TPID*) (this->buffer + sizeof(TCOM)));
	return pidProceso;
}

std::size_t ParserComandos::getTamanoStringPath() {
	return 0;
}

char *getPath() {
	return NULL;
}

map<TPID, ListaPaths*>* ParserComandos::obtenerListaCompartidos() {
	map<TPID, ListaPaths*> *mapa = new map<TPID, ListaPaths*>();
	size_t tamanioLista, cantidadListas = 0;
	TPID pidCliente;
	char *posicionActual = this->buffer;
	// recupero el tamanio total de la lista
	memcpy((void *) & tamanioLista, (void *) posicionActual, sizeof(size_t));
	posicionActual += sizeof(size_t);
	
	while(cantidadListas++ < tamanioLista) {
		// recupero el PID del cliente
		memcpy((void *) & pidCliente, (void *) posicionActual, sizeof(TPID));
		posicionActual += sizeof(TPID);
		
		ListaPaths *lista = new ListaPaths();
		posicionActual = hidratarLista(posicionActual, lista);
	}
	return mapa;
}


char * ParserComandos::hidratarLista(char *posicion, ListaPaths *lista) {
	size_t tamanio, nPathActual = 0;
	// recupero el tamanio de la lista de paths
	memcpy((void *) & tamanio, (void *) posicion, sizeof(size_t));
	posicion += sizeof(size_t);
	
	while(nPathActual++ < tamanio) {
		// recupero el tamanio del path
		memcpy((void *) & tamanio, (void *) posicion, sizeof(size_t));
		posicion += sizeof(size_t);
		
		char *path = NULL;
		if(tamanio > 0)
			path = new char(tamanio);
		memcpy((void *) path, (void *) posicion, tamanio);
		lista->push_back(string(path, tamanio));
	}
	return posicion;
}


char *ParserComandos::serializarLista(map<TPID, ListaPaths*> &mapaCompartidos) {
	size_t tamanioLista = obtenerTamanioLista(mapaCompartidos);
	char *bufferLista = new char(tamanioLista), *posicionActual = bufferLista;
	
	map<TPID, ListaPaths*>::iterator itM = mapaCompartidos.begin();
	
	// copio el tamanio total de la lista
	memcpy((void *) posicionActual, (void *) & tamanioLista, sizeof(size_t));
	posicionActual += sizeof(size_t);
	
	for(; itM != mapaCompartidos.end(); itM++) {
		// copio el PID
		memcpy((void *) posicionActual, (void *) &itM->first, sizeof(TPID));
		posicionActual += sizeof(TPID);
		
		ListaPaths *lista = itM->second;
		posicionActual = serializarListaCliente(*lista, posicionActual);
	}
	
	return bufferLista;
}

char* ParserComandos::serializarListaCliente(ListaPaths &lista, char * buffer) {
	ListaPaths::iterator itL = lista.begin();
	// copio el tamanio de la lista del cliente
	size_t tamanio = lista.size();
	memcpy((void *) buffer, (void *) & tamanio, sizeof(size_t));
	buffer += sizeof(size_t);
	// copio los tamanios de los paths y los paths
	for(; itL != lista.end(); itL++) {
		// copio el tamanio del path (numero de caracteres de la ruta del archivo)
		tamanio = lista.size();
		memcpy((void *) buffer, (void *) & tamanio, sizeof(size_t));
		buffer += sizeof(size_t);
		
		// copio el path
		memcpy((void *) buffer, (void *) itL->c_str(), itL->size());
		buffer += itL->size();
	}
	return buffer;
}


size_t ParserComandos::obtenerTamanioLista(map<TPID, ListaPaths*> &mapa) {
	size_t tamanio = 0;
	// suma el PID para cada cliente y el tamanio de su lista
	tamanio += 2 * sizeof(TPID) * mapa.size();
	
	// suma el tamanio de la lista entera
	tamanio += sizeof(size_t);
	
	map<TPID, ListaPaths*>::iterator itM = mapa.begin();
	ListaPaths::iterator itL;
	for(; itM != mapa.end(); itM++) {
		// sumo el tamanio de cada size_t que guardara el tam de cada path
		tamanio += sizeof(size_t) * itM->second->size();
		for(itL = itM->second->begin(); itL != itM->second->end(); itL++) {
			// sumo el tamanio de cada path
			tamanio += itL->size();
		}
	}
	return tamanio;
}
