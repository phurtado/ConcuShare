#include <iostream>

#include "common.h"
#include "fifo.h"
#include "Cliente.h"

using namespace std;

int main() {
	int opcion = 1;
	char buffer[1000];
	Cliente cliente;

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
			//cliente.compartirArchivo();
			break;
		case 4:
			// IDEM CASO 3
			//cliente.dejarDeCompartirArchivo();
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			return 0;
			break;
		default:
			cout << "Ingrese una opción válida" << endl;
			break;
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
