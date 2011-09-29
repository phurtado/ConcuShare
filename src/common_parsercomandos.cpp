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
	size_t l;
	char *destino = this->buffer + sizeof(TCOM) + sizeof(TPID);
	memcpy((void *) &l, (void *) destino , sizeof(size_t));
	return l;
}

string ParserComandos::getPath() {
	size_t longitud = getTamanoStringPath();
	char *pathArchivo = this->buffer + sizeof(TCOM) + sizeof(TPID) + sizeof(size_t);
	string path(pathArchivo, longitud);
	return path;
}

TPID ParserComandos::getPidDestino() {
	TPID pidDestino;
	size_t longitudPath = getTamanoStringPath();
	char *pidDest = this->buffer + sizeof(TCOM) + sizeof(TPID) + sizeof(size_t) +
						longitudPath;
	memcpy((void *) &pidDestino, (void *) pidDest, sizeof(TPID));
	return pidDestino;
}

map<TPID, ListaPaths*>* ParserComandos::obtenerListaCompartidos() {
	
	map<TPID, ListaPaths*> *mapa = new map<TPID, ListaPaths*>();
	unsigned int tamanioLista;
	TPID pidCliente;
	char *posicionActual = this->buffer;
	
	// recupero el tamanio total de la lista
	memcpy((void *) & tamanioLista, (void *) posicionActual, sizeof(size_t));
	posicionActual += sizeof(size_t);
	
	while(posicionActual - this->buffer < tamanioLista) {
		// recupero el PID del cliente
		memcpy((void *) & pidCliente, (void *) posicionActual, sizeof(TPID));
		posicionActual += sizeof(TPID);
		
		ListaPaths *lista = new ListaPaths();
		posicionActual = hidratarLista(posicionActual, lista);
		mapa->insert(pair< TPID, ListaPaths* >(pidCliente, lista));
	}
	return mapa;
}


char * ParserComandos::hidratarLista(char *posicion, ListaPaths *lista) {
	size_t tamanio, tamPath, nPathActual = 0;
	// recupero el tamanio de la lista de paths
	memcpy((void *) & tamanio, (void *) posicion, sizeof(size_t));
	posicion += sizeof(size_t);
	
	while(nPathActual++ < tamanio) {
		// recupero el tamanio del path
		
		memcpy((void *) & tamPath, (void *) posicion, sizeof(size_t));
		posicion += sizeof(size_t);
		
		char *path = NULL;
		if(tamPath > 0)
			path = new char(tamPath);
		memcpy((void *) path, (void *) posicion, tamPath);
		posicion += tamPath;
		lista->push_back(string(path, tamPath));
	}
	return posicion;
}


char *ParserComandos::serializarLista(map<TPID, ListaPaths*> &mapaCompartidos, size_t tam) {
	
	map<TPID, ListaPaths*>::iterator itM = mapaCompartidos.begin();
	size_t tamanioLista = (tam == 0) ? obtenerTamanioLista(mapaCompartidos) : tam;
	
	char *bufferLista = (char *) calloc(tamanioLista, sizeof(char));
	char *posicionActual = bufferLista;
	
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

char* ParserComandos::serializarListaCliente(ListaPaths &lista, char * posicion) {
	ListaPaths::iterator itL = lista.begin();
	// copio el tamanio de la lista del cliente
	size_t tamanio = lista.size();
	memcpy((void *) posicion, (void *) & tamanio, sizeof(size_t));
	posicion += sizeof(size_t);
	
	for(; itL != lista.end(); itL++) {	
		// copio el tamanio del path (numero de caracteres de la ruta del archivo)
		size_t tamPath = itL->size();
		memcpy((void *) posicion, (void *) & tamPath, sizeof(size_t));
		posicion += sizeof(size_t);
		// copio el path
		memcpy((void *) posicion, (void *) itL->c_str(), tamPath);
		posicion += tamPath;
	}
	return posicion;
}


size_t ParserComandos::obtenerTamanioLista(map<TPID, ListaPaths*> &mapa) {
	// suma el PID para cada cliente y el tamanio de su lista
	size_t tamanio = sizeof(TPID) * mapa.size();
	tamanio += sizeof(size_t) * mapa.size();
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

size_t ParserComandos::obtenerTamanioComando() {
	TCOM comando = getComando();
	switch (comando) {
			case ALTA:
			case BAJA:
			case LISTACOMP:
				return sizeof(TCOM) + sizeof(TPID);
			case COMPARCH:
			case DESCOMPARCH:
				return sizeof(TCOM) + sizeof(TPID) + sizeof(size_t) + 
					getTamanoStringPath();
			case PEDIRARCH:
				cout << "PEDIR ARCHIVOOO" << endl;
				return sizeof(TCOM) + 2 * sizeof(TPID) + sizeof(size_t) + 
					getTamanoStringPath();
	}
	return -1;
}
