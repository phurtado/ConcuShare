#ifndef BUFFMEM_H_
#define BUFFMEM_H_

#include <stdlib.h>
#include <string.h>

/* tamano del buffer */

#define BUFMEMSIZE 20

class BufferMem {
	private:
		char buffer[BUFMEMSIZE];
		off_t tamanoArchivo, bytesEscritos;
		bool esElFin;
		
	public:
		/* inicializa el buffer */
		BufferMem();
		
		/* metodo para obtener el buffer
		 * retorno: el puntero al buffer de BUFMEMSIZE caracteres
		 */
		char *getBuffer();
		
		void setTamArchivo(off_t tam);

		off_t getTamArchivo();
		
		void setTamEscrito(off_t tam);
	
		off_t getTamEscrito();
		
		bool esFin();

		void setFin(bool fin);
		
		void setDatos(const char dato[BUFMEMSIZE]) {
			memcpy(this->buffer, dato, BUFMEMSIZE);
		}
		
		/* destructor del buffer */
		virtual ~BufferMem();

};


#endif /* BUFFMEM_H_ */
