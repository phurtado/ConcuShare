#include <iostream>
#include "servidor.h"

using namespace std;

int main() {
	cout << "lala main server" << endl;
	Servidor *s = new Servidor();
	s->escucharComandos();
	
	delete s;
	
	return 0;
}
