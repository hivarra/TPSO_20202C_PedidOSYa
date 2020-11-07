/*
 * Mensajes.c
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */
#include "Mensajes.h"

void escuchar_cliente_particular(t_cliente* cliente_particular){
	while(1){

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*cliente_particular->socket_envio, logger);
		if (tipo_mensaje == -1){
			log_warning(logger, "Se desconecto del cliente %s", cliente_particular->nombre);
			printf("Se desconecto del cliente %s\n", cliente_particular->nombre);
			close(*cliente_particular->socket_envio);
			pthread_exit(NULL);
		}

		log_info(logger, "[From %s] Se recibe tipo de mensaje: %s", cliente_particular->nombre, get_nombre_mensaje(tipo_mensaje));

		switch(tipo_mensaje){

			case CONSULTAR_PLATOS:{
				char* nombre_restaurante = recibir_consultar_platos(*cliente_particular->socket_envio, logger);//No importa este nombre
				log_info(logger, "[CONSULTAR_PLATOS]Nombre restaurante: %s", nombre_restaurante);//TODO:BORRAR XQ NO IMPORTA ESTE PARAMETRO
				free(nombre_restaurante);
				//t_rta_consultar_platos* respuesta = procesar_consultar_platos();//Debe reenviar el msj a sindicato
				t_rta_consultar_platos* respuesta = malloc(sizeof(t_rta_consultar_platos));
				respuesta->cantPlatos = 2;
				respuesta->platos = list_create();
				t_plato* plato1 = calloc(1,sizeof(t_plato));
				strcpy(plato1->nombre, "ENSALADA");
				plato1->precio = 12;
				list_add(respuesta->platos, plato1);
				t_plato* plato2 = calloc(1,sizeof(t_plato));
				strcpy(plato2->nombre, "BONDIOLA");
				plato2->precio = 99;
				list_add(respuesta->platos, plato2);
				enviar_rta_consultar_platos(respuesta, *cliente_particular->socket_envio, logger);//TODO:PRUEBA
				list_destroy_and_destroy_elements(respuesta->platos, free);
				free(respuesta);
			}
			break;
			case CREAR_PEDIDO:{
				recibir_mensaje_vacio(*cliente_particular->socket_envio, logger);
				//uint32_t id_pedido = procesar_crear_pedido();
				enviar_entero(RTA_CREAR_PEDIDO, 58, *cliente_particular->socket_envio, logger);//TODO:PRUEBA
			}
			break;
			case ANADIR_PLATO:{
				t_anadir_plato* msg_anadir_plato = recibir_anadir_plato(*cliente_particular->socket_envio, logger);
				log_info(logger, "[ANADIR_PLATO]Plato: %s, ID_Pedido: %d", msg_anadir_plato->plato, msg_anadir_plato->id_pedido);
				//uint32_t resultado = procesar_anadir_plato(msg_anadir_plato);
				free(msg_anadir_plato);
				enviar_entero(RTA_ANADIR_PLATO, 1, *cliente_particular->socket_envio, logger);//TODO:PRUEBA
			}
			break;
			case CONFIRMAR_PEDIDO:{
				t_confirmar_pedido* msg_confirmar_pedido = recibir_confirmar_pedido(*cliente_particular->socket_envio, logger);
				log_info(logger, "[CONFIRMAR_PEDIDO]ID_Pedido: %d", msg_confirmar_pedido->id_pedido);
				uint32_t resultado = procesar_confirmar_pedido(msg_confirmar_pedido);
				free(msg_confirmar_pedido);
				enviar_entero(RTA_CONFIRMAR_PEDIDO, 0, *cliente_particular->socket_envio, logger);//TODO:PRUEBA
			}
			break;
			case CONSULTAR_PEDIDO:{
				uint32_t id_pedido = recibir_entero(*cliente_particular->socket_envio, logger);
				log_info(logger, "[CONSULTAR_PEDIDO]ID_Pedido: %d", id_pedido);
				//t_rta_consultar_pedido* respuesta = procesar_consultar_pedido(id_pedido);
				//enviar_rta_consultar_pedido(respuesta, *cliente_particular->socket_envio, logger);
				//free(respuesta);
			}
			break;
			default:
				puts("No se reconoce el tipo de mensaje recibido");
				break;
		}
	}
}

void procesar_socket_envio(t_socket_envio* msg_socket_envio, int* socket_emisor){

	t_cliente* new_client = calloc(1,sizeof(t_cliente));
	strcpy(new_client->nombre, msg_socket_envio->id);
	new_client->socket_envio = socket_emisor;
	list_add(clientes_conectados, new_client);

	pthread_t hilo_escucha_new_client;
	pthread_create(&hilo_escucha_new_client, NULL, (void*)escuchar_cliente_particular, new_client);
	pthread_detach(hilo_escucha_new_client);
}

void procesar_socket_escucha(t_socket_escucha* msg_socket_escucha, int* socket_emisor){

	bool _busqueda_id(t_cliente* buscado){
		return (strcmp(msg_socket_escucha->id, buscado->nombre) == 0);
	}

	t_cliente* cliente_buscado = list_find(clientes_conectados, (void*)_busqueda_id);
	if (cliente_buscado == NULL)
		log_error(logger, "[SOCKET_ESCUCHA]No se encontro el cliente conectado");
	else
		cliente_buscado->socket_escucha = socket_emisor;
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
