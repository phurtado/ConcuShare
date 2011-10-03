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

Cliente::~Cliente(){
    // this->fifoEscritura->cerrar();
    // this->fifoLectura->cerrar();
    if(this->fifoLectura)
		delete this->fifoLectura;
    delete this->fifoEscritura;
	delete this->semEscritura;
}

void Cliente::conectarAlServidor(){
    stringstream ss;
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
    else {
    	cout << "Ya se encuentra conectado al servidor" << endl;
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

int Cliente::compartirArchivo(string pathArchivo){
    return escribirMensajeAlServidor(COMPARCH,pathArchivo);    
}

int Cliente::dejarDeCompartirArchivo(string pathArchivo){
    return escribirMensajeAlServidor(DESCOMPARCH,pathArchivo);
}

map<TPID,ListaPaths*>* Cliente::getCompartidos(){
    return NULL;
}

int Cliente::iniciarDescarga(string pathArchivo){
    return 0;
}

int Cliente::escribirMensajeAlServidor(TCOM tipo,string mensaje){
	// Abro el fifo de lectura del servidor
    size_t longMensaje = mensaje.length();
    char* msj = new char[sizeof(TCOM) + sizeof(TPID) + longMensaje];
    //strlen(msj);
    TPID pid = getpid();

    // Formo el mensaje compuesto de: tipo de operación + pid + mensaje
    memcpy(msj, &tipo, sizeof(TCOM));
    memcpy(msj + sizeof(TCOM), &pid, sizeof(TPID));
    memcpy(msj + sizeof(TCOM) + sizeof(TPID), mensaje.c_str(), longMensaje);
    cout << "VOY A ESCRIBIR " << pid << " " << tipo << " " << mensaje << endl;
	
    // Incremento al semáforo para desbloquear al fifo del servidor
    this->semEscritura->v();
		cout << "Antes de Escribir??" << endl;
    this->fifoEscritura->escribir(msj, sizeof(TCOM) + sizeof(TPID) + longMensaje);
		cout << "Después de escribir??" << endl;
    // Libero la memoria de la fifo
    return 0;
}
