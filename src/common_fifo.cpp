#include "fifo.h"

Fifo :: Fifo (const char *nombre) {
        strcpy ( this->nombre, nombre);
        // se crea el fifo
        mknod (nombre, S_IFIFO | 0770 ,0 );
        // se inicializa el descriptor en -1
        this->fileDes = -1;
}
Fifo :: ~Fifo () {
}

int Fifo :: escribir ( char *dato , int datoSize ) {
        // se abre el file descriptor para escritura
        if (this->fileDes == -1)
                   this->fileDes = open (this->nombre , O_WRONLY);
        // se escriben los datos en el fifo
        if(this->fileDes > 0)
			return write (this->fileDes ,(const void*) dato, datoSize);
		else
			return 0;
}

int Fifo :: leer (char *buffer, int buffSize ) {
        // se abre el file descriptor para lectura
        if (this->fileDes == -1 )
                   this->fileDes = open (this->nombre , O_RDONLY);
        // se leen los datos del fifo
        if(this->fileDes > 0)	
			return read(this->fileDes ,(void*) buffer , buffSize);
		else 
			return 0;
}

void Fifo :: cerrar () {
        if(this->fileDes != -1)
			close(this->fileDes);
		
		unlink(this->nombre);
		this->fileDes = -2;
}
