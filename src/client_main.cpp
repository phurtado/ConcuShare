#include <iostream>
#include <fstream>
#include <string>

#include "common.h"
#include "fifo.h"
#include "Cliente.h"

using namespace std;

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
	Cliente cliente;
	map<TPID, ListaPaths*> *mapaCompartidos = NULL;
	

	while(1) {
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
			cliente.conectarAlServidor();
			break;
		case 2:
			cliente.desconectar();
			break;
		case 3:
			// OJO QUE PUEDEN SER VARIOS A LA VEZ
			
			if(solicitarPath(path, "compartir"))
				break;
			
			cliente.compartirArchivo(path);
			break;
		case 4:
			// IDEM CASO 3
			if(solicitarPath(path, "dejar de compartir"))
				break;
			
			cliente.dejarDeCompartirArchivo(path);
			break;
		case 5:
			break;
		case 6:
			if(mapaCompartidos) delete mapaCompartidos;
			mapaCompartidos = cliente.getCompartidos();
			mostrarListaCompartidos(mapaCompartidos);
			break;
		case 7:
			return 0;
		default:
			cout << "Ingrese una opción válida" << endl;
			}
	}


	/*cout << "lala main cliente" << endl;
	Fifo *fifo = new Fifo(NOMBREFIFOSERVIDOR);
	Semaforo semaforo((char *) NOMBREFIFOSERVIDOR, 0);
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
	// Incremento el semáforo al escribir
	semaforo.v();	
	fifo->escribir(linea, pos - linea);
	fifo->cerrar();
	cout << "Terminé de escribir" << endl;*/
	
	return 0;
}
