#ifndef PARSER_H
#define PARSER_H

#include <map>
#include <string.h>
#include <stdlib.h>

#include "common.h"

class ParserComandos {
    private:
        char *buffer;
        char *serializarListaCliente(ListaPaths &lista, char * buffer);
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
		
		/* metodo para obtener el path del archivo a compartir/descompartir o
		 * el path solicitado en un comando de solicitud de archivo
		 * longitud: longitud del path
		 */
		std::string getPath();
		
			
		// Comando solicitar archivo compartido
		
		/* Los siguientes metodos son solo para el comando mencionado */
		
		/* metodo para obtener el tamanio del path de destino de un comando
		 * de solicitud de transferencia de archivo
		 * retorna: el tamanio mencionado
		 */
		std::size_t getTamanioStringPathDestino();
		
		/* metodo que obtiene el path destino de la solicitud de 
		 * transferencia de un archivo.
		 * retorna: path destino.
		 */
		std::string getPathDestino();
		
		/* metodo para recuperar el pid del cliente que esta compartiendo
		 * el archivo.
		 * retorna: pid del cliente duenio del archivo.
		 */
		TPID getPidClienteDuenioArchivo();
		
		/* metodo que obtiene el path del archivo pedido
		 * Retorna: string con path.
		 */
		std::string getPathArchivoSolicitado();
		
		/* metodo para armar el comando para compartir/descompartir un archivo
		 * comando: comando a usar (COMPARCH o DESCOMPARCH)
		 * pid: pid del cliente
		 * path: path del archivo a compartir/descompartir
		 * Retorna: un char* apuntando al comando serializado.
		 */
		char* armarCompDescomp(TCOM comando, TPID pid, std::string &path);
		
		/*metodo para armar el comando de solicitar transferencia de archivo
		 * comando: idem armarCompDescomp
		 * pidSolicitante: pid del cliente que solicita el archivo
		 * pidDuenio: pid del cliente dueño del archivo
		 * pathArchivo: path del archivo
		 * pathDestino: path del archivo destino
		 * Retorna: un char* apuntando al comando serializado.
		 */
		char *armarSolicitarTransf(TCOM comando, TPID pidSolicitante, 
		TPID pidDuenio, std::string &pathArchivo, std::string &pathDestino);
				
		/* metodo para obtener el tamaño en bytes del comando para 
		 * compartir/descompartir un archivo.
		 * path: path del archivo a compartir/descompartir
		 * Retorna: el tamaño del comando.
		 */
		size_t obtenerTamanioCompDescomp(const std::string &path);
		
		/* metodo para obtener el tamaño en bytes del comando para 
		 * solicitar transferencia de un archivo.
		 * pathArchivo: path del archivo a pedido
		 * pathDestino: path del archivo destino
		 * Retorna: el tamaño del comando.
		 */
		size_t obtenerTamanioSolicitarTransf(const std::string &pathArchivo, 
										const std::string &pathDestino);
		
		
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
