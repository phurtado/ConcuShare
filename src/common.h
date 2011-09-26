#ifndef COMMON_H
#define COMMON_H
#include <list>
#include <unistd.h>
#include <sys/types.h>
<<<<<<< HEAD
#include <string>
=======
#include <iostream>
>>>>>>> b91d5028aa7945c40b2f844de484614846c68017

#define BUFSIZE 100
#define NOMBREFIFOSERVIDOR "fifo_lectura"

typedef enum {ALTA=561,BAJA=562,LISTACOMP=563,
						COMPARCH=564,DESCOMPARCH=565} TCOM;
typedef std::list<std::string> ListaPaths;
typedef pid_t TPID;

#endif
