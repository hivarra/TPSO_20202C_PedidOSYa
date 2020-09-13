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
#include "Configuracion.h"

int socket_app;
t_list* restaurantes;
pthread_mutex_t mutex_restaurantes;

void *crearServidor();
void* atenderConexion();
void procesar_mensaje_restaurante(t_mensaje* msg, int socket_cliente);
void procesar_mensaje_cliente(t_mensaje* msg, int socket_cliente);
void imprimir_restaurante(t_restaurante* restaurante);
void agregarRestaurante(t_restaurante* restaurante);

#endif /* AUXILIARES_CONEXION_H_ */
