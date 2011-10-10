#include "common.h"

#include <iostream>
using namespace std;

void buscarHijosQueTerminaron(list< TPID > *lista) {
	TPID pid;
	int estado;
	while(((pid = waitpid(-1, &estado, WNOHANG)) != 0) && (pid != -1)) {
		cout << "Hijo: " << pid << " terminado." << endl;
		lista->remove(pid);
	}
}
