/*
 * Conexion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Conexion.h"

void guardar_pedido_app(uint32_t* id_pedido){
	pthread_mutex_lock(&mutex_pedidos_app);
	list_add(lista_pedidos_app, id_pedido);
	pthread_mutex_unlock(&mutex_pedidos_app);
}
void escuchar_app(){
	while(1){

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_envio, logger);
		if (tipo_mensaje == -1){
			log_warning(logger, "Se desconecto del proceso App");
			puts("Se desconecto del proceso App");
			close(socket_envio);
			close(socket_escucha);
			pthread_exit(NULL);
		}

		log_info(logger, "[From App] Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

		switch(tipo_mensaje){

			case CONSULTAR_PLATOS:{
				char* nombre_restaurante = recibir_consultar_platos(socket_envio, logger);//No importa este nombre
				free(nombre_restaurante);
				t_rta_consultar_platos* respuesta = procesar_consultar_platos();//Debe reenviar el msj a sindicato
				enviar_rta_consultar_platos(respuesta, socket_envio, logger);
				list_destroy_and_destroy_elements(respuesta->platos, free);
				free(respuesta);
			}
			break;
			case CREAR_PEDIDO:{
				recibir_mensaje_vacio(socket_envio, logger);
				uint32_t* id_pedido = malloc(sizeof(uint32_t));
				*id_pedido = procesar_crear_pedido();
				guardar_pedido_app(id_pedido);
				enviar_entero(RTA_CREAR_PEDIDO, *id_pedido, socket_envio, logger);
			}
			break;
			case ANADIR_PLATO:{
				t_anadir_plato* msg_anadir_plato = recibir_anadir_plato(socket_envio, logger);
				log_info(logger, "[ANADIR_PLATO]Plato: %s, ID_Pedido: %d", msg_anadir_plato->plato, msg_anadir_plato->id_pedido);
				uint32_t resultado = procesar_anadir_plato(msg_anadir_plato);
				free(msg_anadir_plato);
				enviar_entero(RTA_ANADIR_PLATO, resultado, socket_envio, logger);
			}
			break;
			case CONFIRMAR_PEDIDO:{
				t_confirmar_pedido* msg_confirmar_pedido = recibir_confirmar_pedido(socket_envio, logger);
				log_info(logger, "[CONFIRMAR_PEDIDO]ID_Pedido: %d", msg_confirmar_pedido->id_pedido);
				uint32_t resultado = procesar_confirmar_pedido(msg_confirmar_pedido->id_pedido);
				free(msg_confirmar_pedido);
				enviar_entero(RTA_CONFIRMAR_PEDIDO, resultado, socket_envio, logger);
			}
			break;
			case CONSULTAR_PEDIDO:{
				uint32_t id_pedido = recibir_entero(socket_envio, logger);
				log_info(logger, "[CONSULTAR_PEDIDO]ID_Pedido: %d", id_pedido);
				t_rta_consultar_pedido* respuesta = procesar_consultar_pedido(id_pedido);
				enviar_rta_consultar_pedido(respuesta, socket_envio, logger);
				list_destroy_and_destroy_elements(respuesta->comidas, free);
				free(respuesta);
			}
			break;
			default:
				puts("No se reconoce el tipo de mensaje recibido");
				break;
		}
	}
}
void guardar_pedido_cliente(char id_cliente[L_ID],uint32_t id_pedido){
	bool es_cliente(t_cliente* cliente){
		return string_equals_ignore_case(cliente->nombre,id_cliente);
	}
	pthread_mutex_lock(&mutex_cliente_conectados);
	t_cliente* cliente_buscado = list_find(clientes_conectados,(void*)es_cliente);
	pthread_mutex_unlock(&mutex_cliente_conectados);

	uint32_t* id_pedido_aux = malloc(sizeof(uint32_t));
	*id_pedido_aux = id_pedido;
	pthread_mutex_lock(&cliente_buscado->mutex_pedidos);
	list_add(cliente_buscado->pedidos,id_pedido_aux);
	pthread_mutex_unlock(&cliente_buscado->mutex_pedidos);
}
void escuchar_cliente_existente(int socket_cliente, t_handshake* cliente){

	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_cliente, logger);

	log_info(logger, "Se recibe tipo de mensaje %s desde el cliente %s.", get_nombre_mensaje(tipo_mensaje), cliente->id);

	switch(tipo_mensaje){

		case CONSULTAR_PLATOS:{
			char* nombre_restaurante = recibir_consultar_platos(socket_cliente, logger);//No importa este nombre
			free(nombre_restaurante);
			t_rta_consultar_platos* respuesta = procesar_consultar_platos();//Debe reenviar el msj a sindicato
			enviar_rta_consultar_platos(respuesta, socket_cliente, logger);
			list_destroy_and_destroy_elements(respuesta->platos, free);
			free(respuesta);
		}
		break;
		case CREAR_PEDIDO:{
			recibir_mensaje_vacio(socket_cliente, logger);
			uint32_t id_pedido = procesar_crear_pedido();
			guardar_pedido_cliente(cliente->id,id_pedido);
			enviar_entero(RTA_CREAR_PEDIDO, id_pedido, socket_cliente, logger);
		}
		break;
		case ANADIR_PLATO:{
			t_anadir_plato* msg_anadir_plato = recibir_anadir_plato(socket_cliente, logger);
			log_info(logger, "[ANADIR_PLATO]Plato: %s, ID_Pedido: %d", msg_anadir_plato->plato, msg_anadir_plato->id_pedido);
			uint32_t resultado = procesar_anadir_plato(msg_anadir_plato);
			free(msg_anadir_plato);
			enviar_entero(RTA_ANADIR_PLATO, resultado, socket_cliente, logger);
		}
		break;
		case CONFIRMAR_PEDIDO:{
			t_confirmar_pedido* msg_confirmar_pedido = recibir_confirmar_pedido(socket_cliente, logger);
			log_info(logger, "[CONFIRMAR_PEDIDO]ID_Pedido: %d", msg_confirmar_pedido->id_pedido);
			uint32_t resultado = procesar_confirmar_pedido(msg_confirmar_pedido->id_pedido);
			free(msg_confirmar_pedido);
			enviar_entero(RTA_CONFIRMAR_PEDIDO, resultado, socket_cliente, logger);
		}
		break;
		case CONSULTAR_PEDIDO:{
			uint32_t id_pedido = recibir_entero(socket_cliente, logger);
			log_info(logger, "[CONSULTAR_PEDIDO]ID_Pedido: %d", id_pedido);
			t_rta_consultar_pedido* respuesta = procesar_consultar_pedido(id_pedido);
			enviar_rta_consultar_pedido(respuesta, socket_cliente, logger);
			list_destroy_and_destroy_elements(respuesta->comidas, free);
			free(respuesta);
		}
		break;
		default:
			log_error(logger, "Tipo de mensaje no admitido.");
			break;
	}
}

void connection_handler_cliente(int* socket_emisor){

	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*socket_emisor, logger);

	if (tipo_mensaje == -1){
		close(*socket_emisor);
		free(socket_emisor);
		pthread_exit(NULL);
	}

	log_info(logger, "Se recibe tipo de mensaje %s desde un cliente.", get_nombre_mensaje(tipo_mensaje));

		switch(tipo_mensaje){

			case HANDSHAKE_INICIAL:{
				t_handshake_inicial* handshake_inicial = recibir_handshake_inicial(*socket_emisor, logger);
				procesar_handshake_inicial(handshake_inicial, *socket_emisor);
				free(handshake_inicial);
				uint32_t miTipoProceso = RESTAURANTE;
				enviar_entero(RTA_HANDSHAKE, miTipoProceso, *socket_emisor, logger);
				break;
			}
			case HANDSHAKE:{
				t_handshake* handshake = recibir_handshake(*socket_emisor, logger);
				escuchar_cliente_existente(*socket_emisor, handshake);
				free(handshake);
				close(*socket_emisor);
				free(socket_emisor);
				break;
			}
			default:
				log_error(logger, "Tipo de mensaje no admitido.");
				close(*socket_emisor);
				free(socket_emisor);
				break;
		}
}

void crear_socket_envio(){
	socket_envio = -1;//Para saber si hay que cerrarlo

	int socket_envio_aux = crear_conexion(config_get_string_value(config, "IP_APP"), config_get_string_value(config, "PUERTO_APP"));
	if (socket_envio_aux == -1)
		log_warning(logger, "No se pudo conectar a App para el socket de envio y respuesta.");
	else{
		/*Realizo el handshake para el socket de envio y respuesta*/
		t_handshake* handshake = calloc(1,sizeof(t_handshake));
		strcpy(handshake->id, restaurante_conf.nombre_restaurante);
		handshake->tipoProceso = RESTAURANTE;
		enviar_handshake(handshake, socket_envio_aux, logger);
		free(handshake);
		socket_envio = socket_envio_aux;//EL SOCKET PARA QUE APP ME ENVIE Y YO LE CONTESTE, ES ESTE
		/*Creo el hilo para escuchar a app*/
		lista_pedidos_app = list_create();
		pthread_mutex_init(&mutex_pedidos_app,NULL);

		pthread_create(&hilo_escucha_app, NULL, (void*)escuchar_app, NULL);
		pthread_detach(hilo_escucha_app);
	}
}

