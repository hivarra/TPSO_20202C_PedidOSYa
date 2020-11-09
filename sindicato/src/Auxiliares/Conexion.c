#include "Conexion.h"

void connection_handler(int* socket_emisor) {

	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*socket_emisor, logger);

	if (tipo_mensaje == -1) {
		close(*socket_emisor);
		free(socket_emisor);
		pthread_exit(NULL);
	}

	log_trace(logger, "Se recibe tipo de mensaje: %s",
			get_nombre_mensaje(tipo_mensaje));

	switch (tipo_mensaje) {

	case HANDSHAKE: {
		recibir_mensaje_vacio(*socket_emisor, logger);
		uint32_t miTipoProceso = SINDICATO;
		enviar_entero(RTA_HANDSHAKE, miTipoProceso, *socket_emisor, logger);
		break;
	}
	case CONSULTAR_PLATOS: {
		char* restaurante = recibir_consultar_platos(*socket_emisor, logger);
		log_trace(logger, "Restaurante: %s", restaurante);
		t_rta_consultar_platos* respuesta = procesar_consultar_platos(restaurante);
		free(restaurante);
		enviar_rta_consultar_platos(respuesta, *socket_emisor, logger);
		list_destroy_and_destroy_elements(respuesta->platos, free);
		free(respuesta);
		break;
	}
	case GUARDAR_PEDIDO: {
		t_guardar_pedido* msg_guardar_pedido = recibir_guardar_pedido(*socket_emisor, logger);
		log_trace(logger, "Restaurante: %s, ID_Pedido: %d", msg_guardar_pedido->restaurante, msg_guardar_pedido->id_pedido);
		uint32_t resultado_guardar_pedido = procesar_guardar_pedido(msg_guardar_pedido);
		free(msg_guardar_pedido);
		enviar_entero(RTA_GUARDAR_PEDIDO, resultado_guardar_pedido, *socket_emisor, logger);
		break;
	}
	case GUARDAR_PLATO: {
		t_guardar_plato* recibido = recibir_guardar_plato(*socket_emisor, logger);
		log_trace(logger, "Restaurante: %s, ID_Pedido: %d, Plato: %s, Cant: %d", recibido->restaurante, recibido->id_pedido, recibido->plato, recibido->cantPlato);
		uint32_t resultado_guardar_plato = procesar_guardar_plato(recibido);
		free(recibido);
		enviar_entero(RTA_GUARDAR_PLATO, resultado_guardar_plato, *socket_emisor, logger);
		break;
	}
	case CONFIRMAR_PEDIDO: {
		t_confirmar_pedido* recibido = recibir_confirmar_pedido(*socket_emisor, logger);
		log_trace(logger, "Restaurante: %s, ID_Pedido: %d", recibido->restaurante, recibido->id_pedido);
		uint32_t resultado = procesar_confirmar_pedido(recibido);
		free(recibido);
		enviar_entero(RTA_CONFIRMAR_PEDIDO, resultado, *socket_emisor, logger);
		break;
	}
	case OBTENER_PEDIDO: {
		t_obtener_pedido* recibido = recibir_obtener_pedido(*socket_emisor, logger);
		log_trace(logger, "Restaurante: %s, ID_Pedido: %d", recibido->restaurante, recibido->id_pedido);
		t_rta_obtener_pedido* respuesta = procesar_obtener_pedido(recibido);
		free(recibido);
		enviar_rta_obtener_pedido(respuesta, *socket_emisor, logger);
		list_destroy_and_destroy_elements(respuesta->comidas, free);
		free(respuesta);
		break;
	}
	case OBTENER_RESTAURANTE: {
		char* restaurante = recibir_obtener_restaurante(*socket_emisor, logger);
		log_trace(logger, "Restaurante: %s", restaurante);
		t_rta_obtener_restaurante* respuesta = procesar_obtener_restaurante(restaurante);
		free(restaurante);
		enviar_rta_obtener_restaurante(respuesta, *socket_emisor, logger);
		list_destroy_and_destroy_elements(respuesta->cocineros, free);
		list_destroy_and_destroy_elements(respuesta->platos, free);
		free(respuesta);
		break;
	}
	case PLATO_LISTO: {
		t_plato_listo* recibido = recibir_plato_listo(*socket_emisor, logger);
		log_trace(logger, "Restaurante: %s, ID_Pedido: %d, Plato: %s", recibido->restaurante, recibido->id_pedido, recibido->plato);
		uint32_t resultado = procesar_plato_listo(recibido);
		free(recibido);
		enviar_entero(RTA_PLATO_LISTO, resultado, *socket_emisor, logger);
		break;
	}
	case OBTENER_RECETA: {
		char* plato = recibir_obtener_receta(*socket_emisor, logger);
		log_trace(logger, "Plato: %s", plato);
		t_rta_obtener_receta* respuesta = procesar_obtener_receta(plato);
		free(plato);
		enviar_rta_obtener_receta(respuesta, *socket_emisor, logger);
		list_destroy_and_destroy_elements(respuesta->pasos, free);
		free(respuesta);
		break;
	}
	case TERMINAR_PEDIDO: {
		t_terminar_pedido* recibido = recibir_terminar_pedido(*socket_emisor, logger);
		log_trace(logger, "Restaurante: %s, ID_Pedido: %d", recibido->restaurante, recibido->id_pedido);
		uint32_t resultado = procesar_terminar_pedido(recibido);
		free(recibido);
		enviar_entero(RTA_TERMINAR_PEDIDO, resultado, *socket_emisor, logger);
		break;
	}
	default:
		log_error(logger, "El tipo de mensaje %s no es admitido por SINDICATO.", get_nombre_mensaje(tipo_mensaje));
		break;
	}
	close(*socket_emisor);
	free(socket_emisor);
}

void esperar_cliente(int socket_server) {
	pthread_t hilo_conexion;

	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int* new_socket = malloc(sizeof(int));

	*new_socket = accept(socket_server, (void*) &dir_cliente,
			(void*) &tam_direccion);

	pthread_create(&hilo_conexion, NULL, (void*) connection_handler,
			new_socket);
	pthread_detach(hilo_conexion);
}

void escuchar_conexiones_sindicato() {

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP_SINDICATO, config_get_string_value(config, "PUERTO_ESCUCHA"),
			&hints, &servinfo);

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1)
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

	while (1)
		esperar_cliente(socket_servidor);
}
