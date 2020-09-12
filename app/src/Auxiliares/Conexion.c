/*
 * Conexion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#include "Conexion.h"

void *escucharConexiones() {

	/*Abro socket*/
	socket_app = definirSocket(logger);

	/*Bind & Listen*/
	if (bindearSocketYEscuchar(socket_app, IP_APP , app_conf.puerto_escucha,
			logger) <= 0)
		_exit_with_error("BIND", NULL, logger);

	/*Atiendo solicitudes creando un hilo para cada una*/
	while(1){
		pthread_t hilo_conexion;
		int* socketCliente = malloc(sizeof(int));

		*socketCliente = aceptarConexiones(socket_app, logger);

		if(socketCliente < 0)
			log_error(logger, "Error al realizar ACCEPT.\n");

		pthread_create(&hilo_conexion,NULL,(void*)atenderConexion,socketCliente);
		pthread_detach(hilo_conexion);
	}
}

void* atenderConexion(int* socket_cliente) {

	t_mensaje* msg = malloc(sizeof(t_mensaje));
	msg = recibirMensaje(*socket_cliente, logger);
	log_info(logger, "Conexion de proceso:%s a APP",
					get_nombre_proceso(msg->header.tipoProceso));
	enviarMensaje(APP, HANDSHAKE, 0, NULL, *socket_cliente,
			msg->header.tipoProceso, logger);
	msg = recibirMensaje(*socket_cliente, logger);

	switch (msg->header.tipoMensaje) {

	case CONSULTAR_RESTAURANTES:
//		t_guardar_pedido* msj_guardar_pedido = malloc(msg->header.longitud);
//		memcpy(msj_guardar_pedido, msg->content, msg->header.longitud);
//
//		log_trace(logger, "MENSAJE GUARDAR_PEDIDO\n");
//		log_trace(logger, "NOMBRE_RESTAURANTE: %s\n",
//				msj_guardar_pedido->nombre_restaurante);
//		log_trace(logger, "ID_PEDIDO: %d\n", msj_guardar_pedido->id_pedido);
		/*TODO:*/
//		int resultado = guardar_mensaje(msg->header.tipoMensaje, msj_guardar_pedido);
//
//		enviarMensaje(COMANDA, RESULTADO_GUARDAR, sizeof(int), &resultado, cliente_socket, msg->header.tipoProceso, logger);

//		free(msj_guardar_pedido);

		break;

	case SELECCIONAR_RESTAURANTE:
		;
//		TODO:
		break;

	case CONSULTAR_PLATOS:
		;
//		TODO:
		break;

	case CREAR_PEDIDO:
		;
//		TODO:
		break;

	case ANADIR_PLATO:
		;
//		TODO:
		break;

	case PLATO_LISTO:
		;
//		TODO:
		break;

	case CONFIRMAR_PEDIDO:
		;
//		TODO:
		break;
	case CONSULTAR_PEDIDO:
		;
//		TODO:
		break;
	default:
		log_error(logger, "Mensaje no reconocido: %s",
					get_nombre_mensaje( msg->header.tipoMensaje));
	}

	return NULL;
}
