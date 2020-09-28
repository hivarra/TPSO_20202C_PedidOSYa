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
	if (config_has_property(config, "PLATOS_DEFAULT")) {
		restaurante_conf.platos = config_get_array_value(config, "PLATOS_DEFAULT");
	} else {
		error_show("Error al cargar PLATOS_DEFAULT de archivo de configuracion");
	}
	if (config_has_property(config, "AFINIDAD_COCINEROS_DEFAULT")) {
		restaurante_conf.afinidad_cocineros = config_get_array_value(config, "AFINIDAD_COCINEROS_DEFAULT");
	} else {
		error_show("Error al cargar AFINIDAD_COCINEROS_DEFAULT de archivo de configuracion");
	}
	if (config_has_property(config, "CANTIDAD_COCINEROS_DEFAULT")) {
		restaurante_conf.cantidad_cocineros = config_get_int_value(config, "CANTIDAD_COCINEROS_DEFAULT");
	} else {
		error_show("Error al cargar CANTIDAD_COCINEROS_DEFAULT de archivo de configuracion");
	}
	if (config_has_property(config, "RECETA_DEFAULT")) {
		restaurante_conf.receta_nombre = config_get_string_value(config, "RECETA_DEFAULT");
	} else {
		error_show("Error al cargar RECETA_DEFAULT de archivo de configuracion");
	}
	if (config_has_property(config, "RECETA_PASOS_DEFAULT")) {
		restaurante_conf.receta_pasos = config_get_array_value(config, "RECETA_PASOS_DEFAULT");
	} else {
		error_show("Error al cargar RECETA_DEFAULT de archivo de configuracion");
	}
	if (config_has_property(config, "PRECIO_PLATO_DEFAULT")) {
		restaurante_conf.precio_plato = config_get_int_value(config, "PRECIO_PLATO_DEFAULT");
	} else {
		error_show("Error al cargar PRECIO_PLATO_DEFAULT de archivo de configuracion");
	}
	if (config_has_property(config, "CANTIDAD_HORNOS_DEFAULT")) {
		restaurante_conf.cantidad_hornos = config_get_int_value(config, "CANTIDAD_HORNOS_DEFAULT");
	} else {
		error_show("Error al cargar CANTIDAD_HORNOS_DEFAULT de archivo de configuracion");
	}

	free(path_config);
}
void mostrar_propiedades() {

	log_info(logger,"Propiedades cargadas:");
	log_info(logger,"Puerto escucha: %d", restaurante_conf.puerto_escucha);
	log_info(logger,"IP sindicato: %s", restaurante_conf.ip_sindicato);
	log_info(logger,"Puerto sindicato: %d", restaurante_conf.puerto_sindicato);
	log_info(logger,"IP app: %s", restaurante_conf.ip_app);
	log_info(logger,"Quantum: %d", restaurante_conf.quantum);
	log_info(logger,"Puerto app: %d", restaurante_conf.puerto_app);
	log_info(logger,"Algoritmo planificacion: %s", restaurante_conf.algoritmo_planificacion);
	log_info(logger,"Nombre restaurante: %s", restaurante_conf.nombre_restaurante);
	log_info(logger,"Cantidad Cocineros: %d", restaurante_conf.cantidad_cocineros);
	log_info(logger,"Precio Plato Default: %d", restaurante_conf.precio_plato);
	log_info(logger,"Cantidad Hornos Default: %d", restaurante_conf.cantidad_hornos);

	int j=0;
	while(restaurante_conf.afinidad_cocineros[j]){
		log_info(logger, "id %d | Afinidad Cocineros:%s",j, restaurante_conf.afinidad_cocineros[j]);
		j++;
	}

	int f=0;
	while(restaurante_conf.platos[f]){
		log_info(logger, "id %d | Platos Default:%s",f, restaurante_conf.platos[f]);
		f++;
	}
	log_info(logger,"Receta Nombre: %s", restaurante_conf.receta_nombre);
	int n=0;
	while(restaurante_conf.receta_pasos[n]){

		char** receta_completa= string_split(restaurante_conf.receta_pasos[n], "|");

		log_info(logger,"Paso Default: %s", receta_completa[0]);
		log_info(logger, "Riempo Pasos default:%d", atoi(receta_completa[1]));
		n++;
	}


}
