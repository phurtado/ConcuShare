#ifndef BUFFMEM_H_
#define BUFFMEM_H_

#include <stdlib.h>
#include <fstream>

/* tamano del buffer */
#define BUFMEMSIZE 20

class BufferMem {
	private:
		char buffer[BUFMEMSIZE];
		std::streampos tamanoArchivo, bytesEscritos;
		bool esElFin;
		
	public:
		/* inicializa el buffer */
		BufferMem();
		
		/* metodo para obtener el buffer
		 * retorno: el puntero al buffer de BUFMEMSIZE caracteres
		 */
		char *getBuffer();
		
		void setTamArchivo(std::streampos tam);

		std::streampos getTamArchivo();
		
		void setTamEscrito(std::streampos tam);
	
		std::streampos getTamEscrito();
		
		bool esFin();

		void setFin(bool fin);
		
		/* destructor del buffer */
		virtual ~BufferMem();

};


#endif /* BUFFMEM_H_ */
