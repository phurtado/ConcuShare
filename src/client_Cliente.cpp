#include "Cliente.h"
#include <iostream>
#include <sstream>
using namespace std;

Cliente::Cliente(){
    this->fifoEscritura = new Fifo(NOMBREFIFOSERVIDOR);
    stringstream ss;
    ss<<"fifo"<<getpid();
    this->fifoLectura = new Fifo(ss.str().c_str());
}

Cliente::~Cliente(){
    this->fifoEscritura->cerrar();
    this->fifoLectura->cerrar();
    delete this->fifoEscritura;
    delete this->fifoLectura;
}

void Cliente::conectarAlServidor(){
    escribirMensajeAlServidor(ALTA,"");
}

void Cliente::desconectar(){
    this->fifoEscritura->cerrar();
    this->fifoLectura->cerrar();
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
    size_t longMensaje = mensaje.length();
    char* msj = new char[sizeof(TCOM) + sizeof(TPID) + longMensaje];
    strlen(msj);
    TPID pid = getpid();
    memcpy(msj,&tipo,sizeof(TCOM));
    memcpy(msj+sizeof(TCOM),&pid,sizeof(TPID));
    memcpy(msj+sizeof(TCOM)+sizeof(TPID),mensaje.c_str(),longMensaje);
    cout<<"VOY A ESCRIBIR "<<pid<<" "<<tipo<<" "<<mensaje<<endl;
    fifoEscritura->escribir(msj,sizeof(TCOM) +sizeof(TPID) + longMensaje);
    return 0;
}
