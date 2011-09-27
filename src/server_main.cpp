#include <iostream>
#include "servidor.h"

#include "parsercomandos.h"

using namespace std;

void mostrarElementos(map<TPID, ListaPaths* > *mapa) {
	map<TPID, ListaPaths*>::iterator itM = mapa->begin();
	ListaPaths::iterator itL;
	for(; itM != mapa->end(); itM++) {
		cout << "Elemento mapa: " << itM->first << endl;
		for(itL = itM->second->begin(); itL != itM->second->end(); itL++) {
			cout << "PID: " << itM->first << ", Ruta: " << *itL << endl;
		}
	}
}

int main() {
	cout << "lala main server" << endl;
	
	Servidor *s = new Servidor();
	s->escucharComandos();
	
	delete s;
	
	return 0;
}
