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
t_rta_consultar_restaurantes* obtenerRestaurantes(){
	t_rta_consultar_restaurantes* restaurantesConectados = malloc(sizeof(t_rta_consultar_restaurantes));

	/*FILTRO LOS RESTAURANTES DE lista_sockets_escucha*/
	int esRestaurante(t_info_cliente* info_cliente){
		return info_cliente->tipoProceso == RESTAURANTE;
	}
	t_list* lista_restaurantes_conectados = list_filter(listaClientesRestaurantes,(void*)esRestaurante);

	void* obtenerIdChar(t_info_cliente* info_cliente){
		return info_cliente->id;
	}
	/*MAPPING  t_info_cliente -> char[L_ID] */
	t_list* nombresRestaurantesConectados = list_map(lista_restaurantes_conectados,(void*)obtenerIdChar);
	restaurantesConectados->restaurantes = nombresRestaurantesConectados;
	restaurantesConectados->cantRestaurantes = list_size(restaurantesConectados->restaurantes);

	return restaurantesConectados;
}
int obtenerSocketEscucha(t_socket_envio* infoSocketEnvio){
	int id_cliente_es_igual(t_info_cliente* info_cliente){
		return string_equals_ignore_case(info_cliente->id,infoSocketEnvio->id);
	}
	t_info_cliente* info_cliente = list_find(listaClientesRestaurantes,(void*)id_cliente_es_igual);
	return info_cliente->socketEscucha;
}
int obtenerSocketEnvio(t_socket_envio* infoSocketEnvio){
	int id_cliente_es_igual(t_info_cliente* info_cliente){
		return string_equals_ignore_case(info_cliente->id,infoSocketEnvio->id);
	}
	t_info_cliente* info_cliente = list_find(listaClientesRestaurantes,(void*)id_cliente_es_igual);
	return info_cliente->socketEnvio;
}
void procesarMensaje(int* socketEnvio){

	while(1){
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(*socketEnvio, logger);
		log_info(logger, "Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

		switch(tipo_mensaje){
		case CONSULTAR_RESTAURANTES:;
			recibir_mensaje_vacio(*socketEnvio,logger);
			t_rta_consultar_restaurantes* rta_consultar_restaurantes = obtenerRestaurantes();
			if(rta_consultar_restaurantes == NULL)
				log_info(logger,"PRUEBAAA");
			else{
				imprimir_lista_strings(rta_consultar_restaurantes->restaurantes,"LISTA RESTAURANTES A ENVIAR");
				log_info(logger,"CANTIDAD RESTOS:%d",rta_consultar_restaurantes->cantRestaurantes);
			}

			enviar_rta_consultar_restaurantes(rta_consultar_restaurantes,*socketEnvio,logger);
			free(rta_consultar_restaurantes);

			break;
		case SELECCIONAR_RESTAURANTE:;
			char* restauranteSeleccionado = recibir_seleccionar_restaurante(*socketEnvio,logger);
			log_info(logger,"RESTAURANTE SELECCIONADO:%s",restauranteSeleccionado);
			uint32_t resultadoSeleccionar = 1;//CAMBIAR HARDCODEADO DESPUES
			enviar_entero(RTA_SELECCIONAR_RESTAURANTE,resultadoSeleccionar,*socketEnvio,logger);

			free(restauranteSeleccionado);

			break;

		case CONSULTAR_PLATOS:;
			char* restaurante = recibir_consultar_platos(*socketEnvio,logger);
			log_info(logger,"RESTAURANTE:%s CONSULTA_PLATOS",restaurante);
			t_rta_consultar_platos* rta_consultar_platos = malloc(sizeof(t_rta_consultar_platos));
			rta_consultar_platos->cantPlatos = 1;
			t_list* listaPlatos = list_create();
			t_plato* plato = malloc(sizeof(t_plato));
			strcpy(plato->nombre,"Asado");
			plato->precio = 1000;
			list_add(listaPlatos,plato);
			rta_consultar_platos->platos = listaPlatos;
			enviar_rta_consultar_platos(rta_consultar_platos,*socketEnvio,logger);

			free(restaurante);

			break;

		case CREAR_PEDIDO:;
			recibir_mensaje_vacio(*socketEnvio, logger);
			uint32_t id_pedido = 10;
			enviar_entero(RTA_CREAR_PEDIDO,id_pedido,*socketEnvio,logger);

			break;

		case ANADIR_PLATO:;
			t_anadir_plato* anadirPlato = recibir_anadir_plato(*socketEnvio,logger);
			log_info(logger,"PLATO:%s",anadirPlato->plato);
			log_info(logger,"ID_PEDIDO:%d",anadirPlato->id_pedido);
			uint32_t resultado_anadir_plato = 0;
			enviar_entero(RTA_ANADIR_PLATO,resultado_anadir_plato,*socketEnvio,logger);

			free(anadirPlato);

			break;

		case PLATO_LISTO:;
			t_plato_listo* platoListo = recibir_plato_listo(*socketEnvio,logger);
			log_info(logger,"RESTAURANTE:%s",platoListo->restaurante);
			log_info(logger,"PLATO:%s",platoListo->plato);
			log_info(logger,"ID_PEDIDO:%d",platoListo->id_pedido);
			uint32_t resultado_plato_listo = 0;
			enviar_entero(RTA_PLATO_LISTO,resultado_plato_listo,*socketEnvio,logger);

			free(platoListo);

			break;

		case CONFIRMAR_PEDIDO:;
			t_confirmar_pedido* confirmarPedido = recibir_confirmar_pedido(*socketEnvio,logger);
			log_info(logger,"RESTAURANTE:%s",confirmarPedido->restaurante);
			log_info(logger,"ID_PEDIDO:%d",confirmarPedido->id_pedido);
			uint32_t resultado_confirmar_pedido = 0;
			enviar_entero(RTA_CONFIRMAR_PEDIDO,resultado_confirmar_pedido,*socketEnvio,logger);

			free(confirmarPedido);

			break;
		case CONSULTAR_PEDIDO:;
			uint32_t idPedido = recibir_entero(*socketEnvio,logger);
			log_info(logger,"ID_PEDIDO:%d",idPedido);
			t_rta_consultar_pedido* rta_consultar_pedido = malloc(sizeof(t_rta_consultar_pedido));
			rta_consultar_pedido->cantComidas = 1;
			t_list* listaComidas = list_create();
			t_comida* comida = malloc(sizeof(t_comida));
			strcpy(comida->nombre,"polloConPapas");
			comida->cantLista = 1;
			comida->cantTotal = 1;
			list_add(listaComidas,comida);
			rta_consultar_pedido->comidas = listaComidas;
			rta_consultar_pedido->estado = CONFIRMADO;
			strcpy(rta_consultar_pedido->restaurante,"LoDeCarlitos");
			enviar_rta_consultar_pedido(rta_consultar_pedido,*socketEnvio,logger);

			free(rta_consultar_pedido);

			break;
		default:
			break;
		}
	}
}
void mostrar_lista_clientes_restaurantes(){
	void mostrarClienteRestaurante(t_info_cliente* infoCliente){
		log_info(logger,"ID:%s",infoCliente->id);
		log_info(logger,"SOCKET_ESCUCHA:%d",infoCliente->socketEscucha);
		log_info(logger,"SOCKET_ENVIO:%d",infoCliente->socketEnvio);
		log_info(logger,"TIPO_PROCESO:%d",infoCliente->tipoProceso);
		log_info(logger,"POS_X:%d",infoCliente->pos_x);
		log_info(logger,"POS_Y:%d",infoCliente->pos_y);
	}

	list_iterate(listaClientesRestaurantes,(void*)mostrarClienteRestaurante);
}
void guardarSocketEscucha(t_socket_escucha* infoSocketEscucha,int socketCliente){
	int clienteIgual(t_info_cliente* infoCliente) {
		return (string_equals_ignore_case(infoSocketEscucha->id,
				infoCliente->id) && (infoSocketEscucha->tipoProceso == infoCliente->tipoProceso));
	}
	t_info_cliente* cliente = list_find(listaClientesRestaurantes,(void*)clienteIgual);
	if(cliente != NULL){
		cliente->socketEscucha = socketCliente;
	}
	else{
		t_info_cliente* infoCliente = malloc(sizeof(t_info_cliente));
		strcpy(infoCliente->id,infoSocketEscucha->id);
		infoCliente->tipoProceso = infoSocketEscucha->tipoProceso;
		infoCliente->socketEscucha = socketCliente;
		list_add(listaClientesRestaurantes, infoCliente);
	}
//	log_info(logger,"CANTIDAD DE CLIENTES LUEGO DE GUARDAR SOCKET_ESCUCHA:%d",list_size(lista_clientes_restaurantes));
}
void guardarSocketEnvio(t_socket_envio* infoSocketEnvio,int socketEnvio) {
	int cliente_igual(t_info_cliente* info_cliente){
		return string_equals_ignore_case(infoSocketEnvio->id,info_cliente->id);
	}
	t_info_cliente* cliente = list_find(listaClientesRestaurantes,(void*)cliente_igual);
	if(cliente != NULL){
		cliente->pos_x = infoSocketEnvio->posX;
		cliente->pos_y = infoSocketEnvio->posY;
		cliente->tipoProceso = infoSocketEnvio->tipoProceso;
		cliente->socketEnvio = socketEnvio;
	}
	else{
		t_info_cliente* infoCliente = malloc(sizeof(t_info_cliente));
		strcpy(infoCliente->id, infoSocketEnvio->id);
		infoCliente->pos_x = infoSocketEnvio->posX;
		infoCliente->pos_y = infoSocketEnvio->posY;
		infoCliente->tipoProceso = infoSocketEnvio->tipoProceso;
		infoCliente->socketEnvio = socketEnvio;

		list_add(listaClientesRestaurantes, infoCliente);
	}
//	log_info(logger,"CANTIDAD LISTA_CLIENTES_RESTAURANTES DESPUES DE AGREGAR:%d",list_size(lista_clientes_restaurantes));
}
void crearHiloProcesarMensajes(int* socketCliente){
	pthread_t hiloProcesarMensajes;
	pthread_create(&hiloProcesarMensajes,NULL,(void*)procesarMensaje,socketCliente);
	pthread_detach(hiloProcesarMensajes);
}
void* atenderConexion(int* socketCliente) {

//	log_info(logger, "Hilo creado para el socket: %d", *socket_cliente);

	t_tipoMensaje tipoMensaje = recibir_tipo_mensaje(*socketCliente, logger);
	log_info(logger, "Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipoMensaje));

	switch(tipoMensaje) {

	case HANDSHAKE:
		;
		recibir_mensaje_vacio(*socketCliente, logger);
		uint32_t miTipoProceso = APP;
		enviar_entero(RTA_HANDSHAKE, miTipoProceso, *socketCliente, logger);
		close(*socketCliente);
		break;

	case SOCKET_ENVIO:
		;
		t_socket_envio* infoSocketEnvio = recibir_socket_envio(*socketCliente,logger);
		pthread_mutex_lock(&mutexClientesRestaurantes);
		guardarSocketEnvio(infoSocketEnvio,*socketCliente);
//		log_info(logger,"PRUEBA LISTA_CLIENTES_RESTAURANTES:");
//		mostrar_lista_clientes_restaurantes();
		pthread_mutex_unlock(&mutexClientesRestaurantes);

		crearHiloProcesarMensajes(socketCliente);

		free(infoSocketEnvio);

		break;

	case SOCKET_ESCUCHA:
		;
		t_socket_escucha* info_socket_escucha = recibir_socket_escucha(*socketCliente,logger);
//		log_info(logger,"ID:%s",info_socket_escucha->id);
//		log_info(logger,"TIPO_PROCESO:%d",info_socket_escucha->tipoProceso);
//		log_info(logger,"SOCKET:%d",*socketCliente);
		pthread_mutex_lock(&mutexClientesRestaurantes);
		guardarSocketEscucha(info_socket_escucha, *socketCliente);
		pthread_mutex_unlock(&mutexClientesRestaurantes);
		free(info_socket_escucha);

		break;

	default:
		;
		log_error(logger, "El tipo de mensaje %s no es admitido por APP", get_nombre_mensaje(tipoMensaje));
		break;

	}
	return NULL;
}
void incializarRestoDefault(){
	t_info_cliente* infoCliente = malloc(sizeof(t_info_cliente));
	strcpy(infoCliente->id,"RESTO_DEFAULT");
	infoCliente->tipoProceso = RESTAURANTE;
	infoCliente->pos_x = app_conf.pos_rest_default_x;
	infoCliente->pos_y = app_conf.pos_rest_default_y;
	infoCliente->socketEnvio = -1;
	infoCliente->socketEscucha = -1;
	list_add(listaClientesRestaurantes,infoCliente);
}
void inicializarListaClientesRest(){
	listaClientesRestaurantes = list_create();
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
