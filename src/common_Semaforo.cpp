#include "Semaforo.h"

Semaforo::Semaforo (const char* nombre,int valorInicial ) {
	key_t clave = ftok ( nombre,'a' );
	this->id = semget ( clave, 1, 00770 | IPC_CREAT | IPC_EXCL);
	
	if((this->id == -1) && (errno == EEXIST)) { //ya existe el conjunto, no inicializo
		this->id = semget ( clave, 1, 00770 | IPC_CREAT);
		return;
	}
	
	inicializar(0, valorInicial);
}


Semaforo::Semaforo ( const char* nombre, int cantidadSemaforos, int *valoresIniciales ) {
	key_t clave = ftok ( nombre,'a' );
	this->id = semget ( clave, cantidadSemaforos, 00770 | IPC_CREAT | IPC_EXCL);
	
	if((this->id == -1) && (errno == EEXIST))  { //ya existe el conjunto, no inicializo
		this->id = semget ( clave, cantidadSemaforos, 00770 | IPC_CREAT);
		return;
	}
	
	for(int i = 0; i < cantidadSemaforos; i++)
		inicializar(i, valoresIniciales[i]);
}

Semaforo::~Semaforo() {
}

int Semaforo::inicializar (int posSem, int valor) {
	union semnum {
		int val;
		struct semid_ds* buf;
		ushort* array;
	};

	semnum init;
	init.val = valor;
	int resultado = semctl ( this->id, posSem, SETVAL, init );
	return resultado;
}

int Semaforo::p (int numSem) {
	struct sembuf operacion;
	operacion.sem_num = numSem;	// numero de semaforo
	operacion.sem_op  = -1;	// restar 1 al semaforo
	operacion.sem_flg = SEM_UNDO;
	int resultado = semop ( this->id,&operacion,1 );
	return resultado;
}

int Semaforo::v (int numSem) {
	struct sembuf operacion;
	operacion.sem_num = numSem;	// numero de semaforo
	operacion.sem_op  = 1;	// sumar 1 al semaforo
	operacion.sem_flg = SEM_UNDO;
	int resultado = semop ( this->id,&operacion,1 );
	return resultado;
}

void Semaforo :: eliminar () {
	// elimina el conjunto, semnum es ignorado
	semctl ( this->id,0,IPC_RMID );
}
