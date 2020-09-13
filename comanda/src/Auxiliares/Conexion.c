/*
 * Conexion.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#include "Conexion.h"

void connection_handler(int* socket_emisor){
	t_mensaje* msg = malloc(sizeof(t_mensaje));
	msg = recibirMensaje(*socket_emisor, logger);
	log_info(logger, "Conexion de proceso:%s a COMANDA",
					get_nombre_proceso(msg->header.tipoProceso));
	enviarMensaje(COMANDA, HANDSHAKE, 0, NULL, *socket_emisor,
			msg->header.tipoProceso, logger);
	msg = recibirMensaje(*socket_emisor, logger);

	switch (msg->header.tipoMensaje) {

	case GUARDAR_PEDIDO:
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
	free(msg);
}

void esperar_cliente(int socket_servidor){
	pthread_t hilo_conexion;

	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, (void*) &tam_direccion);

	pthread_create(&hilo_conexion, NULL, (void*)connection_handler, &socket_cliente);
	pthread_detach(hilo_conexion);
}

void escuchar_conexiones(){

	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP_COMANDA, comanda_conf.puerto_comanda, &hints, &servinfo);

	for (p=servinfo; p != NULL; p = p->ai_next)
	{
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			log_error(logger, "No se pudo abrir el socket de escucha");
			continue;

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			log_error(logger, "No se pudo bindear el socket de escucha");
			close(socket_servidor);
			continue;
		}
		break;
	}

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);

	log_info(logger, "SERVIDOR | Escuchando conexiones");

	while(1)
		esperar_cliente(socket_servidor);
}

//void *escuchar_conexiones(){
//	/*Abro socket*/
//	socket_comanda = definirSocket(logger);
//	/*Bind & Listen*/
//	if (bindearSocketYEscuchar(socket_comanda, IP_COMANDA , comanda_conf.puerto_comanda,
//			logger) <= 0)
//		_exit_with_error("BIND", NULL, logger);
//
//	/*Atiendo solicitudes creando un hilo para cada una*/
//	while(1){
//		pthread_t hilo_conexion;
//		int* new_sock = malloc(sizeof(int));
//
//		*new_sock = aceptarConexiones(socket_comanda, logger);
//
//		if(new_sock < 0)
//			log_error(logger, "Error al realizar ACCEPT.\n");
//
//		pthread_create(&hilo_conexion,NULL,(void*)connection_handler,new_sock);
//		pthread_detach(hilo_conexion);
//	}
//}
