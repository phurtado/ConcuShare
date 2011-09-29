#include <iostream>
#include <signal.h>

#include "servidor.h"

#include "parsercomandos.h"

using namespace std;

Servidor *s = NULL;

void handler_SIGINT(int sig) {
	s->setContinua(false);
	delete s;
}


void registrarSignalTerm() {
	struct sigaction sa;
	sigemptyset(& sa.sa_mask);
	sigaddset(& sa.sa_mask, SIGINT);
	sa.sa_handler = handler_SIGINT;
	
	sigaction(SIGINT, &sa, NULL);
}

int main() {
	cout << "lala main server" << endl;
	
	s = new Servidor();
	
	registrarSignalTerm();
	
	s->escucharComandos();
	
	return 0;
}
