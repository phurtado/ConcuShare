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
    
		/**
         * Inicializa el parser sin setear el buffer
         */
		ParserComandos();
		
        /**
         * Inicializa el parser con un buffer inicializado
         * @param buffer Un buffer inicializado.
         */
		ParserComandos(char *buffer);
		
		// Todos los comandos
		/* Los siguientes son metodos validos para todos los comandos.
		 */
		
		/**
         * Setea el buffer con el cual se trabajará.
		 * @param buffer Puntero al buffer
		 */
		void setBuffer(char *buffer);
		
		/**
         * Obtiene el comando del buffer.
		 * @pre Debe haber un buffer válido seteado previamente.
         * @return El comando almacenado en el buffer.
		 */
		TCOM getComando();
		
		/**
         * Obtiene el PID del buffer.
		 * @pre Debe haber un buffer valido seteado previamente.
         * @return El PID almacenado en el buffer.
		 */
		TPID getPid();
		
		// Comandos Compartir y descompartir archivo y solicitar
		// transferencia de archvios
		
		/* Los siguientes son metodos validos para dichos comandos.
		 */
		
		/**
         * @pre Válido para comandos Compartir y descompartir archivo y solicitar
         * @return Tamaño del string path.
		 */
		std::size_t getTamanoStringPath();
		
		/**
         * @pre Válido para comandos Compartir y descompartir archivo y solicitar
         * Obtiene el path del archivo a compartir/descompartir o
		 * el path solicitado en un comando de solicitud de archivo
		 * @param longitud Longitud del path
		 */
		std::string getPath();
		
			
		// Comando solicitar archivo compartido
		
		/* Los siguientes metodos son solo para el comando mencionado */
		
		/**
         * @pre Válido para comando solicitar
         * Obtiene el tamaño del path de destino de un comando
		 * de solicitud de transferencia de archivo.
		 * @return El tamaño mencionado.
		 */
		std::size_t getTamanioStringPathDestino();
		
		/** 
         * @pre Válido para comando solicitar
         * Obtiene el path destino de la solicitud de 
		 * transferencia de un archivo.
		 * @return Path destino.
		 */
		std::string getPathDestino();
		
		/** 
         * @pre Válido para comando solicitar
         * Recupera el pid del cliente que esta compartiendo
		 * el archivo.
		 * @return pid del cliente duenio del archivo.
		 */
		TPID getPidClienteDuenioArchivo();
		
		/**
         * @pre Válido para comando solicitar
         * Obtiene el path del archivo pedido
		 * @return String con path.
		 */
		std::string getPathArchivoSolicitado();
		
		/**  
         * @pre Válido para comando solicitar
         * Arma el comando para compartir/descompartir un archivo
		 * @param comando Comando a usar (COMPARCH o DESCOMPARCH)
		 * @param pid Pid del cliente
		 * @param path Path del archivo a compartir/descompartir
		 * @return Un char* apuntando al comando serializado.
		 */
		char* armarCompDescomp(TCOM comando, TPID pid, std::string &path);
		
		/**
         * @pre Válido para comando solicitar
         * Arma el comando de solicitar transferencia de archivo
		 * @param comando Comando a usar (COMPARCH o DESCOMPARCH)
		 * @param pidSolicitante pid del cliente que solicita el archivo
		 * @param pidDuenio pid del cliente dueño del archivo
		 * @param pathArchivo path del archivo
		 * @param pathDestino path del archivo destino
		 * @return un char* apuntando al comando serializado.
		 */
		char *armarSolicitarTransf(TCOM comando, TPID pidSolicitante, 
		TPID pidDuenio, std::string &pathArchivo, std::string &pathDestino);
				
		/**
         * @pre Válido para comando solicitar
         * Obtiene el tamaño en bytes del comando para 
		 * compartir/descompartir un archivo.
		 * @param path path del archivo a compartir/descompartir
		 * @return El tamaño del comando.
		 */
		size_t obtenerTamanioCompDescomp(const std::string &path);
		
		/**
         * @pre Válido para comando solicitar
         * Obtiene el tamaño en bytes del comando para 
		 * solicitar transferencia de un archivo.
		 * @param pathArchivo path del archivo a pedido
		 * @param pathDestino path del archivo destino
		 * @return el tamaño del comando.
		 */
		size_t obtenerTamanioSolicitarTransf(const std::string &pathArchivo, 
										const std::string &pathDestino);
		
		
		// Comando Pedir Lista de archivos
		
		/**
         * @pre Válido para comando Pedir Lista de archivos
         * Obtiene la lista de archivos compartidos del buffer
         * @return Mapa de pids y listas de archivos compartidos.
		 */
		std::map<TPID, ListaPaths*> *obtenerListaCompartidos();
		
		/**
         * @pre Válido para comando Pedir Lista de archivos
         * Serializa la lista de compartidos.
		 * @param mapaCompartidos Mapa de archivos compartidos
		 * @param tam Tamaño de la lista previamente calculado con el metodo obtenerTamanioLista. Por defecto 0, no se pasa el tamanio.
		 * @return Un buffer con la lista serializada.
		 */
		char *serializarLista(std::map<TPID, ListaPaths*> &mapaCompartidos, std::size_t tam = 0);
		
		/**
         * Obtiene el tamaño del array de chars que contendra la
		 * lista de archivos compartidos serializada
		 * @param mapa Mapa con los paths de archivos compartidos.
		 * @return El tamanio en chars.
		 */
		size_t obtenerTamanioLista(std::map<TPID, ListaPaths*> &mapa);
		
		/**
         * @return Tamaño total del comando actual en el buffer.
		 */
		 size_t obtenerTamanioComando();
};


#endif
