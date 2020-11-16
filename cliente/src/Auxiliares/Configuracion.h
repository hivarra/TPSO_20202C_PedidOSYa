/*
 * Configuracion.h
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONFIGURACION_H_
#define AUXILIARES_CONFIGURACION_H_

#include <shared.h>

typedef struct {
	char* ip;
	char* puerto;
	char* archivo_log;
	int posicion_x;
	int posicion_y;
	char* id_cliente;
} t_cliente_config;

t_config* config;
t_cliente_config cliente_config;
t_log *logger;

void cargarConfigCliente(char*);
void cargar_logger_cliente();

#endif /* AUXILIARES_CONFIGURACION_H_ */
