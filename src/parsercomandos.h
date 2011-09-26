#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <string.h>

#include "common.h"

class ParserComandos {
    private:
        char *buffer;
        
        size_t obtenerTamanioLista(std::map<TPID, ListaPaths*> &mapa);
        char* serializarListaCliente(ListaPaths &lista, char * buffer);
    
    public:
		/* inicializan el parser */
		ParserComandos();
		
		ParserComandos(char *buffer);
		
		// Todos los comandos
		/* Los siguientes son metodos validos para todos los comandos.
		 */
		
		/* metodo para setear el buffer con el cual se trabajara
		 * buffer: puntero al buffer
		 */
		void setBuffer(char *buffer);
		
		/* metodo para obtener el comando del buffer.
		 * Debe haber un buffer valido seteado previamente.
		 */
		TCOM getComando();
		
		/* metodo para obtener el PID del buffer.
		 * Debe haber un buffer valido seteado previamente.
		 */
		TPID getPid();
		
		// Comandos Compartir y descompartir archivo
		
		/* Los siguientes son metodos validos para dichos comandos.
		 */
		
		/* metodo para obtener el tamano del string path.
		 */
		std::size_t getTamanoStringPath();
		
		/* metodo para obtener el path del archivo a compartir/descompartir
		 */
		char *getPath();
		
		// Comando Pedir Lista de archivos
		
		/* metodo para obtener la lista de archivos compartidos del buffer
		 */
		std::map<TPID, ListaPaths*> *obtenerListaCompartidos();
		
		/* metodo para serializar la lista de compartidos.
		 * mapaCompartidos: mapa de archivos compartidos
		 * Retorna: un buffer con la lista serializada.
		 */
		char *serializarLista(std::map<TPID, ListaPaths*> &mapaCompartidos);
};


#endif
