#include <iostream>
#include <signal.h>
#include "servidor.h"
#include "parsercomandos.h"
#include "logger.h"
#include <sstream>

using namespace std;

Servidor *s = NULL;

void handler_SIGINT(int sig) {
	if(s!=NULL){
        s->setContinua(false);
        delete s;
	    s = NULL;
    }
}


void registrarSignalInt() {
	struct sigaction sa;
	sigemptyset(& sa.sa_mask);
	sa.sa_handler = handler_SIGINT;
	
	sigaction(SIGINT, &sa, NULL);
}

void initLog(std::string logfile){
    Logger::setLogFile(logfile);
    Logger::open();
}

int main(int argc,char** argv) {
	cout << "Servidor escuchando peticiones de clientes..." << endl;

    //Inicializacion Logger
    if(argc>1){
        if(strcmp(argv[1],"--debug")==0){
            stringstream ss;
            ss<<"server"<<getpid()<<".log";
            initLog(ss.str());
        }
    }
    Logger::instancia() << "Iniciando Servidor." << el;
	s = new Servidor();
	registrarSignalInt();
	s->escucharComandos();
	
	Logger::liberar();
	
	if(s)
		delete s;
	
    Logger::close();
	return 0;
}
