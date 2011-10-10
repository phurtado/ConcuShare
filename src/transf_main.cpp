#include <iostream>

#include "common.h"
#include "transferencia.h"
#include "logger.h"
#include <sstream>
#include <cstring>
using namespace std;

void initLog(std::string logfile){
    Logger::setLogFile(logfile);
    Logger::open();
}

/* Argumentos (en orden):
 * 1 - E o R
 * 	E: envia archivo
 * 	R: Recibe el archivo
 * 2 - pathArchivo: path del archivo a enviar
 * 3 - pathDestino: path del archivo donde se recibiran los datos
 * 4 - --debug (opcional) activa el modo debug
 */

int main(int argc, char *argv[]) {
	//Inicializacion Logger
    if(argc == 5){
        if(strcmp(argv[4],"--debug")==0){
            stringstream ss;
            ss<<"transf"<<getpid()<<".log";
            initLog(ss.str());
        }
    }
	
    if(argc < 4) {
		cerr << "Error. Numero de argumentos invalido." << endl;
		return 0;
	}
	
	string pathOrigen(argv[2]), pathDestino(argv[3]), modo(argv[1]);
	
	int res = 0;
	if(! modo.compare("E")) {
		Transferencia transf(pathOrigen, pathDestino, ENVIAR);
		while((res = transf.enviar()) == 0)
			;
	}
	
	if(! modo.compare("R")) {
		Transferencia transf(pathOrigen, pathDestino, RECIBIR);
		while((res = transf.recibir()) == 0)
			;
	}
	
	return (res == 1) ? 0 : 1;
}
