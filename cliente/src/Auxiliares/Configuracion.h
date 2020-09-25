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
	char* ip_comanda;
	int puerto_comanda;
	char* ip_restaurante;
	int puerto_restaurante;
	char* ip_sindicato;
	int puerto_sindicato;
	char* ip_app;
	int puerto_app;
	char* archivo_log;
	int posicion_x;
	int posicion_y;
	char* proceso_server;
	u_int32_t id_cliente;
} t_cliente_config;

t_config* config;
t_cliente_config cliente_config;

void cargarConfigCliente();

#endif /* AUXILIARES_CONFIGURACION_H_ */
