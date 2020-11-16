/*
 * Mensajes.c
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */
#include "Mensajes.h"

t_cliente* buscar_cliente(char* id){

	bool _busqueda_id(t_cliente* buscado){
		return (strcmp(id, buscado->nombre) == 0);
	}

	t_cliente* cliente_buscado = list_find(clientes_conectados, (void*)_busqueda_id);

	return cliente_buscado;
}

void procesar_handshake_inicial(t_handshake_inicial* msg_handshake_inicial, int socket_emisor){

	t_cliente* new_client = calloc(1,sizeof(t_cliente));
	strcpy(new_client->nombre, msg_handshake_inicial->id);
	new_client->socket_escucha = socket_emisor;
	list_add(clientes_conectados, new_client);
}

t_rta_consultar_platos* procesar_consultar_platos(){
	t_rta_consultar_platos* respuesta = malloc(sizeof(t_rta_consultar_platos));
	return respuesta;
}

uint32_t procesar_crear_pedido(){
	uint32_t id_pedido;
	return id_pedido;
}

uint32_t procesar_anadir_plato(t_anadir_plato* msg_anadir_plato){
	uint32_t resultado;
	return resultado;
}

uint32_t procesar_confirmar_pedido(t_confirmar_pedido* msg_confirmar_pedido){
	uint32_t resultado;
	return resultado;
}

t_rta_consultar_pedido* procesar_consultar_pedido(uint32_t id_pedido){
	t_rta_consultar_pedido* respuesta = malloc(sizeof(t_rta_consultar_pedido));
	return respuesta;
}
