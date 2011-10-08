#include <iostream>
#include <signal.h>
#include "servidor.h"
#include "parsercomandos.h"

using namespace std;

Servidor *s = NULL;

void handler_SIGINT(int sig) {
	s->setContinua(false);
	delete s;
	s = NULL;
}

void registrarSignalTerm() {
	struct sigaction sa;
	sigemptyset(& sa.sa_mask);
	sa.sa_handler = handler_SIGINT;
	
	sigaction(SIGINT, &sa, NULL);
}

int main() {
	cout << "Servidor escuchando peticiones de clientes..." << endl;
	s = new Servidor();
	registrarSignalTerm();
	s->escucharComandos();
	if(s)
		delete s;
	
	return 0;
}
