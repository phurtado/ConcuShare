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
	cout << "lala main transf" << endl;
	if(argc != 4) {
		cerr << "Error. Numero de argumentos invalido." << endl;
		return 0;
	}
		
	for(int i = 0; i < argc; i++)
		cout << "Arg " << i << " es " << argv[i] << endl;
	
	string pathOrigen(argv[2]), pathDestino(argv[3]), modo(argv[1]);
	Transferencia transf(pathOrigen, pathDestino);
	
	if(! modo.compare("E")) {
		cout << "VOY A ENVIAR" << endl;
		int res;
		while((res = transf.enviar()) == 0)
			cout << "Res = " << res << endl;
	}
	
	if(! modo.compare("R")) {
		cout << "VOY A RECIBIR" << endl;
		int res;
		while((res = transf.recibir()) == 0)
			cout << "Res = " << res << endl;
	}
	
	return 0;
}
