#include <iostream>
#include <fstream>
#include <string>

#include "common.h"
#include "fifo.h"
#include "Cliente.h"

using namespace std;

Cliente *cliente = NULL;

void handler_SIGINT(int sig) {
	if(cliente)
		delete cliente;
	cliente = NULL;
}

void handler_SIGTERM(int sig) {
	cout << "Terminé con SIGINT" << endl;
}


void registrarSignalTerm() {
	struct sigaction sa;
	sigemptyset(& sa.sa_mask);
	sa.sa_handler = handler_SIGINT;
	
	sigaction(SIGINT, &sa, NULL);
}

void registrarSignalTerm2() {
	struct sigaction sa;
	sigemptyset(& sa.sa_mask);
	sa.sa_handler = handler_SIGTERM;
	
	sigaction(SIGTERM, &sa, NULL);
}



void mostrarListaCompartidos(map<TPID, ListaPaths*> *mapaCompartidos) {
	map<TPID, ListaPaths*>::iterator itM = mapaCompartidos->begin();
	ListaPaths::iterator itL;
	cout << "Lista de compartidos con " << mapaCompartidos->size() << " clientes." << endl;
	for(; itM != mapaCompartidos->end(); itM++) {
		cout << "Cliente pid: " << itM->first << endl;
		for(itL = itM->second->begin(); itL != itM->second->end(); itL++)
			cout << "\tArchivo de " << itM->first << ": " << *itL << endl;
	}
}

int solicitarPath(string &path, string accion) {
	
	cout << "Ingrese path del archivo a " << accion << ": ";
	cin >> path;
	fstream *arch = new fstream(path.c_str(), fstream::in);
	if(arch->fail()) {
		cout << "El archivo no existe" << endl;
		return 1;
	}
	arch->close();
	delete arch;
	return 0;
}
	

int main() {
	int opcion = 1;
	char buffer[1000];
	string path;
	cliente = new Cliente();
	map<TPID, ListaPaths*> *mapaCompartidos = NULL;
	registrarSignalTerm();
	bool continua = true;
	while(continua) {
		// Imprimo el menú del programa
		FILE* fd = fopen("MenuCliente.txt", "r");
		do {
			fscanf(fd,"%[^\n]\n", buffer);
			cout << buffer << endl;
		} while(!feof(fd));
		fclose(fd);
		
		cout << endl << "Número de opción: ";
		scanf("%s", buffer);
		opcion = atoi(buffer);

		switch(opcion) {
		case 1:
			cliente->conectarAlServidor();
			break;
		case 2:
			cliente->desconectar();
			break;
		case 3:
			// OJO QUE PUEDEN SER VARIOS A LA VEZ
			
			if(solicitarPath(path, "compartir"))
				break;
			
			cliente->compartirArchivo(path);
			break;
		case 4:
			// IDEM CASO 3
			if(solicitarPath(path, "dejar de compartir"))
				break;
			
			cliente->dejarDeCompartirArchivo(path);
			break;
		case 5:
			break;
		case 6:
			if(mapaCompartidos) delete mapaCompartidos;
			mapaCompartidos = cliente->getCompartidos();
			mostrarListaCompartidos(mapaCompartidos);
			break;
		case 7:
			if(cliente)
				delete cliente;
			continua = false;
			break;
		default:
			cout << "Ingrese una opción válida" << endl;
			}
	}
	
	return 0;
}
