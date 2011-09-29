#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <string.h>
#include <stdlib.h>

#include "common.h"

class ParserComandos {
    private:
        char *buffer;
        
        
        char* serializarListaCliente(ListaPaths &lista, char * buffer);
        char *hidratarLista(char *posicionActual, ListaPaths *lista);
        
    
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
		
		// Comandos Compartir y descompartir archivo y solicitar
		// transferencia de archvios
		
		/* Los siguientes son metodos validos para dichos comandos.
		 */
		
		/* metodo para obtener el tamano del string path.
		 */
		std::size_t getTamanoStringPath();
		
		/* metodo para obtener el path del archivo a compartir/descompartir
		 * longitud: longitud del path
		 */
		std::string getPath();
		
		
		/* metodo que obtiene el pid destino de la solicitud de 
		 * transferencia de un archivo.
		 * retorna: PID destino.
		 */
		TPID getPidDestino();
		
		// Comando Pedir Lista de archivos
		
		/* metodo para obtener la lista de archivos compartidos del buffer
		 */
		std::map<TPID, ListaPaths*> *obtenerListaCompartidos();
		
		/* metodo para serializar la lista de compartidos.
		 * mapaCompartidos: mapa de archivos compartidos
		 * tam: tamanio de la lista previamente calculado con el metodo 
		 * obtenerTamanioLista. Por defecto 0, no se pasa el tamanio.
		 * Retorna: un buffer con la lista serializada.
		 */
		char *serializarLista(std::map<TPID, ListaPaths*> &mapaCompartidos, std::size_t tam = 0);
		
		/* metodo para obtener el tamanio del array de chars que contendra la
		 * lista de archivos compartidos serializada
		 * mapa: mapa con los paths de archivos compartidos.
		 * retorna: el tamanio en chars.
		 */
		size_t obtenerTamanioLista(std::map<TPID, ListaPaths*> &mapa);
		
		/* metodo para obtener el tamanio total del comando actual 
		 * del buffer.
		 */
		 size_t obtenerTamanioComando();
};


#endif
