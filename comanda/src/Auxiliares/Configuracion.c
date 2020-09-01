/*
 * Configuracion.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */


#include "Configuracion.h"

void cargar_configuracion_comanda(char * path_config) {
	t_config* config = config_create(path_config);

	if (config_has_property(config, "TAMANIO_MEMORIA")) {
		comanda_conf.tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	} else {
		loguear_error_carga_propiedad("TAMANIO_MEMORIA");
	}

	if (config_has_property(config, "TAMANIO_SWAP")) {
		comanda_conf.tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");
	} else {
		loguear_error_carga_propiedad("TAMANIO_SWAP");
	}

	if (config_has_property(config, "ALGORITMO_REEMPLAZO")) {
		comanda_conf.algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	} else {
		loguear_error_carga_propiedad("ALGORITMO_REEMPLAZO");
	}

	if (config_has_property(config, "PUERTO_ESCUCHA")) {
		comanda_conf.puerto_comanda = config_get_int_value(config, "PUERTO_ESCUCHA");
	} else {
		loguear_error_carga_propiedad("PUERTO_ESCUCHA");
	}

	if (config_has_property(config, "FRECUENCIA_COMPACTACION")) {
		comanda_conf.frecuencia_compactacion = config_get_int_value(config, "FRECUENCIA_COMPACTACION");
	} else {
		loguear_error_carga_propiedad("FRECUENCIA_COMPACTACION");
	}
	if (config_has_property(config, "ARCHIVO_LOG")) {
		comanda_conf.archivo_log = config_get_string_value(config, "ARCHIVO_LOG");
	} else {
		loguear_error_carga_propiedad("ARCHIVO_LOG");
	}
}

// Auxiliares
void mostrar_propiedades() {

	log_info(logger,"Propiedades cargadas:");
	log_info(logger,"Tamanio memoria: %d", comanda_conf.tamanio_memoria);
	log_info(logger,"Tamanio swap: %d", comanda_conf.tamanio_swap);
	log_info(logger,"Algoritmo reemplazo: %s", comanda_conf.algoritmo_reemplazo);
	log_info(logger,"Puerto escucha: %d", comanda_conf.puerto_comanda);
	log_info(logger,"Frecuencia compactacion: %d", comanda_conf.frecuencia_compactacion);
	log_info(logger,"Archivo log: %s", comanda_conf.archivo_log);
}
