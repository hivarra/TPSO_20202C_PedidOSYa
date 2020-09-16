/*
 * Conexion.h
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONEXION_H_
#define AUXILIARES_CONEXION_H_
#define IP_COMANDA "127.0.0.1"

#include <shared.h>
#include "Configuracion.h"

int socket_comanda;
int socket_cliente;

typedef struct{
	uint32_t id_cliente;
	uint32_t socket;
} t_cliente_info;

t_list* lista_clientes;

void* escuchar_conexiones();
void agregar_cliente(t_mensaje* msg,int socket);
t_cliente_info* guardar_info_cliente(t_mensaje* msg,int socket);
t_cliente_info* crear_cliente_info(int id_cliente, int socket);
void agregar_a_lista_clientes(t_cliente_info* cliente_info);
t_cliente_info* buscar_cliente(int id_cliente);
int existe_cliente(t_cliente_info* cliente_info);

#endif /* AUXILIARES_CONEXION_H_ */
