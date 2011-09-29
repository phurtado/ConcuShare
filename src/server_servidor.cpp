#include "servidor.h"

using namespace std;

Servidor::Servidor() {
	this->fifoLectura = new Fifo(NOMBREFIFOSERVIDOR);
	this->fifosEscritura = new map< TPID, Fifo*>();
	this->mapaPaths = new map<TPID, ListaPaths*>();
	this->sigueEscuchando = true;
	this->buffer = (char *) calloc(BUFSIZE, sizeof(char));
}

void Servidor::setContinua(bool continua) {
	this->sigueEscuchando = continua;
}

int Servidor::leerComando(ParserComandos &parser) {
	int res = this->fifoLectura->leer(this->buffer, BUFSIZE), i = 2;
	if(res <= 0) // si es 0 es porque el cliente se cerro
		return -1;
	if(! this->sigueEscuchando)
		return 0;
	int tam = parser.obtenerTamanioComando();
	while(res < tam) {
		char *bufferNuevo = (char *) calloc(i * BUFSIZE, sizeof(char));
		memcpy((void *)bufferNuevo, buffer, (i - 1) * BUFSIZE);
		free(this->buffer);
		this->buffer = bufferNuevo;
		int resProv = this->fifoLectura->leer(this->buffer + (i - 1) * BUFSIZE, BUFSIZE);
		if(resProv < 0)
			return -1;
		res += resProv;
		i++;
		cout << "Leido resP = " << resProv << ", res = " << res << "tam = " << tam << endl;
		parser.setBuffer(this->buffer);
	}
	return 0;
}

void Servidor::escucharComandos() {
	ParserComandos parser(this->buffer);
	string pathArchivo;
	while(this->sigueEscuchando) {
		int res = leerComando(parser);
		if(res != 0)
			return;
		TCOM comando = parser.getComando();
		TPID pidProceso = parser.getPid();
		
		switch (comando) {
			case ALTA:
				altaCliente(pidProceso);
				break;
			case BAJA:
				bajaCliente(pidProceso);
				break;
			case COMPARCH:
				pathArchivo = parser.getPath();
				compartirArchivo(pathArchivo, pidProceso);
				break;
			case DESCOMPARCH:
				pathArchivo = parser.getPath();
				descompartirArchivo(pathArchivo, pidProceso);
				break;
			case LISTACOMP:
				enviarListaCompartidosACliente(pidProceso);
				break;
			case PEDIRARCH:
				pathArchivo = parser.getPath();
				TPID pidDestino = parser.getPidDestino();
				transferirArchivo(pathArchivo, pidProceso, pidDestino);
				cout << "transferencia desde " << pidProceso << " hasta " << pidDestino << 
				" el archivo " << pathArchivo << " de size " << 
				parser.getTamanoStringPath() << endl;
		}
	}
}

int Servidor::altaCliente(TPID pidCliente) {
	cout << "Alta cliente pid " << pidCliente << endl;
	map< TPID, ListaPaths* >::iterator itM = this->mapaPaths->find(pidCliente);
	if(itM != this->mapaPaths->end()) // lo encontro
		return -1;
	
	(*this->mapaPaths)[pidCliente] = new ListaPaths();
	
	// TODO: Agregar la FIFO a la lista de FIFOS de clientesssss
	
	return 0;
}

int Servidor::bajaCliente(TPID pidCliente) {
	cout << "Baja cliente pid " << pidCliente << endl;
	map< TPID, ListaPaths* >::iterator itM = this->mapaPaths->find(pidCliente);
	if(itM != this->mapaPaths->end()) {// lo encontro
		this->mapaPaths->erase(itM);
		return 0;
		// TODO: Eliminar la FIFO de la lista de FIFOS de clientesssss
		
	}
	return -1;
}

int Servidor::enviarListaCompartidosACliente(TPID pidCliente) {
	cout << "Retornando lista para el pid " << pidCliente << endl;
	ParserComandos parser;
	std::map<TPID, Fifo*>::iterator itM = this->fifosEscritura->find(pidCliente);
	if(itM != this->fifosEscritura->end()) { // lo encontro, cliente valido
		size_t tamanioLista = parser.obtenerTamanioLista(*this->mapaPaths);
		char *listaSerializada = parser.serializarLista(*this->mapaPaths, tamanioLista);
		itM->second->escribir(listaSerializada, tamanioLista);
		return 0;
	}
	
	return -1;
}

int Servidor::compartirArchivo(string &pathArchivo, TPID pidCliente) {
	cout << "Compartiendo archivo " << pathArchivo << ", del pid " << pidCliente << endl;
	map< TPID, ListaPaths* >::iterator itM = this->mapaPaths->find(pidCliente);
	if(itM != this->mapaPaths->end()) { // lo encontro
		itM->second->push_back(pathArchivo);
		return 0;
	}
	//no encontro el cliente con el pid pidCliente
	return -1;
}

int Servidor::descompartirArchivo(string &pathArchivo, TPID pidCliente) {
	map< TPID, ListaPaths* >::iterator itM = this->mapaPaths->find(pidCliente);
	if(itM != this->mapaPaths->end()) { // lo encontro
		itM->second->remove(pathArchivo);
		return 0;
	}
	//no encontro el cliente con el pid pidCliente
	return -1;
}

int Servidor::transferirArchivo(string &pathArchivo, TPID pidClienteOrigen,
		 TPID pidClienteDestino) {
		
			 return 0;
		 }

Servidor::~Servidor() {
	this->fifoLectura->cerrar();
	map< TPID, Fifo*>::iterator itF = this->fifosEscritura->begin();
	for(; itF != this->fifosEscritura->end(); itF++)
		itF->second->cerrar();
	
	map<TPID, ListaPaths*>::iterator itM = this->mapaPaths->begin();
	for(; itM != this->mapaPaths->end(); itM++)
		delete itM->second;
	
	delete this->fifoLectura;
	delete this->fifosEscritura;
	delete this->mapaPaths;
	free(this->buffer);
}
		
