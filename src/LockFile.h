#ifndef LOCKFILE_H_
#define LOCKFILE_H_

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

typedef enum {LECTURA, ESCRITURA} TMODO;


class LockFile {

private:

	struct flock fl;
	int fd;
	short tipoLock;
	char nombre [ 255 ];
	bool lockActivo;
	
public:

	LockFile ( const char* nombre, TMODO modo );
	virtual ~LockFile();
	
	int tomarLock ();
	int liberarLock ();
	int escribir ( char* buffer,int buffsize );
	int leer ( char* buffer,int buffsize );
	
	bool archivoLockeado();
	
	off_t obtenerTamanioArchivo();
	off_t obtenerPosicion();
};

#endif /* LOCKFILE_H_ */
