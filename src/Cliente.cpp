#include "Cliente.h"

using namespace std;

Cliente::Cliente(){
    this->fifoLectura = new Fifo(NOMBREFIFOSERVIDOR);
}

Cliente::~Cliente(){

}

void Cliente::conectarAlServidor(){

}

void Cliente::desconectar(){
    delete fifoLectura;
}

int Cliente::compartirArchivo(string pathArchivo){
    return 0;
}

int Cliente::dejarDeCompartirArchivo(string pathArchivo){
    return 0;
}

map<TPID,ListaPaths*>* Cliente::getCompartidos(){
    return NULL;
}

int Cliente::iniciarDescarga(string pathArchivo){
    return 0;
}
