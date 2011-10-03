#include "servidor.h"

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
	cout << "Semaforo" << endl;
	if(this->semLectura->p())
		cout << "Error en p" << endl;
	cout << "A leer" << endl;
	int res = this->fifoLectura->leer(this->buffer, BUFSIZE);
	int i = 2;

	if(res <= 0) // si es 0 es porque el cliente se cerro
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
		cout << "Leido resP = " << resProv << ", res = " << res << "tam = " << tam << endl;
		parser.setBuffer(this->buffer);
	}
	cout << "Caracteres leidos: " << res << endl;
	return 0;
}

void Servidor::escucharComandos() {
	ParserComandos parser(this->buffer);
	string pathArchivo;
	while(this->sigueEscuchando) {
		if(leerComando(parser))
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
				pathArchivo = parser.getPathArchivoSolicitado();
				string pathDestino = parser.getPathDestino();
				TPID pidClienteDuenio = parser.getPidClienteDuenioArchivo();
				transferirArchivo(pathArchivo, pathDestino, pidProceso, pidClienteDuenio);
				cout << "transferencia desde " << pidClienteDuenio << 
				" hasta " << pidProceso << 
				" el archivo " << pathArchivo << " de size " << 
				parser.getTamanoStringPath() << " al archivo " << 
				pathDestino << " de size " << parser.getTamanioStringPathDestino() << endl;
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
		// Cierro la fifo luego de mandar el mensaje
		//(*this->fifosEscritura)[pidCliente]->cerrar();
		delete (*this->fifosEscritura)[pidCliente];
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

int Servidor::transferirArchivo(string &pathArchivo, string &pathDestino, 
				TPID pidClienteDestino, TPID pidClienteDuenioArchivo) {
	
	TPID pid = fork();
	
	if(pid == 0) { // es el hijo
		execl("./transf", "transf", pathArchivo.c_str(), pathDestino.c_str(), 0);
	}
	
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
		
