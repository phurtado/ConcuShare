#ifndef COMMON_H
#define COMMON_H
#include <list>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>

#define BUFSIZE 24 ///< Tamaño de buffers. BUFSIZE MINIMO = sizeof(TCOM) + sizeof(TPID) + 2 * sizeof(size_t)

#define NOMBREFIFOSERVIDOR "fifo_lectura"

typedef enum {ALTA=561,BAJA=562,LISTACOMP=563,
						COMPARCH=564,DESCOMPARCH=565,PEDIRARCH=566} TCOM; ///< Enumerado de tipos de mensajes para enviar por los fifos.
typedef std::list<std::string> ListaPaths; ///< Lista de paths de archivos compartidos.
typedef pid_t TPID;

// Respuestas del servidor al cliente
#define ALTAOK "0\0"
#define ALTAWRONG "-1\0"
#define BAJAOK "0\0"
#define BAJAWRONG "-1\0"
#define COMPOK "0\0"
#define COMPERROR "-1\0"
#define DESCOMPOK "0\0"
#define DESCOMPERROR "-1\0"
#define TROK "0\0"
#define TRERROR "-1\0"

void buscarHijosQueTerminaron(std::list< TPID > *lista);


#endif
