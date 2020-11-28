/*
 * Conexion.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#include "Conexion.h"

void connection_handler(int* socket_emisor){

	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*socket_emisor, logger);

	if (tipo_mensaje == -1){
		close(*socket_emisor);
		free(socket_emisor);
		pthread_exit(NULL);
	}

	log_trace(logger, "Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

	switch (tipo_mensaje) {

		case HANDSHAKE_INICIAL:{

			t_handshake_inicial* handshake_recibido = recibir_handshake_inicial(*socket_emisor, logger);
			free(handshake_recibido);//NO ME IMPORTA QUIEN SE CONECTA
			uint32_t miTipoProceso = COMANDA;
			enviar_entero(RTA_HANDSHAKE, miTipoProceso, *socket_emisor, logger);
			break;
		}

		case GUARDAR_PEDIDO:{

			t_guardar_pedido* msg_guardar_pedido = recibir_guardar_pedido(*socket_emisor, logger);
			log_trace(logger, "Restaurante: %s, ID_Pedido: %d", msg_guardar_pedido->restaurante, msg_guardar_pedido->id_pedido);
			uint32_t resultado_guardar_pedido = procesar_guardar_pedido(msg_guardar_pedido);
			free(msg_guardar_pedido);
			enviar_entero(RTA_GUARDAR_PEDIDO, resultado_guardar_pedido, *socket_emisor, logger);
			break;
		}

		case GUARDAR_PLATO:{

			t_guardar_plato* recibido = recibir_guardar_plato(*socket_emisor, logger);
			log_trace(logger, "Restaurante: %s, ID_Pedido: %d, Plato: %s, Cant: %d", recibido->restaurante, recibido->id_pedido, recibido->plato, recibido->cantPlato);
			uint32_t resultado_guardar_plato = procesar_guardar_plato(recibido);
			free(recibido);
			enviar_entero(RTA_GUARDAR_PLATO, resultado_guardar_plato, *socket_emisor, logger);
			break;
		}

		case OBTENER_PEDIDO:{

			t_obtener_pedido* recibido = recibir_obtener_pedido(*socket_emisor, logger);
			log_trace(logger, "Restaurante: %s, ID_Pedido: %d", recibido->restaurante, recibido->id_pedido);
			t_rta_obtener_pedido* respuesta = procesar_obtener_pedido(recibido);
			free(recibido);
			enviar_rta_obtener_pedido(respuesta, *socket_emisor, logger);
			list_destroy_and_destroy_elements(respuesta->comidas, free);
			free(respuesta);
			break;
		}

		case CONFIRMAR_PEDIDO:{

			t_confirmar_pedido* recibido = recibir_confirmar_pedido(*socket_emisor, logger);
			log_trace(logger, "Restaurante: %s, ID_Pedido: %d", recibido->restaurante, recibido->id_pedido);
			uint32_t resultado = procesar_confirmar_pedido(recibido);
			free(recibido);
			enviar_entero(RTA_CONFIRMAR_PEDIDO, resultado, *socket_emisor, logger);
			break;
		}

		case PLATO_LISTO:{

			t_plato_listo* recibido = recibir_plato_listo(*socket_emisor, logger);
			log_trace(logger, "Restaurante: %s, ID_Pedido: %d, Plato: %s", recibido->restaurante, recibido->id_pedido, recibido->plato);
			uint32_t resultado = procesar_plato_listo(recibido);
			free(recibido);
			enviar_entero(RTA_PLATO_LISTO, resultado, *socket_emisor, logger);
			break;
		}

		case FINALIZAR_PEDIDO:{

			t_finalizar_pedido* recibido = recibir_finalizar_pedido(*socket_emisor, logger);
			log_trace(logger, "Restaurante: %s, ID_Pedido: %d", recibido->restaurante, recibido->id_pedido);
			uint32_t resultado = procesar_finalizar_pedido(recibido);
			free(recibido);
			enviar_entero(RTA_FINALIZAR_PEDIDO, resultado, *socket_emisor, logger);
			break;
		}

		default:
			log_error(logger, "El tipo de mensaje %s no es admitido por COMANDA", get_nombre_mensaje(tipo_mensaje));
			break;

	}
	close(*socket_emisor);
	free(socket_emisor);
}

void esperar_cliente(int socket_servidor){
	pthread_t hilo_conexion;

	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int* new_socket = malloc(sizeof(int));

	*new_socket = accept(socket_servidor, (void*) &dir_cliente, (void*) &tam_direccion);

	pthread_create(&hilo_conexion, NULL, (void*)connection_handler, new_socket);
	pthread_detach(hilo_conexion);
}

void escuchar_conexiones_comanda(){

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP_COMANDA, config_get_string_value(config, "PUERTO_ESCUCHA"), &hints, &servinfo);

	for (p=servinfo; p != NULL; p = p->ai_next)
	{
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_servidor);
			continue;
		}
		break;
	}

	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);

	log_trace(logger, "SERVIDOR | Escuchando conexiones");

	while(1)
		esperar_cliente(socket_servidor);
}
//
//void escuchar__conexiones_comanda(){
//	/*Abro socket*/
//	socket_comanda = definirSocket(logger);
//	/*Bind & Listen*/
//	if (bindearSocketYEscuchar(socket_comanda, IP_COMANDA , atoi(config_get_string_value(config, "PUERTO_ESCUCHA")),
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
