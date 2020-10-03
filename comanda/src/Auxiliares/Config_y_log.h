/*
 * Configuracion.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONFIG_Y_LOG_H_
#define AUXILIARES_CONFIG_Y_LOG_H_

#include <shared.h>

// Variables globales
t_config* config;
t_log* logger;

void cargar_configuracion_comanda(char *path_config);
void cargar_logger_comanda();
void mostrar_propiedades();

#endif /* AUXILIARES_CONFIG_Y_LOG_H_ */
