#include <iostream>

#include "common.h"
#include "transferencia.h"

using namespace std;

/* Argumentos:
 * 1 - E o R
 * 	E: envia archivo
 * 	R: Recibe el archivo
 * 2 - pathArchivo: path del archivo a enviar
 * 3 - pathDestino: path del archivo donde se recibiran los datos
 */

int main(int argc, char *argv[]) {
	
	if(argc != 4) {
		cerr << "Error. Numero de argumentos invalido." << endl;
		return 0;
	}
	
	string pathOrigen(argv[2]), pathDestino(argv[3]), modo(argv[1]);
	Transferencia transf(pathOrigen, pathDestino);
	int res = 0;
	
	if(! modo.compare("E")) {
		while((res = transf.enviar()) == 0);
	}
	
	if(! modo.compare("R")) {
		while((res = transf.recibir()) == 0);
	}
	
	return (res == 1) ? 0 : 1;
}
