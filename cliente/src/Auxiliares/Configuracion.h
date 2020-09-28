/*
 * Configuracion.h
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONFIGURACION_H_
#define AUXILIARES_CONFIGURACION_H_
#define PATH  "../config/cliente.config"

#include <shared.h>

typedef struct {
	char* ip;
	int puerto;
	char* archivo_log;
	int posicion_x;
	int posicion_y;
	u_int32_t id_cliente;
} t_cliente_config;

t_config* config;
t_cliente_config cliente_config;

void cargarConfigCliente();

#endif /* AUXILIARES_CONFIGURACION_H_ */
