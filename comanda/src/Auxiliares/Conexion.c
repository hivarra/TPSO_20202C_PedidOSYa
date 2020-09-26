/*
 * Conexion.c
 *
 *  Created on: 1 sep. 2020
 *      Author: utnso
 */

#include "Conexion.h"

t_tipoMensaje recibir_tipo_mensaje(int socket_cliente) {
	t_tipoMensaje tipo_mensaje;

	if (recv(socket_cliente, &tipo_mensaje, sizeof(t_tipoMensaje), MSG_WAITALL) < 0) {
		log_error(logger, "Error al recibir tipo de mensaje de socket:%d", socket_cliente);
	}
	return tipo_mensaje;
}
u_int32_t recibir_int(int socket_cliente) {
	u_int32_t integer;

	if (recv(socket_cliente, &integer, sizeof(u_int32_t), MSG_WAITALL) < 0) {
		log_error(logger, "Error al recibir integer de socket:%d", socket_cliente);
	}
	return integer;
}
void procesar_mensajes(int id_cliente, int socket_cliente_envio){
	t_cliente_info* cliente_info = buscar_cliente(id_cliente);
	int socket_envio_rta;
	if(cliente_info != NULL)
		socket_envio_rta = cliente_info->socket;
	else
		socket_envio_rta = socket_cliente_envio;

	while(1){
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_cliente_envio);
		log_info(logger, "Se recibe tipo de mensaje:%s", get_nombre_mensaje(tipo_mensaje));

		switch (tipo_mensaje) {

		case CONSULTAR_RESTAURANTES:
			;
			t_list* lista_aux_restos = list_create();
			char* nombre_resto_1 = "RESTO_1";
			char* nombre_resto_2 = "RESTO_2";
			list_add(lista_aux_restos, nombre_resto_1);
			list_add(lista_aux_restos, nombre_resto_2);
			log_info(logger, "LLEGO ACA");
			t_rta_consultar_restaurantes* rta_consultar_rtes = malloc(sizeof(t_rta_consultar_restaurantes));
			rta_consultar_rtes->cantRestaurantes = 2;
			rta_consultar_rtes->restaurantes = lista_aux_restos;
			enviarRespuesta(RTA_CONSULTAR_RESTAURANTES,rta_consultar_rtes,socket_envio_rta,logger);
			free(rta_consultar_rtes);

			break;

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
			;
			break;
		}
	}
}
void connection_handler(int* socket_emisor){
	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*socket_emisor);
	log_info(logger, "Se recibe tipo de mensaje:%s", get_nombre_mensaje(tipo_mensaje));

	switch (tipo_mensaje) {

	case SOCKET_ESCUCHA:
		;
		int id_cliente_escucha = recibir_int(*socket_emisor);
		t_cliente_info* cliente_info = crear_cliente_info(id_cliente_escucha, *socket_emisor);
		agregar_a_lista_clientes(cliente_info);
		log_info(logger,
				"Se agregó cliente con ID_CLIENTE:%d Y SOCKET_ESCUCHA:%d a lista_clientes",
				id_cliente_escucha, *socket_emisor);
		break;

	case SOCKET_ENVIO:
		;
		int id_cliente_envio = recibir_int(*socket_emisor);
		log_info(logger, "Se recibe mensaje SOCKET_ENVIO de SOCKET:%d", *socket_emisor);

		procesar_mensajes(id_cliente_envio, *socket_emisor);

		break;

//	case CONSULTAR_RESTAURANTES:
//			;
//			t_list* lista_aux_restos = list_create();
//			char* nombre_resto_1 = "RESTO_1";
//			char* nombre_resto_2 = "RESTO_2";
//			list_add(lista_aux_restos, nombre_resto_1);
//			list_add(lista_aux_restos, nombre_resto_2);
//			log_info(logger, "LLEGO ACA");
//			t_rta_consultar_restaurantes* rta_consultar_rtes = malloc(sizeof(t_rta_consultar_restaurantes));
//			rta_consultar_rtes->cantRestaurantes = 2;
//			rta_consultar_rtes->restaurantes = lista_aux_restos;
//			enviarRespuesta(RTA_CONSULTAR_RESTAURANTES,rta_consultar_rtes,*socket_emisor,logger);
//			free(rta_consultar_rtes);
//
//			break;

	default:
		break;
//		log_error(logger, "Mensaje no reconocido: %s",
//					get_nombre_mensaje( msg->header.tipoMensaje));
	}
}

void esperar_cliente(int socket_servidor){
	pthread_t hilo_conexion;

	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, (void*) &tam_direccion);

	pthread_create(&hilo_conexion, NULL, (void*)connection_handler, &socket_cliente);
	pthread_detach(hilo_conexion);
}

//void escuchar_conexiones(){
//
//	int socket_servidor;
//
//	struct addrinfo hints, *servinfo, *p;
//
//	memset(&hints, 0, sizeof(hints));
//	hints.ai_family = AF_UNSPEC;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_flags = AI_PASSIVE;
//
//	getaddrinfo(IP_COMANDA, comanda_conf.puerto_comanda, &hints, &servinfo);
//
//	for (p=servinfo; p != NULL; p = p->ai_next)
//	{
//		if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
//			log_error(logger, "No se pudo abrir el socket de escucha");
//			continue;
//
//		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
//			log_error(logger, "No se pudo bindear el socket de escucha");
//			close(socket_servidor);
//			continue;
//		}
//		break;
//	}
//
//	listen(socket_servidor, SOMAXCONN);
//
//	freeaddrinfo(servinfo);
//
//	log_info(logger, "SERVIDOR | Escuchando conexiones");
//
//	while(1)
//		esperar_cliente(socket_servidor);
//}

void *escuchar_conexiones(){
	/*Abro socket*/
	socket_comanda = definirSocket(logger);
	/*Bind & Listen*/
	if (bindearSocketYEscuchar(socket_comanda, IP_COMANDA , atoi(comanda_conf.puerto_comanda),
			logger) <= 0)
		_exit_with_error("BIND", NULL, logger);

	lista_clientes = list_create();

	/*Atiendo solicitudes creando un hilo para cada una*/
	while(1){
		pthread_t hilo_conexion;
		int* new_sock = malloc(sizeof(int));

		*new_sock = aceptarConexiones(socket_comanda, logger);

		if(new_sock < 0)
			log_error(logger, "Error al realizar ACCEPT.\n");

		pthread_create(&hilo_conexion,NULL,(void*)connection_handler,new_sock);
		pthread_detach(hilo_conexion);
	}
}
//void agregar_cliente(t_mensaje* msg,int socket){
//	t_cliente_info* cliente_info = guardar_info_cliente(msg, socket);
//	agregar_a_lista_clientes(cliente_info);
//}
//t_cliente_info* guardar_info_cliente(t_mensaje* msg,int socket){
//	u_int32_t id_cliente = msg->header.idProceso;
//	return crear_cliente_info(id_cliente, socket);
//}
t_cliente_info* crear_cliente_info(int id_cliente, int socket){
	t_cliente_info* cliente = malloc(sizeof(t_cliente_info));
	cliente->id_cliente = id_cliente;
	cliente->socket = socket;
	return cliente;
}
void agregar_a_lista_clientes(t_cliente_info* cliente_info){
	list_add(lista_clientes, cliente_info);
}
t_cliente_info* buscar_cliente(int id_cliente){
	int es_cliente(t_cliente_info* cliente){
		return cliente->id_cliente == id_cliente;
	}
	return list_find(lista_clientes, (void*)es_cliente);
}
int existe_cliente(t_cliente_info* cliente_info){
	return cliente_info != NULL;
}
