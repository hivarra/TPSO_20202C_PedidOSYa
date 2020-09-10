/*
 * Configuracion.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONFIGURACION_H_
#define AUXILIARES_CONFIGURACION_H_

#include <shared.h>
#include "Logueo.h"

// Definición de tipos
typedef struct {
	uint32_t tamanio_memoria;
	uint32_t tamanio_swap;
	char* algoritmo_reemplazo;
	uint32_t puerto_comanda;
	uint32_t frecuencia_compactacion;
	char* archivo_log;

} t_comanda_conf;

// Variables globales
t_config* config;
t_comanda_conf comanda_conf;


void cargar_configuracion_comanda(char *path_config);
void mostrar_propiedades();

#endif /* AUXILIARES_CONFIGURACION_H_ */
