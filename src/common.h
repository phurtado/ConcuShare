#ifndef COMMON_H
#define COMMON_H
#include <list>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>

#include <string>
#include <iostream>

#define BUFSIZE 22
/* BUFSIZE MINIMO = sizeof(TCOM) + sizeof(TPID) + sizeof(size_t) */

#define NOMBREFIFOSERVIDOR "fifo_lectura"

typedef enum {ALTA=561,BAJA=562,LISTACOMP=563,
						COMPARCH=564,DESCOMPARCH=565,PEDIRARCH=566} TCOM;
typedef std::list<std::string> ListaPaths;
typedef pid_t TPID;

#endif
