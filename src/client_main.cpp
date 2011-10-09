#include <iostream>
#include <fstream>
#include <string>
#include "common.h"
#include "fifo.h"
#include "Cliente.h"
#include <sys/stat.h>


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
		delete arch;
		return 1;
	}
	arch->close();
	delete arch;
	return 0;
}

void descargarArchivo() {
	int pid;
	string destPath;
	string sharePath;
	char buffer[10];

	// Por el momento asumo que lo que me ingresan es un número, dsp valido
	cout << "Ingrese el pid del cliente del cual quiere descargar el archivo" << endl;
	fscanf(stdin, "%s", buffer);
	pid = atoi(buffer);
	if (pid == 0) {
		cout << "El pid ingresado no es válido" << endl;
		return;
	}

	// Verifico si la ruta de destino es un directorio válido.
	cout << "Ingrese el path de destino" << endl;
	struct stat buf;
	cin >> destPath;
	if (lstat(destPath.c_str(), &buf) == -1) {
		cout << "El path de destino ingresado no es válido" << endl;
		return;
	}
	if(! S_ISDIR(buf.st_mode)) {
		cout << "El path de destino debe ser un directorio" << endl;
		return;
	}

	// Verifico si la ruta del archivo compartido es válida.
	cout << "Ingrese el path del archivo compartido" << endl;
	cin >> sharePath;
	if(lstat(sharePath.c_str(), &buf) == -1) {
		cout << "El path del archivo compartido no es válido" << endl;
		return;
	}
	if(! S_ISREG(buf.st_mode)) {
		cout << "El path del archiv compartido debe ser un archivo regular" << endl;
		return;
	}

	// Si es válida, procedo a pasarle los datos al cliente
	cliente->empezarTransferencia(destPath, sharePath, pid);
}

void errorClienteNoConectado() {
	cout << "Debe conectarse al servidor para realizar alguna tarea" << endl;
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
			if (cliente->conectado() == false) {
				errorClienteNoConectado();
				break;	
			}	
			if(solicitarPath(path, "compartir")) {
				cout << "El path del archivo a compartir no existe" << endl;
				break;
			}
			cliente->compartirArchivo(path);
			break;

		case 4:
			// IDEM CASO 3
			if (cliente->conectado() == false) {
				errorClienteNoConectado();
				break;	
			}
			if(solicitarPath(path, "dejar de compartir")) {
				cout << "El path del archivo a descompartir no existe" << endl;
				break;
			}
			cliente->dejarDeCompartirArchivo(path);
			break;

		case 5:
			if (cliente->conectado() == false) {
				errorClienteNoConectado();
				break;
			}
			descargarArchivo();
			break;

		case 6:
			if (cliente->conectado() == false) {
				errorClienteNoConectado();
				break;
			}
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
			break;
			}
	}
	
	return 0;
}
