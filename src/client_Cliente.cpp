#include "Cliente.h"
#include <iostream>
#include <sstream>
using namespace std;

Cliente::Cliente(){
  this->fifoLectura = NULL;
  this->fifoEscritura = new Fifo(NOMBREFIFOSERVIDOR);
  this->semEscritura = new Semaforo( (char*) NOMBREFIFOSERVIDOR, 0);
	this->estaConectado = false;
}

Cliente::~Cliente() {
  if(this->estaConectado)
		this->desconectar();
  if(this->fifoLectura)
		delete this->fifoLectura;
  delete this->fifoEscritura;
	delete this->semEscritura;
}

void Cliente::conectarAlServidor(){
    stringstream ss;
    if(this->estaConectado) {
    	cout << "Ya se encuentra conectado al servidor" << endl;
    	return;
    }
    ss << "fifo" << getpid();
    this->fifoLectura = new Fifo(ss.str().c_str());
    this->escribirMensajeAlServidor(ALTA, "");
    char buffer[100];
    int bytesLeidos = this->fifoLectura->leer(buffer, 100);
		buffer[bytesLeidos] = '\0';
	
    if(strcmp(buffer,ALTAOK) == 0) {
    	cout << "Conexión realizada con éxito" << endl;
			this->estaConectado = true;
    }
    
}

void Cliente::desconectar() {
	char buffer[100];
	if(this->estaConectado == true) {
		this->estaConectado = false;
		this->escribirMensajeAlServidor(BAJA, "");
		int bytesLeidos = this->fifoLectura->leer(buffer, 100);
		buffer[bytesLeidos] = 0;
		if(strcmp(buffer,BAJAOK) == 0) {
			cout << "Desconexión realizada con éxito" << endl;
			this->estaConectado = false;
			this->fifoLectura->cerrar();
			delete this->fifoLectura;
			this->fifoLectura = NULL;
		}
	}
	else {
		cout << "No se encuentra conectado al servidor" << endl;
	}
}

int Cliente::compartirArchivo(string &pathArchivo){
    ParserComandos parser;
    char *aEnviar = parser.armarCompDescomp(COMPARCH, getpid(), pathArchivo);
    // Incremento al semáforo para desbloquear al fifo del servidor
    this->semEscritura->v();
    this->fifoEscritura->escribir(aEnviar, parser.obtenerTamanioCompDescomp(pathArchivo));
    return 0;
}

int Cliente::dejarDeCompartirArchivo(string &pathArchivo){
    ParserComandos parser;
    char *aEnviar = parser.armarCompDescomp(DESCOMPARCH, getpid(), pathArchivo);
    // Incremento al semáforo para desbloquear al fifo del servidor
    this->semEscritura->v();
    this->fifoEscritura->escribir(aEnviar, parser.obtenerTamanioCompDescomp(pathArchivo));
    return 0;
}

map<TPID,ListaPaths*>* Cliente::getCompartidos(){
    char buffer[BUFSIZE], *bufDef = buffer;
    size_t tamLista;
    escribirMensajeAlServidor(LISTACOMP,"");
    this->fifoLectura->leer(buffer,BUFSIZE);
    memcpy((void *) &tamLista, (void *) buffer, sizeof(size_t));
    
    if(tamLista > BUFSIZE) {
		bufDef = new char[tamLista];
		memcpy((void *) bufDef, (void *) buffer, BUFSIZE);
		this->fifoLectura->leer(bufDef + BUFSIZE, tamLista - BUFSIZE);
		}

		ParserComandos parser(bufDef);
		map<TPID,ListaPaths*>* mapa = parser.obtenerListaCompartidos();
		if(bufDef != buffer)
			delete[] bufDef;
		return mapa;
}

int Cliente::empezarTransferencia(string destPath, string sharePath, TPID pid) {
	// Le pido al servidor la lista de archivos compartidos y valido
	// que sharePath y pid sean válidos
	map< TPID, ListaPaths* >* hash = this->getCompartidos(); 
	map< TPID, ListaPaths* >::iterator itM = hash->find(pid);
	if (itM == hash->end()) {
		// El cliente no existe !
		cout << "El cliente con el pid: " << pid << " no se encuentra conectado" << endl;
		return -1;
	}

	// El cliente existe..., verifico si el mismo está compartiendo el archivo que
	// pide el cliente.
	ListaPaths::iterator itL = itM->second->begin();
	for (; itL != itM->second->end(); itL++) {
		if ( *itL == sharePath ) 
			break;
	}
		
	if ( itL == itM->second->end()) {
		// El archivo compartido no existe...
		cout << "El cliente con el pid: " << pid << " no está compartiendo el archivo " << sharePath << endl;
		return -2;
	}
	
	// El archivo existe!!!!, podemos comenzar la transferencia
	cout << "Debería transmitir el archivo!!" << endl;
	// this->escribirMensajeAlServidor(PEDIRARCH,  
    return 0;
}

bool Cliente::conectado() {
	return this->estaConectado;
}


int Cliente::escribirMensajeAlServidor(TCOM tipo,string mensaje){
	// Abro el fifo de lectura del servidor
    size_t longMensaje = mensaje.length();
    char* msj = new char[sizeof(TCOM) + sizeof(TPID) + longMensaje];
    TPID pid = getpid();
	
    // Formo el mensaje compuesto de: tipo de operación + pid + mensaje
    memcpy(msj, &tipo, sizeof(TCOM));
    memcpy(msj + sizeof(TCOM), &pid, sizeof(TPID));
    memcpy(msj + sizeof(TCOM) + sizeof(TPID), mensaje.c_str(), longMensaje);
    //cout << "VOY A ESCRIBIR " << pid << " " << tipo << " " << mensaje << endl;
	
    // Incremento al semáforo para desbloquear al fifo del servidor
    this->semEscritura->v();
    this->fifoEscritura->escribir(msj, sizeof(TCOM) + sizeof(TPID) + longMensaje);
    // Libero la memoria de la fifo
    return 0;
}