void conectar_a_app(){
	socket_escucha = -1;//Para saber si hay que cerrarlo

	int socket_handshake = crear_conexion(config_get_string_value(config, "IP_APP"), config_get_string_value(config, "PUERTO_APP"));
	if (socket_handshake == -1)
		log_warning(logger, "No se pudo conectar a App para el socket de actualizaciones.");
	else{
		/*Realizo el handshake inicial (Para el socket de actualizaciones)*/
		t_handshake_inicial* handshake_inicial = calloc(1,sizeof(t_handshake_inicial));
		strcpy(handshake_inicial->id, restaurante_conf.nombre_restaurante);
		handshake_inicial->posX = metadata_restaurante.pos_x;
		handshake_inicial->posY = metadata_restaurante.pos_y;
		handshake_inicial->tipoProceso = RESTAURANTE;
		enviar_handshake_inicial(handshake_inicial, socket_handshake, logger);
		free(handshake_inicial);

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake, logger);
		if(tipo_mensaje == RTA_HANDSHAKE){
			uint32_t respuesta_entero = recibir_entero(socket_handshake, logger);
			if (respuesta_entero == APP){
				log_info(logger, "Se realizo el HANDSHAKE INICIAL con App.");
				socket_escucha = socket_handshake;//EL SOCKET PARA ENVIAR ACTUALIZACIONES HACIA APP
				crear_socket_envio();
			}
			else{
				log_warning(logger, "[Handshake] el proceso que respondio no es App");
				close(socket_handshake);
			}
		}
	}
}
void obtener_restaurante(){
	int socket_new = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_new == -1){
		log_error(logger, "[Obtener Restaurante] No se pudo conectar a Sindicato");
		puts("[Obtener Restaurante] No se pudo conectar a Sindicato");
		exit(-1);
	}
	else{
		enviar_obtener_restaurante(restaurante_conf.nombre_restaurante, socket_new, logger);
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_new, logger);
		if(tipo_mensaje == RTA_OBTENER_RESTAURANTE){
			t_rta_obtener_restaurante* info_restaurante = recibir_rta_obtener_restaurante(socket_new, logger);
			metadata_restaurante.pos_x = info_restaurante->posX;
			metadata_restaurante.pos_y = info_restaurante->posY;
			metadata_restaurante.cantidad_hornos = info_restaurante->cantHornos;
			id_pedidos = info_restaurante->cantPedidos;
			metadata_restaurante.afinidades_cocineros = info_restaurante->cocineros;
			metadata_restaurante.platos = info_restaurante->platos;
			log_info(logger, "[Obtener Restaurante] Se obtuvo la metadata desde Sindicato");
			log_info(logger,"[Obtener Restaurante]PosX:%d",metadata_restaurante.pos_x);
			log_info(logger,"[Obtener Restaurante]PosY:%d",metadata_restaurante.pos_y);
			log_info(logger,"[Obtener Restaurante]Cant. Hornos:%d",metadata_restaurante.cantidad_hornos);
			log_info(logger,"[Obtener Restaurante]Cant. Pedidos:%d",id_pedidos);
			log_info(logger,"[Obtener Restaurante]Afinidades:");
			void imprimir_afinidades(char* afinidad){
				log_info(logger,"\tAfinidad:%s",afinidad);
			}
			list_iterate(metadata_restaurante.afinidades_cocineros,(void*)imprimir_afinidades);
			log_info(logger,"[Obtener Restaurante]Platos:");
			void imprimir_plato(t_plato* plato){
				log_info(logger,"\tNombre Plato:%s",plato->nombre);
				log_info(logger,"\tPrecio Plato:%d",plato->precio);
			}
			list_iterate(metadata_restaurante.platos,(void*)imprimir_plato);

			free(info_restaurante);
			close(socket_new);
		}
	}
}

