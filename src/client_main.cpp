#include <iostream>

#include "common.h"
#include "fifo.h"

using namespace std;

int main() {
	cout << "lala main cliente" << endl;
	Fifo *fifo = new Fifo(NOMBREFIFOSERVIDOR);
	char linea[BUFSIZE];
	TCOM comando = ALTA;
	TPID pid = 111;
	
	for(size_t i = 0; i < sizeof(TCOM); i++)
		*(linea + i) = *((char*) &comando + i);
	for(size_t i = 0; i < sizeof(TPID); i++)
		*(linea + sizeof(TCOM) + i) = *((char*) &pid + i);
	
	cout << "Size: " << sizeof(TCOM) + sizeof(TPID) << endl;
	fifo->escribir(linea, sizeof(TCOM) + sizeof(TPID));
	
	
	return 0;
}
