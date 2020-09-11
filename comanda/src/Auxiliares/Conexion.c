/*
 * Conexion.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#include "Conexion.h"

void *escuchar_conexiones(){
	/*Abro socket*/
	int* new_sock;
	socket_comanda = definirSocket(logger);
	/*Bind & Listen*/
	if (bindearSocketYEscuchar(socket_comanda, IP_COMANDA , comanda_conf.puerto_comanda,
			logger) <= 0)
		_exit_with_error("BIND", NULL, logger);

	/*Atiendo solicitudes creando un hilo para cada una*/
	while(1){
		pthread_t hilo_conexion;
		new_sock = malloc(sizeof(int));

		*new_sock = aceptarConexiones(socket_comanda, logger);

		if(new_sock < 0)
			log_error(logger, "Error al realizar ACCEPT.\n");

		int* p_client = malloc(sizeof(int));
		*p_client = *new_sock;

		pthread_create(&hilo_conexion,NULL,(void*)connection_handler,p_client);
		pthread_detach(hilo_conexion);
	}
}
void connection_handler(void* socket_emisor){

	int cliente_socket = (*(int*) socket_emisor);
	free(socket_emisor);
	t_mensaje* msg = malloc(sizeof(t_mensaje));
	msg = recibirMensaje(cliente_socket, logger);
	log_info(logger, "Conexion de proceso:%s a COMANDA",
					get_nombre_proceso(msg->header.tipoProceso));
	enviarMensaje(COMANDA, HANDSHAKE, 0, NULL, cliente_socket,
			msg->header.tipoProceso, logger);
	msg = recibirMensaje(cliente_socket, logger);

	switch (msg->header.tipoMensaje) {

	case GUARDAR_PEDIDO:
		;
//		TODO:
		break;

	case GUARDAR_PLATO:
		;
//		TODO:
		break;

	case OBTENER_PEDIDO:
		;
//		TODO:
		break;

	case CONFIRMAR_PEDIDO:
		;
//		TODO:
		break;

	case PLATO_LISTO:
		;
//		TODO:
		break;

	case FINALIZAR_PEDIDO:
		;
//		TODO:
		break;

	default:
		log_error(logger, "Mensaje no reconocido: %s",
					get_nombre_mensaje( msg->header.tipoMensaje));
	}
}
