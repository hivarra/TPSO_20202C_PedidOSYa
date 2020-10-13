/*
 * Conexion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

#define IP_APP "127.0.0.1"

#include <shared.h>
#include <protocolo.h>
#include "Configuracion.h"

typedef struct{
	char id[L_ID];
	uint32_t tipoProceso;
	int socket;
	int pos_x;
	int pos_y;
} t_info_cliente;

int socket_app;
t_list* restaurantes;
pthread_mutex_t mutex_restaurantes;

t_list* lista_clientes_restaurantes;

void *crearServidor();
void* atenderConexion();
void inicializar_lista_clientes_rest();
void incializar_resto_default();
//void imprimir_restaurante(t_restaurante* restaurante);
//void agregarRestaurante(t_restaurante* restaurante);

#endif /* AUXILIARES_CONEXION_H_ */
