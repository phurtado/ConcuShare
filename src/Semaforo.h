#ifndef SEMAFORO_H_
#define SEMAFORO_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>

#include "logger.h"

class Semaforo {

private:
	int id;
	int valorInicial;
	int inicializar (int posSem, int valor);
	
public:
	/* construye una instancia con un grupo de un semaforo */
	Semaforo (const char* nombre, int valorInicial );
	
	/* construye una instancia con un grupo de cantidadSemaforos semaforos
	 * y se inicializan los semaforos con los valoresIniciales.
	 */
	Semaforo ( const char* nombre, int cantidadSemaforos, int *valoresIniciales );
	
	virtual ~Semaforo();
	int p (int numSem = 0); // decrementa
	int v (int numSem = 0); // incrementa
	int getValor(int numSem = 0);
	void eliminar ();
};

#endif /* SEMAFORO_H_ */
