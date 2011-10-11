#include "LockFile.h"

LockFile :: LockFile ( const char* nombre, TMODO modo ) {

	strcpy ( this->nombre,nombre );
	
	switch(modo) {
		case LECTURA:
			this->fd = open ( this->nombre,O_RDONLY ); 
			this->tipoLock = F_RDLCK;
			break;
		case ESCRITURA:
			this->fd = open ( this->nombre,O_CREAT|O_WRONLY,0777 );
			this->tipoLock = F_WRLCK;
			break;
	}
	if(this->fd == -1)
	this->fl.l_whence = SEEK_SET;
	this->fl.l_start = 0;
	this->fl.l_len = obtenerTamanioArchivo();
	this->fl.l_pid = getpid ();
	
	this->lockActivo = false;
}

int LockFile :: tomarLock () {
	this->fl.l_type = this->tipoLock;
	int resultado = fcntl ( this->fd,F_SETLKW,&(this->fl) );
	this->lockActivo = true;
	return resultado;
}


int LockFile :: liberarLock () {
	this->fl.l_type = F_UNLCK;
	int resultado = fcntl ( this->fd,F_SETLK,&(this->fl) );
	this->lockActivo = false;
	return resultado;
}

int LockFile :: escribir ( char* buffer,int buffsize ) {
	lseek ( this->fd,0,SEEK_END );
	int resultado = write ( this->fd,buffer,buffsize );
	return resultado;
}

int LockFile :: leer ( char* buffer, int buffsize ) {
	int resultado = read ( this->fd,buffer,buffsize );
	return resultado;
}

LockFile :: ~LockFile () {
	close ( this->fd );
}


off_t LockFile::obtenerTamanioArchivo() {
	off_t posActual = lseek(this->fd, 0, SEEK_CUR);
	off_t tamArchivo = lseek(this->fd, 0, SEEK_END);
	lseek(this->fd, posActual, SEEK_SET);
	
	return tamArchivo;
}

off_t LockFile::obtenerPosicion() {
	return lseek(this->fd, 0, SEEK_CUR);
}

bool LockFile::archivoLockeado() {
	return this->lockActivo;
}
