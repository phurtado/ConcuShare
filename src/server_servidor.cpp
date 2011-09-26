#include "servidor.h"

using namespace std;

Servidor::Servidor() {
	this->fifoLectura = new Fifo(NOMBREFIFOSERVIDOR);
	this->fifosEscritura = new list<Fifo>();
	this->mapaPaths = new map<TPID, ListaPaths*>();
	this->sigueEscuchando = true;
	
}

void Servidor::escucharComandos() {
	ParserComandos parser(buffer);
	while(this->sigueEscuchando) {
		int res = this->fifoLectura->leer(buffer, BUFSIZE);
		TCOM comando = parser.getComando();
		TPID pidProceso = parser.getPid();
		cout << "com = " << comando << ", proc = " <<pidProceso << endl;
		switch (comando) {
			case ALTA:
				altaCliente(pidProceso);
				break;
			case BAJA:
				bajaCliente(pidProceso);
				break;
			case LISTACOMP:
				size_t longitudPath = getLongitudPath(buffer);
				string pathArchivo = getPathArchivo(buffer, longitudPath);
				compartirArchivo(pathArchivo, pidProceso);
		}
		break;
	}
}

size_t Servidor::getLongitudPath(char *buffer) {
	size_t *l = (size_t *) (buffer + sizeof(TCOM) + sizeof(TPID));
	return *l;
}

string Servidor::getPathArchivo(char *buffer, size_t longitud) {
	char *pathArchivo = buffer + sizeof(TCOM) + sizeof(TPID) + sizeof(size_t);
	string path(pathArchivo, longitud);
	return path;
}

int Servidor::altaCliente(TPID pidCliente) {
	cout << "Alta cliente pid " << pidCliente << endl;
	(*this->mapaPaths)[pidCliente] = new ListaPaths();
	return 0;
}

int Servidor::bajaCliente(TPID pidCliente) {
	cout << "Baja cliente pid " << pidCliente << endl;
	this->mapaPaths->erase(pidCliente);
	return 0;
}

int Servidor::getListaCompartidos(TPID pidCliente) {
	cout << "Retornando lista para el pid " << pidCliente << endl;
	
	
	
	return 0;
}

int Servidor::compartirArchivo(string &pathArchivo, TPID pidCliente) {
	cout << "Compartiendo archivo " << pathArchivo << ", del pid " << pidCliente << endl;
	return 0;
}

int Servidor::descompartirArchivo(string &pathArchivo, TPID pidCliente) {
	return 0;
}

int Servidor::transferirArchivo(string &pathArchivo, TPID pidClienteOrigen,
		 TPID pidClienteDestino) {
			 return 0;
		 }

Servidor::~Servidor() {
	this->fifoLectura->cerrar();
	list<Fifo>::iterator it = this->fifosEscritura->begin();
	for(; it != this->fifosEscritura->end(); it++)
		it->cerrar();
	
	map<TPID, ListaPaths*>::iterator itM = this->mapaPaths->begin();
	for(; itM != this->mapaPaths->end(); itM++)
		delete itM->second;
	
	delete this->fifoLectura;
	delete this->fifosEscritura;
	delete this->mapaPaths;
}
		
