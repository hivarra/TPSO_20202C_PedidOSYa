/*
 * Configuracion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#include "Configuracion.h"

void cargar_configuracion_restaurante(char * path_config) {

	config = config_create(path_config);
	if (config == NULL){
		puts("Archivo de configuracion no valido");
		exit(-1);
	}

	if (!config_has_property(config, "PUERTO_ESCUCHA")) {
		puts("Error al leer PUERTO_ESCUCHA de archivo de configuracion");
		exit(2);
	}

	if (!config_has_property(config, "IP_SINDICATO")) {
		puts("Error al leer IP_SINDICATO de archivo de configuracion");
		exit(2);
	}
	else
		restaurante_conf.ip_sindicato = config_get_string_value(config, "IP_SINDICATO");

	if (!config_has_property(config, "PUERTO_SINDICATO")) {
		puts("Error al leer PUERTO_SINDICATO de archivo de configuracion");
		exit(2);
	}
	else
		restaurante_conf.puerto_sindicato = config_get_string_value(config, "PUERTO_SINDICATO");

	if (!config_has_property(config, "IP_APP")){
		puts("Error al leer IP_APP de archivo de configuracion");
		exit(2);
	}

	if (!config_has_property(config, "PUERTO_APP")){
		puts("Error al leer PUERTO_APP de archivo de configuracion");
		exit(2);
	}

	if (!config_has_property(config, "QUANTUM")) {
		puts("Error al leer QUANTUM de archivo de configuracion");
		exit(2);
	}
	else
		restaurante_conf.quantum = config_get_int_value(config, "QUANTUM");

	if (!config_has_property(config, "ARCHIVO_LOG")) {
		puts("Error al leer ARCHIVO_LOG de archivo de configuracion");
		exit(2);
	}

	if (!config_has_property(config, "ALGORITMO_PLANIFICACION")) {
		puts("Error al leer ALGORITMO_PLANIFICACION de archivo de configuracion");
		exit(2);
	}
	else if (strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "FIFO")!= 0
			&& strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "RR")!= 0){
		puts("ALGORITMO_PLANIFICACION no valido");
		exit(2);
	}

	if (!config_has_property(config, "NOMBRE_RESTAURANTE")) {
		puts("Error al leer NOMBRE_RESTAURANTE de archivo de configuracion\n");
		exit(2);
	}
	else
		restaurante_conf.nombre_restaurante = config_get_string_value(config, "NOMBRE_RESTAURANTE");
}

void cargar_logger_restaurante() {
	int carpeta_creada;

	carpeta_creada = crear_carpeta_log(config_get_string_value(config, "ARCHIVO_LOG"));
	if (carpeta_creada){
		logger = log_create(config_get_string_value(config, "ARCHIVO_LOG"), "Sindicato", 1, LOG_LEVEL_TRACE);
		log_info(logger, "*************** NUEVO LOG ***************");
	}
	else{
		puts("Error al crear la carpera del log");
		exit(2);
	}
}

void mostrar_propiedades() {

	log_info(logger,"Propiedades leidas:");
	log_info(logger,"Puerto escucha: %s", config_get_string_value(config, "PUERTO_ESCUCHA"));
	log_info(logger,"IP Sindicato: %s", restaurante_conf.ip_sindicato);
	log_info(logger,"Puerto Sindicato: %s", restaurante_conf.puerto_sindicato);
	log_info(logger,"IP app: %s", config_get_string_value(config, "IP_APP"));
	log_info(logger,"Puerto App: %s", config_get_string_value(config, "PUERTO_APP"));
	log_info(logger,"Quantum: %d", restaurante_conf.quantum);
	log_info(logger,"Algoritmo planificacion: %s", config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	log_info(logger,"Nombre restaurante: %s", restaurante_conf.nombre_restaurante);
	log_info(logger,"Archvo log: %s", config_get_string_value(config, "ARCHIVO_LOG"));
}
