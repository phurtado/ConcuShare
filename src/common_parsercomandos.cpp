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

string ParserComandos::getPathDestino() {
	size_t longitudPath = getTamanoStringPath();
	size_t longitudPathDestino = getTamanioStringPathDestino();
	char *pathDestino = this->buffer + sizeof(TCOM) + 2 * sizeof(TPID) + 
			2 * sizeof(size_t) + longitudPath;
	return string(pathDestino, longitudPathDestino);
}

size_t ParserComandos::getTamanioStringPathDestino() {
	size_t longitud;
	char *destino = this->buffer + sizeof(TCOM) + sizeof(TPID) + 
			sizeof(size_t);
	memcpy((void *) &longitud, (void *) destino, sizeof(size_t));
	return longitud;
}

TPID ParserComandos::getPidClienteDuenioArchivo() {
	char *pidCliente = this->buffer + sizeof(TCOM) + sizeof(TPID) + 
			2 * sizeof(size_t);
	size_t pidClienteSizeT;
	memcpy((void *) &pidClienteSizeT, (void *) pidCliente, sizeof(TPID));
	return pidClienteSizeT;
}

std::string ParserComandos::getPathArchivoSolicitado() {
	char *path = this->buffer + sizeof(TCOM) + 2 * sizeof(TPID) + 
			2 * sizeof(size_t);
	size_t longitud = getTamanoStringPath();
	return string(path, longitud);
}

char* ParserComandos::armarCompDescomp(TCOM comando, TPID pid, std::string &path) {
	char *comandoSerializado = new char[sizeof(TPID) + sizeof(TCOM) + 
								sizeof(size_t) + path.size()];
	size_t longPath = path.size();
	char *posicion = comandoSerializado;
	memcpy((void *)posicion, (void *) &comando, sizeof(TCOM));
	posicion += sizeof(TCOM);
	memcpy((void *)posicion, (void *) &pid, sizeof(TPID));
	posicion += sizeof(TPID);
	memcpy((void *)posicion, (void *) &longPath, sizeof(size_t));
	posicion += sizeof(size_t);
	memcpy((void *)posicion, (void *) path.c_str(), longPath);
	return comandoSerializado;
}

char *armarSolicitarTransf(TCOM comando, TPID pidSolicitante, TPID pidDuenio, 
							std::string &pathArchivo, std::string &pathDestino) {
	size_t tamPathArch = pathArchivo.size();
	size_t tamPathDest = pathDestino.size();
	char *comandoSerializado = new char[2 * sizeof(TPID) + sizeof(TCOM) + 
								2 * sizeof(size_t) + tamPathArch + 
								tamPathDest];
	char *pos = comandoSerializado;
	
	memcpy((void *) pos, (void *) &comando, sizeof(TCOM));
	pos += sizeof(TCOM);
	memcpy((void *) pos, (void *) &pidSolicitante, sizeof(TPID));
	pos += sizeof(TPID);
	memcpy((void *) pos, (void *) &tamPathArch, sizeof(size_t));
	pos += sizeof(size_t);
	memcpy((void *) pos, (void *) &tamPathDest, sizeof(size_t));
	pos += sizeof(size_t);
	memcpy((void *) pos, (void *) &pidDuenio, sizeof(TPID));
	pos += sizeof(TPID);
	memcpy((void *) pos, (void *) pathArchivo.c_str(), tamPathArch);
	pos += tamPathArch;
	memcpy((void *) pos, (void *) pathDestino.c_str(), tamPathDest);
	return comandoSerializado;
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
		
		if(tamPath > 0) {
			char *path = new char[tamPath];
			memcpy((void *) path, (void *) posicion, tamPath);
			posicion += tamPath;
			lista->push_back(string(path, tamPath));
		}
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

size_t ParserComandos::obtenerTamanioCompDescomp(const string &path) {
	return sizeof(TPID) + sizeof(TCOM) + sizeof(size_t) + path.size();
}

size_t ParserComandos::obtenerTamanioSolicitarTransf(const string &pathArchivo, 
										const string &pathDestino) {
	size_t tamPathArch = pathArchivo.size();
	size_t tamPathDest = pathDestino.size();
	return 2 * sizeof(TPID) + sizeof(TCOM) + 2 * sizeof(size_t) + tamPathArch + 
								tamPathDest;
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
				return sizeof(TCOM) + 2 * sizeof(TPID) + 2 * sizeof(size_t) + 
					getTamanoStringPath() + getTamanioStringPathDestino();
	}
	return -1;
}
