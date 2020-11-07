/*
 * Conexion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Conexion.h"

void iniciar_conexiones_app(void);


void escuchar_app(){
	while(1){

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_envio, logger);
		if (tipo_mensaje == -1){
			log_warning(logger, "Se desconecto del proceso App");
			puts("Se desconecto del proceso App");
			close(socket_envio);
			pthread_exit(NULL);
		}

		log_info(logger, "[From App] Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

		switch(tipo_mensaje){

			case CONSULTAR_PLATOS:{
				char* nombre_restaurante = recibir_consultar_platos(socket_envio, logger);//No importa este nombre
				log_info(logger, "[CONSULTAR_PLATOS]Nombre restaurante: %s", nombre_restaurante);//TODO:BORRAR XQ NO IMPORTA ESTE PARAMETRO
				free(nombre_restaurante);
				//t_rta_consultar_platos* respuesta = procesar_consultar_platos();//Debe reenviar el msj a sindicato
				//enviar_rta_consultar_platos(respuesta, socket_envio, logger);
				//free(respuesta);
			}
			break;
			case CREAR_PEDIDO:{
				recibir_mensaje_vacio(socket_envio, logger);
				//uint32_t id_pedido = procesar_crear_pedido();
				//enviar_entero(RTA_CREAR_PEDIDO, id_pedido, socket_envio, logger);
			}
			break;
			case ANADIR_PLATO:{
				t_anadir_plato* msg_anadir_plato = recibir_anadir_plato(socket_envio, logger);
				log_info(logger, "[ANADIR_PLATO]Plato: %s, ID_Pedido: %d", msg_anadir_plato->plato, msg_anadir_plato->id_pedido);
				//uint32_t resultado = procesar_anadir_plato(msg_anadir_plato);
				free(msg_anadir_plato);
				//enviar_entero(RTA_ANADIR_PLATO, resultado, socket_envio, logger);
			}
			break;
			case CONFIRMAR_PEDIDO:{
				t_confirmar_pedido* msg_confirmar_pedido = recibir_confirmar_pedido(socket_envio, logger);
				log_info(logger, "[CONFIRMAR_PEDIDO]ID_Pedido: %d", msg_confirmar_pedido->id_pedido);
				//uint32_t resultado = procesar_confirmar_pedido(msg_confirmar_pedido);
				free(msg_confirmar_pedido);
				//enviar_entero(RTA_CONFIRMAR_PEDIDO, resultado, socket_envio, logger);
			}
			break;
			case CONSULTAR_PEDIDO:{
				uint32_t id_pedido = recibir_entero(socket_envio, logger);
				log_info(logger, "[CONSULTAR_PEDIDO]ID_Pedido: %d", id_pedido);
				//t_rta_consultar_pedido* respuesta = procesar_consultar_pedido(id_pedido);
				//enviar_rta_consultar_pedido(respuesta, socket_envio, logger);
				//free(respuesta);
			}
			break;
			default:
				puts("No se reconoce el tipo de mensaje recibido");
				break;
		}
	}
}

void connection_handler_cliente(int* socket_emisor){

	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*socket_emisor, logger);

	if (tipo_mensaje == -1){
		close(*socket_emisor);
		free(socket_emisor);
		pthread_exit(NULL);
	}

	log_info(logger, "Se recibe tipo de mensaje %s desde un nuevo cliente.", get_nombre_mensaje(tipo_mensaje));

	switch (tipo_mensaje){

		case HANDSHAKE:{
			recibir_mensaje_vacio(*socket_emisor, logger);
			uint32_t miTipoProceso = RESTAURANTE;
			enviar_entero(RTA_HANDSHAKE, miTipoProceso, *socket_emisor, logger);
			close(*socket_emisor);
			free(socket_emisor);
			break;
		}
		case SOCKET_ENVIO:{
			t_socket_envio* msg_socket_envio = recibir_socket_envio(*socket_emisor, logger);
			procesar_socket_envio(msg_socket_envio, socket_emisor);
			free(msg_socket_envio);
			break;
		}
		case SOCKET_ESCUCHA:{
			t_socket_escucha* msg_socket_escucha = recibir_socket_escucha(*socket_emisor, logger);
			procesar_socket_escucha(msg_socket_escucha, socket_emisor);
			free(msg_socket_escucha);
			break;
		}
		default:
			log_error(logger, "Tipo de mensaje no admitido.");
			close(*socket_emisor);
			free(socket_emisor);
			break;
	}
}

void iniciar_conexiones_app(){

	socket_envio = crear_conexion(config_get_string_value(config, "IP_APP"), config_get_string_value(config, "PUERTO_APP"));
	if (socket_envio != -1){
		t_socket_envio* param_socket_envio = calloc(1,sizeof(t_socket_envio));
		strcpy(param_socket_envio->id, restaurante_conf.nombre_restaurante);
		param_socket_envio->posX = metadata_restaurante->pos_x;
		param_socket_envio->posY = metadata_restaurante->pos_y;
		param_socket_envio->tipoProceso = RESTAURANTE;
		enviar_socket_envio(param_socket_envio, socket_envio, logger);
		free(param_socket_envio);

		socket_escucha = crear_conexion(config_get_string_value(config, "IP_APP"), config_get_string_value(config, "PUERTO_APP"));

		if (socket_escucha != -1){
			t_socket_escucha* socket_escucha_info = calloc(1,sizeof(t_socket_escucha));
			strcpy(socket_escucha_info->id, restaurante_conf.nombre_restaurante);
			socket_escucha_info->tipoProceso = RESTAURANTE;
			enviar_socket_escucha(socket_escucha_info, socket_escucha, logger);
			free(socket_escucha_info);
		}
		pthread_create(&hilo_escucha_app, NULL, (void*)escuchar_app, NULL);
		pthread_detach(hilo_escucha_app);
	}
}

void conectar_a_app(){

	int socket_handshake = crear_conexion(config_get_string_value(config, "IP_APP"), config_get_string_value(config, "PUERTO_APP"));
	if (socket_handshake == -1)
		log_warning(logger, "No se pudo conectar a App");
	else{
		enviar_mensaje_vacio(HANDSHAKE, socket_handshake, logger);
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake, logger);
		if(tipo_mensaje == RTA_HANDSHAKE){
			uint32_t respuesta_entero = recibir_entero(socket_handshake, logger);
			if (respuesta_entero == APP){
				log_info(logger, "Fin Handshake con App");
				iniciar_conexiones_app();
			}
			else
				log_warning(logger, "[Handshake] el proceso que respondio no es App");
		}
		close(socket_handshake);
	}
}

void obtener_restaurante(){
	int socket_new = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_new == -1)
		log_warning(logger, "[Obtener Restaurante] No se pudo conectar a Sindicato");
	else{
		enviar_obtener_restaurante(restaurante_conf.nombre_restaurante, socket_new, logger);
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_new, logger);
		if(tipo_mensaje == RTA_OBTENER_RESTAURANTE){
			t_rta_obtener_restaurante* info_restaurante = recibir_rta_obtener_restaurante(socket_new, logger);
			metadata_restaurante = malloc(sizeof(t_metadata_restaurante));
			metadata_restaurante->pos_x = info_restaurante->posX;
			metadata_restaurante->pos_y = info_restaurante->posY;
			metadata_restaurante->cantidad_hornos = info_restaurante->cantHornos;
			metadata_restaurante->cantidad_pedidos = info_restaurante->cantPedidos;
			metadata_restaurante->cocineros = info_restaurante->cocineros;
			metadata_restaurante->platos = info_restaurante->platos;
			log_info(logger, "[Obtener Restaurante] Se obtuvo la metadata desde Sindicato");
			log_info(logger,"POS_X:%d",metadata_restaurante->pos_x);
			log_info(logger,"POS_Y:%d",metadata_restaurante->pos_y);
			log_info(logger,"CANTIDAD_HORNOS:%d",metadata_restaurante->cantidad_hornos);
			log_info(logger,"CANTIDAD_PEDIDOS:%d",metadata_restaurante->cantidad_pedidos);
			imprimir_lista_strings(metadata_restaurante->cocineros,"AFINIDAD_COCINEROS");
			log_info(logger,"PLATOS:");
			void imprimir_plato(t_plato* plato){
				log_info(logger,"NOMBRE_PLATO:%s",plato->nombre);
				log_info(logger,"PRECIO_PLATO:%d",plato->precio);
			}
			list_iterate(metadata_restaurante->platos,(void*)imprimir_plato);

			free(info_restaurante);
			close(socket_new);
		}
	}
}

void conectar_a_sindicato(){

	int socket_handshake = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_handshake == -1)
		log_warning(logger, "No se pudo conectar a Sindicato");
	else{
		enviar_mensaje_vacio(HANDSHAKE, socket_handshake, logger);
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake, logger);
		if(tipo_mensaje == RTA_HANDSHAKE){
			uint32_t respuesta_entero = recibir_entero(socket_handshake, logger);
			if (respuesta_entero == SINDICATO){
				log_info(logger, "Fin Handshake con Sindicato");
				obtener_restaurante();
			}
			else
				log_warning(logger, "[Handshake] el proceso que respondio no es Sindicato");
		}
		close(socket_handshake);
	}
}

void esperar_cliente(int socket_servidor){
	pthread_t hilo_conexion;

	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int* new_socket = malloc(sizeof(int));

	*new_socket = accept(socket_servidor, (void*) &dir_cliente, (void*) &tam_direccion);

	pthread_create(&hilo_conexion, NULL, (void*)connection_handler_cliente, new_socket);
	pthread_detach(hilo_conexion);
}

void escuchar_clientes(){

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP_RESTAURANTE, config_get_string_value(config, "PUERTO_ESCUCHA"), &hints, &servinfo);

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

	log_info(logger, "SERVIDOR | Escuchando clientes");
	clientes_conectados = list_create();//INICALIZO LA LISTA DE CLIENTES

	while(1)
		esperar_cliente(socket_servidor);
}
