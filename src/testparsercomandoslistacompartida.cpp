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
	/*
	Servidor *s = new Servidor();
	s->escucharComandos();
	
	delete s;
	*/
	
	ParserComandos parser;
	map<TPID, ListaPaths*> mapa;
	
	TPID pid1 = 5;
	ListaPaths path11;
	path11.push_back("rut1");
	path11.push_back("rut2");
	path11.push_back("rut3");
	
	mapa.insert(pair<TPID, ListaPaths*>(pid1, &path11));
	
	
	TPID pid2 = 8;
	ListaPaths path12;
	path12.push_back("ruta11");
	path12.push_back("ruta22");
	path12.push_back("ruta33");
	
	mapa.insert(pair<TPID, ListaPaths*>(pid2, &path12));
	
	cout << "Mapa Original: " << endl;
	mostrarElementos(&mapa);
	
		
	char *c = parser.serializarLista(mapa);
	
	parser.setBuffer(c);
	
	map<TPID, ListaPaths*> *mapaR = parser.obtenerListaCompartidos();
	
	cout << "Mapa Recuperado: " << endl;
	mostrarElementos(mapaR);
	
	
	
	return 0;
}
