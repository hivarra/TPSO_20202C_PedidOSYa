/*
 * Conexion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_

#include <shared.h>
#include "Configuracion.h"

#endif /* AUXILIARES_CONEXION_H_ */

int socket_restaurant;

t_list* cocineros;
t_list* platos;
t_list* recetas;
t_list* pedidos;

pthread_mutex_t mutex_cocineros;

//void *crearServidor(); TODO: crearServidor
//void *atenderConexion();TODO:
//void procesar_mensaje_app(t_mensaje* msg, int socket_app);TODO: procesar_mensaje_app
//void procesar_mensaje_cliente(t_mensaje* msg, int socket_cliente);TODO: procesar_mensaje_cliente
//void procesar_mensaje_sindicato(t_mensaje* msg, int socket_sindicato);TODO: procesar_mensaje_sindicato
//void imprimir_plato(t_plato* plato);
//void agregar_plato(t_plato* plato);
//void imprimir_recetas(t_plato* recetas);
//void agregar_recetas(t_plato* recetas);
//void imprimir_pedidos(t_plato* pedidos);
//void agregar_pedidos(t_plato* pedidos);


