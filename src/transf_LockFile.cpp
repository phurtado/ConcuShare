#include "LockFile.h"



#include <iostream>
using namespace std;

LockFile :: LockFile ( const char* nombre, TMODO modo ) {

	strcpy ( this->nombre,nombre );
	
	switch(modo) {
		case LECTURA:
			this->fd = open ( this->nombre,O_RDWR );
			this->tipoLock = F_RDLCK;
			cout << "FD: " << this->fd << " abierto como lectura." << endl;
			break;
		case ESCRITURA:
			this->fd = open ( this->nombre,O_CREAT|O_WRONLY,0777 );
			this->tipoLock = F_WRLCK;
			cout << "FD: " << this->fd << " abierto como escritura." << endl;
			break;
	}
	if(this->fd == -1)
	cout << "Error OPEN!!! " << endl;
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
	cout << "Leyendo del archivo: " << this->fd << " " << resultado << " bytes: " << buffer << endl;
	return resultado;
}

LockFile :: ~LockFile () {
	close ( this->fd );
}


off_t LockFile::obtenerTamanioArchivo() {
	off_t posActual = lseek(this->fd, 0, SEEK_CUR);
	off_t tamArchivo = lseek(this->fd, 0, SEEK_END);
	cout << "TamArchivo: lseek: " << tamArchivo << ", actual: " << posActual << endl;
	lseek(this->fd, posActual, SEEK_SET);
	
	return tamArchivo;
}

off_t LockFile::obtenerPosicion() {
	return lseek(this->fd, 0, SEEK_CUR);
}

bool LockFile::archivoLockeado() {
	return this->lockActivo;
}
