#include "servidor.h"
#include <sstream>
#include "logger.h"

Servidor::Servidor() {
	this->fifoLectura = new Fifo(NOMBREFIFOSERVIDOR);
	this->fifosEscritura = new map< TPID, Fifo*>();
	this->mapaPaths = new map<TPID, ListaPaths*>();
	this->listaHijos = new list< TPID >();
	this->sigueEscuchando = true;
	this->buffer = (char *) calloc(BUFSIZE, sizeof(char));
	this->semLectura = new Semaforo((char*) NOMBREFIFOSERVIDOR, 0);
}

void Servidor::setContinua(bool continua) {
	this->sigueEscuchando = continua;
}

int Servidor::leerComando(ParserComandos &parser) {
	// Decremento el semáforo para que el fifo se bloquee hasta que
	// algún cliente le escriba algo
	this->semLectura->p();
	
	int res = this->fifoLectura->leer(this->buffer, BUFSIZE), i = 2;
	
	if(res < 0)
		return -1;
	
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
		parser.setBuffer(this->buffer);
	}
	return 0;
}

void Servidor::escucharComandos() {
	ParserComandos parser(this->buffer);
	string pathArchivo, pathDestino;
	TPID pidClienteDuenio = 0;
	while(this->sigueEscuchando) {
		if(leerComando(parser))
			return;
		buscarHijosQueTerminaron(this->listaHijos);
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
				pathArchivo = parser.getPathArchivoSolicitado();
				pathDestino = parser.getPathDestino();
				pidClienteDuenio = parser.getPidClienteDuenioArchivo();
				transferirArchivo(pathArchivo, pathDestino, pidProceso, pidClienteDuenio);
				break;
				default:
					cout << "Comando incorrecto: " << comando << endl;
		}
	}
}
	
int Servidor::altaCliente(TPID pidCliente) {
	cout << "Alta cliente pid " << pidCliente << endl;
	map< TPID, ListaPaths* >::iterator itM = this->mapaPaths->find(pidCliente);
	
	if(itM != this->mapaPaths->end()) {
		// Lo encontro, le envió al cliente un -1 indicandole que ya está conectado.
		(*this->fifosEscritura)[pidCliente]->escribir((char*) ALTAWRONG, strlen(ALTAWRONG));
		return -1;
	}
	
	// El cliente no está en la lista, lo tengo que dar de alta.
	// Creo el fifo del cliente
	stringstream ss;
	ss << "fifo" << pidCliente;
	(*this->fifosEscritura)[pidCliente] = new Fifo(ss.str().c_str());
	(*this->fifosEscritura)[pidCliente]->escribir((char*) ALTAOK, strlen(ALTAOK));
	(*this->mapaPaths)[pidCliente] = new ListaPaths();
	return 0;
}

int Servidor::bajaCliente(TPID pidCliente) {
	cout << "Baja cliente pid " << pidCliente << endl;
	map< TPID, ListaPaths* >::iterator itM = this->mapaPaths->find(pidCliente);

	if(itM != this->mapaPaths->end()) { 
		// Lo encontro
		this->mapaPaths->erase(itM);
		(*this->fifosEscritura)[pidCliente]->escribir((char*) BAJAOK, strlen(BAJAOK));
		
		delete (*this->fifosEscritura)[pidCliente];
		this->fifosEscritura->erase(pidCliente);
		return 0;
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
		cout << "Enviando lista a cliente " << itM->first << " TAMLista: " << tamanioLista << endl;
		itM->second->escribir(listaSerializada, tamanioLista);
		return 0;
	}
	return -1;
}

int Servidor::compartirArchivo(string &pathArchivo, TPID pidCliente) {
	cout << "Compartiendo archivo " << pathArchivo << ", del pid " << pidCliente << endl;
	if(! archivoCompartidoActualmente(pathArchivo)) {
		map< TPID, ListaPaths* >::iterator itM = this->mapaPaths->find(pidCliente);
		if(itM != this->mapaPaths->end()) { // lo encontro
			itM->second->push_back(pathArchivo);
			(*this->fifosEscritura)[pidCliente]->escribir((char*) COMPOK, strlen(COMPERROR));
			return 0;
		}
	}  //archivo compartido de antes o no encontro el cliente
	(*this->fifosEscritura)[pidCliente]->escribir((char*) COMPERROR, strlen(COMPERROR));
	return -1;
}

