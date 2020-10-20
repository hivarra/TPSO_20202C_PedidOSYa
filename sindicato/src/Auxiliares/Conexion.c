#include "Conexion.h"

void connection_handler(int* socket_emisor) {

	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*socket_emisor, logger);

	if (tipo_mensaje == -1) {
		close(*socket_emisor);
		free(socket_emisor);
		pthread_exit(NULL);
	}

	log_info(logger, "Se recibe tipo de mensaje: %s",
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
		log_info(logger, "Restaurante: %s", restaurante);
		free(restaurante);
		break;
	}

	case GUARDAR_PEDIDO: {

		t_guardar_pedido* pedido = recibir_guardar_pedido(*socket_emisor, logger);
		free(pedido);

		break;
	}

	case GUARDAR_PLATO: {

		t_guardar_plato* recibido = recibir_guardar_plato(*socket_emisor,
				logger);
		free(recibido);
		break;
	}

	case CONFIRMAR_PEDIDO: {

		t_confirmar_pedido* recibido = recibir_confirmar_pedido(*socket_emisor,
				logger);
		free(recibido);
		break;
	}

	case OBTENER_PEDIDO: {
		t_obtener_pedido* recibido = recibir_obtener_pedido(*socket_emisor,
				logger);
		free(recibido);
		break;
	}

	case OBTENER_RESTAURANTE: {

		char* restaurante = recibir_obtener_restaurante(*socket_emisor, logger);
		free(restaurante);

		break;
	}

	case PLATO_LISTO: {

		t_plato_listo* recibido = recibir_plato_listo(*socket_emisor, logger);
		free(recibido);
		break;
	}

	case OBTENER_RECETA: {
		char* plato = recibir_terminar_pedido(*socket_emisor, logger);
		free(plato);
		break;
	}
	case TERMINAR_PEDIDO: {

		t_terminar_pedido* recibido = recibir_terminar_pedido(*socket_emisor,
				logger);
		free(recibido);
		break;
	}

	default:
		log_error(logger, "El tipo de mensaje %s no es admitido por COMANDA",
				get_nombre_mensaje(tipo_mensaje));
		break;

	}
	close(*socket_emisor);
	free(socket_emisor);
}

void esperar_cliente(int socket_servidor) {
	pthread_t hilo_conexion;

	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int* new_socket = malloc(sizeof(int));

	*new_socket = accept(socket_servidor, (void*) &dir_cliente,
			(void*) &tam_direccion);

	pthread_create(&hilo_conexion, NULL, (void*) connection_handler,
			new_socket);
	pthread_detach(hilo_conexion);
}

void escuchar_conexiones_sindicato() {

	int socket_servidor;

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

	log_info(logger, "SERVIDOR | Escuchando conexiones");

	while (1)
		esperar_cliente(socket_servidor);
}

void escuchar__conexiones_sindicato() {
	/*Abro socket*/
	socket_sindicato = definirSocket(logger);
	/*Bind & Listen*/
	if (bindearSocketYEscuchar(socket_sindicato, IP_SINDICATO,
			atoi(config_get_string_value(config, "PUERTO_ESCUCHA")), logger)
			<= 0)
		_exit_with_error("BIND", NULL, logger);

	/*Atiendo solicitudes creando un hilo para cada una*/
	while (1) {
		pthread_t hilo_conexion;
		int* new_sock = malloc(sizeof(int));

		*new_sock = aceptarConexiones(socket_sindicato, logger);

		if (new_sock < 0)
			log_error(logger, "Error al realizar ACCEPT.\n");

		pthread_create(&hilo_conexion, NULL, (void*) connection_handler,
				new_sock);
		pthread_detach(hilo_conexion);
	}
}
