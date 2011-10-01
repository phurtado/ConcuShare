#include <iostream>

#include "common.h"
#include "fifo.h"

using namespace std;

int main() {
	cout << "lala main cliente" << endl;
	Fifo *fifo = new Fifo(NOMBREFIFOSERVIDOR);
	char linea[2 * BUFSIZE];
	TCOM comando = PEDIRARCH;
	TPID pidO = 111, pidD = 112;
	size_t tamP = 9, tamD = 8;
	string path = "lala45679";
	string pathD = "titi0123";
	char *pos = linea;
	
	memcpy((void *) pos, (void *) &comando, sizeof(TCOM));
	pos += sizeof(TCOM);
	memcpy((void *) pos, (void *) &pidO, sizeof(TPID));
	pos += sizeof(TPID);
	memcpy((void *) pos, (void *) &tamP, sizeof(size_t));
	pos += sizeof(size_t);
	memcpy((void *) pos, (void *) &tamD, sizeof(size_t));
	pos += sizeof(size_t);
	memcpy((void *) pos, (void *) &pidD, sizeof(TPID));
	pos += sizeof(TPID);
	memcpy((void *) pos, (void *) path.c_str(), path.size());
	pos += path.size();
	memcpy((void *) pos, (void *) pathD.c_str(), pathD.size());
	pos += pathD.size();
	
	
	cout << "Tam Comando: " << pos - linea << endl;
	
	fifo->escribir(linea, pos - linea);
	
	//sleep(10);
	
	fifo->cerrar();
	
	return 0;
}
