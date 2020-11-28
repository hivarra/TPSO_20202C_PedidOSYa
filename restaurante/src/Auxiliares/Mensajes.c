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
	respuesta->cantPlatos = 0;
	respuesta->platos = list_create();

	int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_sindicato == -1)
		log_warning(logger, "[Consultar PLatos] No se pudo conectar a Sindicato");
	else{
		char* msg_consultar_platos = calloc(1,L_ID);
		strcpy(msg_consultar_platos, restaurante_conf.nombre_restaurante);
		log_info(logger, "[Consultar Platos] Se consulta a Sindicato los platos.");
		enviar_consultar_platos(msg_consultar_platos, socket_sindicato, logger);
		free(msg_consultar_platos);
		t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
		if (tipo_rta == RTA_CONSULTAR_PLATOS){
			list_destroy(respuesta->platos);
			free(respuesta);
			respuesta = recibir_rta_consultar_platos(socket_sindicato, logger);
		}
		close(socket_sindicato);
	}
	return respuesta;
}

uint32_t procesar_crear_pedido(){
	uint32_t nuevo_id = 0;

	int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_sindicato == -1)
		log_warning(logger, "[Crear Pedido] No se pudo conectar a Sindicato");
	else{
		pthread_mutex_lock(&mutex_id_pedidos);
		id_pedidos++;
		t_guardar_pedido* msg_guardar_pedido = calloc(1,sizeof(t_guardar_pedido));
		strcpy(msg_guardar_pedido->restaurante, restaurante_conf.nombre_restaurante);
		msg_guardar_pedido->id_pedido = id_pedidos;
		log_info(logger, "[Crear Pedido] Se envia a Sindicato GUARDAR_PEDIDO con ID %d.", id_pedidos);
		enviar_guardar_pedido(msg_guardar_pedido, socket_sindicato, logger);
		free(msg_guardar_pedido);
		t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
		if (tipo_rta == RTA_GUARDAR_PEDIDO){
			uint32_t resultado = recibir_entero(socket_sindicato, logger);
			log_info(logger, "[RTA_GUARDAR_PEDIDO]Resultado: %s",resultado? "OK":"FAIL");
			if (!resultado)
				id_pedidos--;
			else
				nuevo id = id_pedidos;
		}
		pthread_mutex_unlock(&mutex_id_pedidos);
		close(socket_sindicato);
	}
	return nuevo_id;
}

uint32_t procesar_anadir_plato(t_anadir_plato* msg_anadir_plato){
	uint32_t resultado = 0;

	pthread_mutex_lock(&mutex_id_pedidos);
	uint32_t id_maximo = id_pedidos;
	pthread_mutex_unlock(&mutex_id_pedidos);

	if(msg_anadir_plato->id_pedido <= id_maximo){
		int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
		if (socket_sindicato == -1)
			log_warning(logger, "[Anadir Plato] No se pudo conectar a Sindicato");
		else{
			t_guardar_plato* msg_guardar_plato = calloc(1,sizeof(t_guardar_plato));
			strcpy(msg_guardar_plato->restaurante, restaurante_conf.nombre_restaurante);
			msg_guardar_plato->id_pedido = msg_anadir_plato->id_pedido;
			strcpy(msg_guardar_plato->plato, msg_anadir_plato->plato);
			msg_guardar_plato->cantPlato = 1;
			log_info(logger, "[Anadir Plato] Se envia a Sindicato GUARDAR_PLATO ID: %d, Plato: %s, Cant.: 1", msg_anadir_plato->id_pedido, msg_anadir_plato->plato);
			enviar_guardar_plato(msg_guardar_plato, socket_sindicato, logger);
			free(msg_guardar_plato);
			t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
			if (tipo_rta == RTA_GUARDAR_PLATO)
				resultado = recibir_entero(socket_sindicato, logger);
			close(socket_sindicato);
		}
	}
	else
		log_warning(logger, "[Anadir Plato] Pedido %d inexistente.", msg_anadir_plato->id_pedido);
	return resultado;
}

uint32_t procesar_confirmar_pedido(t_confirmar_pedido* msg_confirmar_pedido){
	uint32_t resultado = 0;
	//TODO: Toda la logica para arrancar la planificacion de los platos
	return resultado;
}

t_rta_consultar_pedido* procesar_consultar_pedido(uint32_t id_pedido){
	t_rta_consultar_pedido* respuesta = calloc(1,sizeof(t_rta_consultar_pedido));
	strcpy(respuesta->restaurante, restaurante_conf.nombre_restaurante);
	respuesta->cantComidas = 0;
	respuesta->estado = 0;
	respuesta->comidas = list_create();

	int socket_sindicato = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_sindicato == -1)
		log_warning(logger, "[Consultar Pedido] No se pudo conectar a Sindicato");
	else{
		t_obtener_pedido* msg_obtener_pedido = calloc(1,sizeof(t_obtener_pedido));
		strcpy(msg_obtener_pedido->restaurante, restaurante_conf.nombre_restaurante);
		msg_obtener_pedido->id_pedido = id_pedido;
		log_info(logger, "[Consultar Pedido] Se consulta a Sindicato la info del pedido %d.", id_pedido);
		enviar_obtener_pedido(msg_obtener_pedido, socket_sindicato, logger);
		free(msg_obtener_pedido);
		t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_sindicato, logger);
		if (tipo_rta == RTA_OBTENER_PEDIDO){
			t_rta_obtener_pedido* respuesta_obt_ped = recibir_rta_obtener_pedido(socket_sindicato, logger);
			respuesta->cantComidas = respuesta_obt_ped->cantComidas;
			list_destroy(respuesta->comidas);
			respuesta->comidas = respuesta_obt_ped->comidas;
			respuesta->estado = respuesta_obt_ped->estado;
			free(respuesta_obt_ped);
		}
		close(socket_sindicato);
	}
	return respuesta;
}
