/*
 * Configuracion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONFIGURACION_H_
#define AUXILIARES_CONFIGURACION_H_

#include <shared.h>
#include "Logueo.h"

// Definición de tipos
typedef struct {
	int puerto_escucha;
	char* ip_sindicato;
	int puerto_sindicato;
	char* ip_app;
	int puerto_app;
	int quantum;
	char* archivo_log;
	char* algoritmo_planificacion;
	char* nombre_restaurante;
} t_restaurante_conf;

// Variables globales
t_config* config;
t_restaurante_conf restaurante_conf;


void cargar_configuracion_restaurante(char *path_config);
void mostrar_propiedades();

#endif /* AUXILIARES_CONFIGURACION_H_ */