int Servidor::archivoCompartidoActualmente(string &pathArchivo) {
	map<TPID, ListaPaths*>::iterator itM = this->mapaPaths->begin();
	ListaPaths::iterator itL;
	for(; itM != this->mapaPaths->end(); itM++) {
		ListaPaths *lista = itM->second;
		if(find(lista->begin(), lista->end(), pathArchivo) != lista->end()) {
			return 1;
		}
	}
	return 0;
}

int Servidor::descompartirArchivo(string &pathArchivo, TPID pidCliente) {
	cout << "Dejando de compartir el archivo " << pathArchivo << endl;
	map< TPID, ListaPaths* >::iterator itM = this->mapaPaths->find(pidCliente);
	if(itM != this->mapaPaths->end()) { // lo encontro
		itM->second->remove(pathArchivo);
		(*this->fifosEscritura)[pidCliente]->escribir((char*) COMPOK, strlen(COMPERROR));
		return 0;
	}
	//no encontro el cliente con el pid pidCliente
	(*this->fifosEscritura)[pidCliente]->escribir((char*) COMPERROR, strlen(COMPERROR));
	return -1;
}

int Servidor::transferirArchivo(string &pathArchivo, string &pathDestino, 
				TPID pidClienteDestino, TPID pidClienteDuenioArchivo) {
	cout << "Transferencia desde " << pidClienteDuenioArchivo << 
				" hasta " << pidClienteDestino << 
				" el archivo " << pathArchivo << " al archivo " << 
				pathDestino << endl;

    
	TPID pid = fork();
	if(pid == 0) { // es el hijo
        if(Logger::isOpen()){ //si se abrió el cliente en modo debug, las transferencias asociadas también se ejecutarán en modo debug 
            execl("./transf", "transf", "E", pathArchivo.c_str(), pathDestino.c_str(), "--debug", 0);
        }
        else{
            execl("./transf", "transf", "E", pathArchivo.c_str(), pathDestino.c_str(), 0);
        }
	}
	else if(pid == -1) { // error de fork, notifico al cliente del error, no hay transferencia
		(*this->fifosEscritura)[pidClienteDestino]->escribir((char*) TRERROR, strlen(TROK));
		return -1;
	}
	else{
		(*this->fifosEscritura)[pidClienteDestino]->escribir((char*) TROK, strlen(TROK));
	    this->listaHijos->push_back(pid);
	    stringstream ss;
        ss<<"Ejecutando proceso de transferencia con PID "<<pid<<" desde "<<pidClienteDuenioArchivo<<" hasta "<<pidClienteDestino<<" del archivo "<<pathArchivo<<" al archivo "<<pathDestino<<"."<<endl;
        Logger::log(ss.str());
    }
	return 0;
}


Servidor::~Servidor() {
	this->fifoLectura->cerrar();
	map< TPID, Fifo*>::iterator itF = this->fifosEscritura->begin();
	for(; itF != this->fifosEscritura->end(); itF++)
		// el que la cierra es cada cliente
		delete (itF->second);
	map<TPID, ListaPaths*>::iterator itM = this->mapaPaths->begin();
	for(; itM != this->mapaPaths->end(); itM++)
		delete itM->second;
	
	// espero por hijos que no terminaron todavia
	list<TPID>::iterator itP = this->listaHijos->begin();
	for(; itP != this->listaHijos->end(); itP++)
		waitpid(*itP, NULL, 0);
	
	delete this->fifoLectura;
	delete this->fifosEscritura;
	delete this->mapaPaths;
	delete this->listaHijos;
	this->semLectura->eliminar();
	free(this->buffer);
}
		
