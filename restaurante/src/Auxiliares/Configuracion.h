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
	uint32_t puerto_escucha;
	char* ip_sindicato;
	uint32_t puerto_sindicato;
	char* ip_app;
	uint32_t puerto_app;
	uint32_t quantum;
	char** afinidad_cocineros;
	uint32_t cantidad_cocineros;
	uint32_t cantidad_hornos;
	char* receta_nombre;
	char** receta_pasos;
	char** platos;
	uint32_t precio_plato;
	char* archivo_log;
	char* algoritmo_planificacion;
	char* nombre_restaurante;
	uint32_t posX;
	uint32_t posY;

} t_restaurante_conf;

// Variables globales
t_config* config;
t_restaurante_conf restaurante_conf;


void cargar_configuracion_restaurante(char *path_config);
void mostrar_propiedades();

#endif /* AUXILIARES_CONFIGURACION_H_ */
