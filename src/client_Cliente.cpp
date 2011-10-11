#include "Cliente.h"
#include <iostream>
#include <sstream>
#include "logger.h"
using namespace std;

int existeArchivo(string &path) {
	int res = open(path.c_str(), O_RDWR | O_CREAT | O_EXCL, 0755);
	if(res == -1) // el archivo existe, no se crea
		return 1;
	return 0;
}


Cliente::Cliente(){
  this->fifoLectura = NULL;
  this->fifoEscritura = new Fifo(NOMBREFIFOSERVIDOR);
  this->semEscritura = new Semaforo( (const char*) NOMBREFIFOSERVIDOR, 0);
	this->estaConectado = false;
	this->listaHijos = new list<TPID>();
}

Cliente::~Cliente() {
  if(this->estaConectado)
		this->desconectar();
  if(this->fifoLectura) {
		this->fifoLectura->cerrar();
		delete this->fifoLectura;
	}
  delete this->fifoEscritura;
	delete this->semEscritura;
	
	list<TPID>::iterator it = this->listaHijos->begin();
	for(; it != this->listaHijos->end(); it++) {
		Logger::instancia() << "Esperando por " << *it << el;
		waitpid(*it, NULL, 0);
		Logger::instancia() << "Listo" << el;
	}
	
	delete this->listaHijos;
}

void Cliente::conectarAlServidor(){
    stringstream ss;
    char buffer[100];
    if(this->estaConectado) {
    	cout << "Ya se encuentra conectado al servidor" << endl;
    	return;
    }
    ss << "fifo" << getpid();
    this->fifoLectura = new Fifo(ss.str().c_str());
    this->escribirMensajeAlServidor(ALTA, "");
    this->recibirMensajeDelServidor(buffer,100);
    if(strcmp(buffer,ALTAOK) == 0) {
    	cout << "Conexión realizada con éxito" << endl;
        Logger::instancia() << "Conexión realizada con éxito." << el;
			this->estaConectado = true;
    }
    Logger::instancia() << "Recibido mensaje del servidor: " << buffer << el;
}

void Cliente::recibirMensajeDelServidor(char* buffer,int bufsize){
    int bytesLeidos = this->fifoLectura->leer(buffer, bufsize);
	buffer[bytesLeidos] = '\0';
    Logger::instancia() << "Recibido mensaje del servidor: " << buffer << el;
}


void Cliente::desconectar() {
	char buffer[100];
	if(this->estaConectado == true) {
		this->estaConectado = false;
		this->escribirMensajeAlServidor(BAJA, "");
		recibirMensajeDelServidor(buffer, 100);
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
    char buffer[100];
    int bytesLeidos = this->fifoLectura->leer(buffer,100);
    buffer[bytesLeidos] = 0;
    if(string(buffer).compare(COMPOK)) {
		cout << "Error al compartir el archivo." << endl;
		return -1;
	}
	cout << "Archivo compartido exitosamente." << endl;
    return 0;
}

int Cliente::dejarDeCompartirArchivo(string &pathArchivo){
    ParserComandos parser;
    char *aEnviar = parser.armarCompDescomp(DESCOMPARCH, getpid(), pathArchivo);
    // Incremento al semáforo para desbloquear al fifo del servidor
    this->semEscritura->v();
    this->fifoEscritura->escribir(aEnviar, parser.obtenerTamanioCompDescomp(pathArchivo));
    char buffer[100];
    int bytesLeidos = this->fifoLectura->leer(buffer,100);
    buffer[bytesLeidos] = 0;
    if(string(buffer).compare(DESCOMPOK)) {
		cout << "Error al dejar de compartir el archivo." << endl;
		return -1;
	}
	cout << "Archivo no compartido exitosamente." << endl;
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
		if(bufDef != buffer) {
			
			delete[] bufDef;}
	
		return mapa;
}
string Cliente::crearNombreDestino(string &sharePath, string &destPath) {
	char *copiaNombre = new char[sharePath.size() + 1];
	memcpy((void *) copiaNombre, (void *) sharePath.c_str(), sharePath.size());
	copiaNombre[sharePath.size()] = 0;
	string nombreBaseArchivo(basename(copiaNombre));
	string nombreCompletoDest(destPath + "/" + nombreBaseArchivo);
	delete [] copiaNombre;
	return nombreCompletoDest;
}

int Cliente::validarArchivoCompartido(TPID pid, string &pathOrigen, string &pathDestino) {
	if(pid == getpid()) {
		cout << "Error: el pid es el del proceso actual." << endl;
		return -4;
	}
	
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
		if ( pathOrigen.compare(*itL) == 0 ) 
			break;
	}
	
	if ( itL == itM->second->end()) {
		// El archivo compartido no existe...
		cout << "El cliente con el pid: " << pid << " no está compartiendo el archivo " << pathOrigen << endl;
		return -2;
	}
	
	return 0;
}

int Cliente::empezarTransferencia(string destPath, string sharePath, TPID pid) {
	
	int res;
	if((res = validarArchivoCompartido(pid, sharePath, destPath)))
		return res;
	
	string nombreCompletoDest = crearNombreDestino(sharePath, destPath);
	
	TPID pidProceso = getpid();
	ParserComandos parser;
	char *comando = parser.armarSolicitarTransf(PEDIRARCH, pidProceso, pid, sharePath, nombreCompletoDest);
	
	// Incremento al semáforo para desbloquear al fifo del servidor
    this->semEscritura->v();
	this->fifoEscritura->escribir(comando, parser.obtenerTamanioSolicitarTransf(sharePath, nombreCompletoDest));
	char buffer[100];
	recibirMensajeDelServidor(buffer, 100);
	if(strcmp(buffer, TROK) != 0)
		cout << "Error en la transferencia del archivo " << sharePath << endl;
	else {
		if(existeArchivo(nombreCompletoDest)) {
			cout << "Error: el archivo destino " << nombreCompletoDest << " ya existe. No se realiza la transferencia." << endl;
			return -3;
		}
		crearHijoReceptor(sharePath, nombreCompletoDest, pid);
	}
	
    return 0;
}

int Cliente::crearHijoReceptor(string &pathOrigen, string &pathDestino, TPID pid) {
	TPID pidHijo = fork();	
	if(pidHijo == 0) { // es el hijo
		if(Logger::isOpen()){ //si se abrió el cliente en modo debug, las transferencias asociadas también se ejecutarán en modo debug 
                execl("./transf", "transf", "R", pathOrigen.c_str(), pathDestino.c_str(), "--debug", NULL);
            }
            else{
                execl("./transf", "transf", "R", pathOrigen.c_str(), pathDestino.c_str(), NULL);
            } 
		}
		else if(pidHijo > 0) {
			this->listaHijos->push_back(pidHijo);
            Logger::instancia()<<"Ejecutando proceso (PID "<<pidHijo<<") de recepción desde el proceso con PID "<<
            pid<<" hasta "<<getpid()<<" del archivo "<<pathOrigen<<" al archivo "<<pathDestino<<"."<<el;
            
        } else
			cout << "Error en la transferencia del archivo " << pathOrigen << endl;
	return 0;
}



bool Cliente::conectado() {
	return this->estaConectado;
}

list<TPID> *Cliente::getListaHijos() {
	return this->listaHijos;
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
    
    Logger::instancia() << "Enviado el mensaje: " << pid << tipo << mensaje << el;
    return 0;
}
