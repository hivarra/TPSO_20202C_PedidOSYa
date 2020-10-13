/*
 * Conexion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#include "Conexion.h"

void *crearServidor() {

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
t_rta_consultar_restaurantes* obtener_restaurantes(){
	t_rta_consultar_restaurantes* restaurantes_conectados = malloc(sizeof(t_rta_consultar_restaurantes));

	/*FILTRO LOS RESTAURANTES DE lista_sockets_escucha*/
	int es_restaurante(t_info_cliente* info_cliente){
		return info_cliente->tipoProceso == RESTAURANTE;
	}
	t_list* lista_restaurantes_conectados = list_filter(lista_clientes_restaurantes,(void*)es_restaurante);

	void* obtener_id_char(t_info_cliente* info_cliente){
		return info_cliente->id;
	}
	/*MAPPING  t_info_cliente -> char[L_ID] */
	t_list* nombres_restaurantes_conectados = list_map(lista_restaurantes_conectados,(void*)obtener_id_char);
	restaurantes_conectados->restaurantes = nombres_restaurantes_conectados;
	restaurantes_conectados->cantRestaurantes = list_size(restaurantes_conectados->restaurantes);

	return restaurantes_conectados;
}
int obtener_socket_escucha(t_socket_envio* info_socket_envio){
	int id_cliente_es_igual(t_info_cliente* info_cliente){
		return string_equals_ignore_case(info_cliente->id,info_socket_envio->id);
	}
	t_info_cliente* info_cliente = list_find(lista_clientes_restaurantes,(void*)id_cliente_es_igual);
	return info_cliente->socket;
}
void procesar_mensaje(int socket_envio,t_socket_envio* info_socket_envio){
	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_envio, logger);
	log_info(logger, "Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

	switch(tipo_mensaje){
	case CONSULTAR_RESTAURANTES:;
		recibir_mensaje_vacio(socket_envio,logger);
		int socket_escucha = obtener_socket_escucha(info_socket_envio);
		t_rta_consultar_restaurantes* rta_consultar_restaurantes = obtener_restaurantes();
		enviar_rta_consultar_restaurantes(rta_consultar_restaurantes,socket_escucha,logger);
		free(rta_consultar_restaurantes);

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
		break;
	}
}
void guardar_socket_escucha(t_socket_escucha* info_socket_escucha,int socket_cliente){
	int cliente_igual(t_info_cliente* info_cliente) {
		return string_equals_ignore_case(info_socket_escucha->id,
				info_cliente->id) && info_socket_escucha->tipoProceso == info_cliente->tipoProceso;
	}
	t_info_cliente* cliente = list_find(lista_clientes_restaurantes,(void*)cliente_igual);

	if(cliente != NULL){
		cliente->socket = socket_cliente;
	}
//	log_info(logger,"CANTIDAD DE CLIENTES LUEGO DE GUARDAR SOCKET_ESCUCHA:%d",list_size(lista_clientes_restaurantes));
}
void agregar_info_cliente(char id_cliente[L_ID], int pos_x, int pos_y,
		uint32_t tipo_proceso) {

	int cliente_igual(t_info_cliente* info_cliente){
		return string_equals_ignore_case(id_cliente,info_cliente->id);
	}

	t_info_cliente* cliente = list_find(lista_clientes_restaurantes,(void*)cliente_igual);
	if(cliente != NULL){
		cliente->pos_x = pos_x;
		cliente->pos_y = pos_y;
		cliente->tipoProceso = tipo_proceso;
	}
	else{
		t_info_cliente* info_cliente = malloc(sizeof(t_info_cliente));
		strcpy(info_cliente->id, id_cliente);
		info_cliente->pos_x = pos_x;
		info_cliente->pos_y = pos_y;
		info_cliente->tipoProceso = tipo_proceso;
		list_add(lista_clientes_restaurantes, info_cliente);
	}
	log_info(logger,"CANTIDAD LISTA_CLIENTES_RESTAURANTES DESPUES DE AGREGAR:%d",list_size(lista_clientes_restaurantes));
}
void* atenderConexion(int* socket_cliente) {

	log_info(logger, "Hilo creado para el socket: %d", *socket_cliente);

	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*socket_cliente, logger);
	log_info(logger, "Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

	switch(tipo_mensaje) {

	case HANDSHAKE:
		;
		recibir_mensaje_vacio(*socket_cliente, logger);
		uint32_t miTipoProceso = APP;
		enviar_entero(RTA_HANDSHAKE, miTipoProceso, *socket_cliente, logger);
		break;

	case SOCKET_ENVIO:
		;
		t_socket_envio* info_socket_envio = recibir_socket_envio(*socket_cliente,logger);
		agregar_info_cliente(info_socket_envio->id, info_socket_envio->posX,
				info_socket_envio->posY, info_socket_envio->tipoProceso);
		procesar_mensaje(*socket_cliente, info_socket_envio);
		free(info_socket_envio);

		break;

	case SOCKET_ESCUCHA:
		;
		t_socket_escucha* info_socket_escucha = recibir_socket_escucha(*socket_cliente,logger);
		guardar_socket_escucha(info_socket_escucha, *socket_cliente);
		free(info_socket_escucha);

		break;

	default:
		;
		log_error(logger, "El tipo de mensaje %s no es admitido por APP", get_nombre_mensaje(tipo_mensaje));
		break;

	}
	return NULL;
}
void incializar_resto_default(){
	t_info_cliente* info_cliente = malloc(sizeof(t_info_cliente));
	strcpy(info_cliente->id,"RESTO_DEFAULT");
	info_cliente->tipoProceso = RESTAURANTE;
	info_cliente->pos_x = app_conf.pos_rest_default_x;
	info_cliente->pos_y = app_conf.pos_rest_default_y;
	info_cliente->socket = -1;
	list_add(lista_clientes_restaurantes,info_cliente);
}
void inicializar_lista_clientes_rest(){
	lista_clientes_restaurantes = list_create();
}
//void imprimir_restaurante(t_restaurante* restaurante) {
//
//	log_info(logger, "Restaurante: %s | PosX: %d | PosY: %d", restaurante->nombre, restaurante->posX, restaurante->posY);
//
//}
//void agregarRestaurante(t_restaurante* restaurante) {
//
//	pthread_mutex_lock(&mutex_restaurantes);
//	list_add(restaurantes, restaurante);
//	pthread_mutex_unlock(&mutex_restaurantes);
//}