void conectar_a_sindicato(){
	int socket_handshake = crear_conexion(restaurante_conf.ip_sindicato, restaurante_conf.puerto_sindicato);
	if (socket_handshake == -1){
		log_error(logger, "No se pudo conectar a Sindicato");
		puts("No se pudo conectar a Sindicato.");
		exit(-1);//Termina el programa
	}
	else{
		/*Realizo el handshake inicial*/
		t_handshake_inicial* handshake_inicial = calloc(1,sizeof(t_handshake_inicial));
		strcpy(handshake_inicial->id, restaurante_conf.nombre_restaurante);
		handshake_inicial->posX = 0;//No importa este valor, y todavia no lo conozco
		handshake_inicial->posY = 0;//No importa este valor, y todavia no lo conozco
		handshake_inicial->tipoProceso = RESTAURANTE;
		enviar_handshake_inicial(handshake_inicial, socket_handshake, logger);
		free(handshake_inicial);

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake, logger);
		if(tipo_mensaje == RTA_HANDSHAKE){
			uint32_t respuesta_entero = recibir_entero(socket_handshake, logger);
			if (respuesta_entero == SINDICATO){
				log_info(logger, "Fin Handshake Inicial con Sindicato.");
				obtener_restaurante();
			}
			else{
				log_error(logger, "[Handshake] el proceso que respondio no es Sindicato");
				puts("[Handshake] el proceso que respondio no es Sindicato");
				exit(-1);//Termina el programa
			}

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
	pthread_mutex_init(&mutex_cliente_conectados,NULL);

	while(1)
		esperar_cliente(socket_servidor);
}
