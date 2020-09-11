/*
 * Configuracion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#include "Configuracion.h"

void cargar_configuracion_restaurante(char * path_config) {
	t_config* config = config_create(path_config);

	if (config_has_property(config, "PUERTO_ESCUCHA")) {
		restaurante_conf.puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
	} else {
		error_show("Error al cargar PUERTO_ESCUCHA de archivo de configuracion");
	}
	if (config_has_property(config, "IP_SINDICATO")) {
		restaurante_conf.ip_sindicato = config_get_string_value(config, "IP_SINDICATO");
	} else {
		error_show("Error al cargar IP_SINDICATO de archivo de configuracion");
	}
	if (config_has_property(config, "PUERTO_SINDICATO")) {
		restaurante_conf.puerto_sindicato = config_get_int_value(config, "PUERTO_SINDICATO");
	} else {
		error_show("Error al cargar PUERTO_SINDICATO de archivo de configuracion");
	}
	if (config_has_property(config, "IP_APP")) {
		restaurante_conf.ip_app = config_get_string_value(config, "IP_APP");
	} else {
		error_show("Error al cargar IP_APP de archivo de configuracion");
	}
	if (config_has_property(config, "PUERTO_APP")) {
		restaurante_conf.puerto_app = config_get_int_value(config, "PUERTO_APP");
	} else {
		error_show("Error al cargar PUERTO_APP de archivo de configuracion");
	}
	if (config_has_property(config, "QUANTUM")) {
		restaurante_conf.quantum = config_get_int_value(config, "QUANTUM");
	} else {
		error_show("Error al cargar QUANTUM de archivo de configuracion");
	}
	if (config_has_property(config, "ARCHIVO_LOG")) {
		restaurante_conf.archivo_log = config_get_string_value(config, "ARCHIVO_LOG");
	} else {
		error_show("Error al cargar ARCHIVO_LOG de archivo de configuracion");
	}
	if (config_has_property(config, "ALGORITMO_PLANIFICACION")) {
		restaurante_conf.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	} else {
		error_show("Error al cargar ALGORITMO_PLANIFICACION de archivo de configuracion");
	}
	if (config_has_property(config, "NOMBRE_RESTAURANTE")) {
		restaurante_conf.nombre_restaurante = config_get_string_value(config, "NOMBRE_RESTAURANTE");
	} else {
		error_show("Error al cargar NOMBRE_RESTAURANTE de archivo de configuracion");
	}
	free(path_config);
}
void mostrar_propiedades() {

	log_info(logger,"Propiedades cargadas:");
	log_info(logger,"Puerto escucha: %d", restaurante_conf.puerto_escucha);
	log_info(logger,"IP sindicato: %s", restaurante_conf.ip_sindicato);
	log_info(logger,"Puerto sindicato: %d", restaurante_conf.puerto_sindicato);
	log_info(logger,"IP app: %s", restaurante_conf.ip_app);
	log_info(logger,"Puerto app: %d", restaurante_conf.puerto_app);
	log_info(logger,"Algoritmo planificacion: %s", restaurante_conf.algoritmo_planificacion);
	log_info(logger,"Nombre restaurante: %s", restaurante_conf.nombre_restaurante);
}
