/*
 * Configuracion.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */


#include "Config_y_log.h"

void cargar_configuracion_comanda(char * path_config) {
	config = config_create(path_config);
	if (config == NULL){
		puts("Archivo de configuracion no valido");
		exit(-1);
	}

	if (!config_has_property(config, "TAMANIO_MEMORIA")) {
		puts("Error al leer TAMANIO_MEMORIA de archivo de configuracion");
		exit(2);
	}

	if (!config_has_property(config, "TAMANIO_SWAP")) {
		puts("Error al leer TAMANIO_SWAP de archivo de configuracion");
		exit(2);
	}

	if (!config_has_property(config, "ALGORITMO_REEMPLAZO")){
		puts("Error al leer ALGORITMO_REEMPLAZO de archivo de configuracion");
		exit(2);
	}
	else if (strcmp(config_get_string_value(config, "ALGORITMO_REEMPLAZO"), "LRU")!= 0
			&& strcmp(config_get_string_value(config, "ALGORITMO_REEMPLAZO"), "CLOCK_MEJ")!= 0){
		puts("ALGORITMO_REEMPLAZO no valido");
		exit(2);
	}

	if (!config_has_property(config, "PUERTO_ESCUCHA")) {
		puts("Error al leer PUERTO_ESCUCHA de archivo de configuracion");
		exit(2);
	}
	if (!config_has_property(config, "ARCHIVO_LOG")) {
		puts("Error al leer ARCHIVO_LOG de archivo de configuracion");
		exit(2);
	}
}


void cargar_logger_comanda() {
	int carpeta_creada;

	carpeta_creada = crear_carpeta_log(config_get_string_value(config, "ARCHIVO_LOG"));
	if (carpeta_creada){
		logger = log_create(config_get_string_value(config, "ARCHIVO_LOG"), "Comanda", 1, LOG_LEVEL_TRACE);
		log_info(logger, "*************** NUEVO LOG ***************");
	}
	else{
		puts("Error al crear la carpera del log");
		exit(2);
	}
}

// Auxiliares
void mostrar_propiedades() {
	log_trace(logger,"Propiedades leidas:");
	log_trace(logger,"Tamanio memoria: %d", config_get_int_value(config, "TAMANIO_MEMORIA"));
	log_trace(logger,"Tamanio swap: %d", config_get_int_value(config, "TAMANIO_SWAP"));
	log_trace(logger,"Algoritmo reemplazo: %s", config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
	log_trace(logger,"Puerto escucha: %s", config_get_string_value(config, "PUERTO_ESCUCHA"));
	log_trace(logger,"Archivo log: %s", config_get_string_value(config, "ARCHIVO_LOG"));
}
