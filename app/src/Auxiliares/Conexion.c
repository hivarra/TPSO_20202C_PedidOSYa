/*
 * Conexion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#include "Conexion.h"

void conectar_a_comanda(){
	int socket_handshake = crear_conexion(app_conf.ip_comanda, app_conf.puerto_comanda);
	if (socket_handshake == -1){
		log_error(logger, "No se pudo conectar a Comanda");
		puts("No se pudo conectar a Comanda.");
		exit(-1);//Termina el programa
	}
	else{
		/*Realizo el handshake inicial*/
		t_handshake_inicial* handshake_inicial = calloc(1,sizeof(t_handshake_inicial));
		strcpy(handshake_inicial->id, app_conf.ip_comanda); // TODO: Esto comentarlo, porque no sirve
		handshake_inicial->posX = 0;//No importa este valor, y todavia no lo conozco
		handshake_inicial->posY = 0;//No importa este valor, y todavia no lo conozco
		handshake_inicial->tipoProceso = APP;
		enviar_handshake_inicial(handshake_inicial, socket_handshake, logger);
		free(handshake_inicial);

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake, logger);
		if(tipo_mensaje == RTA_HANDSHAKE){
			uint32_t respuesta_entero = recibir_entero(socket_handshake, logger);
			if (respuesta_entero == COMANDA){
				log_info(logger, "Fin Handshake Inicial con Comanda.");
			}
			else{
				log_error(logger, "[Handshake] el proceso que respondio no es Comanda");
				puts("[Handshake] el proceso que respondio no es Comanda");
				exit(-1);//Termina el programa
			}

		}
		close(socket_handshake);
	}
}

int conectar_a_comanda_simple(){

	int socket_comanda = crear_conexion(app_conf.ip_comanda, app_conf.puerto_comanda);
	if (socket_comanda == -1){
		log_error(logger, "No se pudo conectar a Comanda");
		puts("No se pudo conectar a Comanda.");
		exit(-1);//Termina el programa
	}

	return socket_comanda;
}

void incializarRestoDefault(){
	infoRestoDefault = calloc(1,sizeof(t_info_restaurante));
	strcpy(infoRestoDefault->id, "RESTO_DEFAULT");
	infoRestoDefault->pos_x = app_conf.pos_rest_default_x;
	infoRestoDefault->pos_y = app_conf.pos_rest_default_y;
	infoRestoDefault->socketEnvio = -1;
	infoRestoDefault->socketEscucha = -1;
	//list_add(restaurantesConectados,infoRestoDefault);//Mejor ver si la lista esta vacia, si lo esta, responder este restaurante
}

void inicializarListaClientesRest(){
	clientesConectados = list_create();
	restaurantesConectados = list_create();
}

t_info_cliente* buscarClienteConectado(char* nombre_cliente){
	bool cliente_igual(t_info_cliente* info_cliente){
		return string_equals_ignore_case(info_cliente->id, nombre_cliente);
	}

	pthread_mutex_lock(&mutexClientes);
	t_info_cliente* cliente = list_find(clientesConectados,(void*)cliente_igual);
	pthread_mutex_unlock(&mutexClientes);

	return cliente;
}

t_info_restaurante* buscarRestauranteConectado(char* nombre_restaurante) {
	bool restaurante_igual(t_info_restaurante* info_restaurante) {
		return string_equals_ignore_case(info_restaurante->id,
				nombre_restaurante);
	}
	t_info_restaurante* restaurante;
	if (list_size(restaurantesConectados) > 0) {
		pthread_mutex_lock(&mutexRestaurantes);
		restaurante = list_find(restaurantesConectados,
				(void*) restaurante_igual);
		pthread_mutex_unlock(&mutexRestaurantes);
	} else {
		restaurante = infoRestoDefault;
	}
	return restaurante;
}

t_rta_consultar_restaurantes* obtenerRestaurantes(){
	t_rta_consultar_restaurantes* respuesta_restaurantes = malloc(sizeof(t_rta_consultar_restaurantes));
	respuesta_restaurantes->restaurantes = list_create();

	void* obtenerIdChar(t_info_restaurante* info_restaurante){
		char* nombre_restraurante = malloc(L_ID);
		strcpy(nombre_restraurante, info_restaurante->id);
		return nombre_restraurante;
	}

	if(list_size(restaurantesConectados) > 0) {
		/*MAPPING  t_info_cliente -> char[L_ID] */
		respuesta_restaurantes->restaurantes = list_map(restaurantesConectados,(void*)obtenerIdChar);
		respuesta_restaurantes->cantRestaurantes = list_size(restaurantesConectados);
	} else {
		// Cargo el restaurante default
		char* resto_default = calloc(1, L_ID);
		strcpy(resto_default, infoRestoDefault->id);
		list_add(respuesta_restaurantes->restaurantes, resto_default);
		respuesta_restaurantes->cantRestaurantes = 1;
	}

	return respuesta_restaurantes;
}

void procesarMensaje(int socket_cliente, char* id_cliente){

	int socket_comanda = -1;
	t_tipoMensaje tipo_rta;
	t_info_cliente* cliente = buscarClienteConectado(id_cliente);
	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_cliente, logger);
	log_info(logger, "Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

	switch(tipo_mensaje){
	case CONSULTAR_RESTAURANTES:;
		recibir_mensaje_vacio(socket_cliente,logger);
		t_rta_consultar_restaurantes* rta_consultar_restaurantes = obtenerRestaurantes();
		imprimir_lista_strings(rta_consultar_restaurantes->restaurantes,"LISTA RESTAURANTES A ENVIAR");
		log_info(logger,"CANTIDAD RESTOS:%d",rta_consultar_restaurantes->cantRestaurantes);

		enviar_rta_consultar_restaurantes(rta_consultar_restaurantes,socket_cliente,logger);
		list_destroy_and_destroy_elements(rta_consultar_restaurantes->restaurantes, free);
		free(rta_consultar_restaurantes);

		break;

	case SELECCIONAR_RESTAURANTE:;
		char* restauranteSeleccionado = recibir_seleccionar_restaurante(socket_cliente,logger);
		log_info(logger,"RESTAURANTE SELECCIONADO:%s",restauranteSeleccionado);

//		cliente = buscarClienteConectado(id_cliente);
		t_info_restaurante* info_restaurante = buscarRestauranteConectado(restauranteSeleccionado);

		uint32_t resultadoSeleccionar = 0;

		if(info_restaurante != NULL) {
			strcpy(cliente->restaurante_seleccionado, restauranteSeleccionado);
			resultadoSeleccionar = 1;
			log_info(logger,"RESTAURANTE :%s ASOCIADO AL CLIENTE: %s",restauranteSeleccionado, cliente->id);
		}
		//TODO: enviar el OK
		enviar_entero(RTA_SELECCIONAR_RESTAURANTE,resultadoSeleccionar,socket_cliente,logger);

		free(restauranteSeleccionado);

		break;

	case CONSULTAR_PLATOS:;
		char* restaurante = recibir_consultar_platos(socket_cliente,logger);
//			log_info(logger,"RESTAURANTE:%s CONSULTA_PLATOS",restaurante);
		t_rta_consultar_platos* rta_consultar_platos = malloc(sizeof(t_rta_consultar_platos));
		t_list* listaPlatos = list_create();

		int j = 0;
		if(list_size(restaurantesConectados) > 0) {
			//TODO: Consultar al restaurante seleccionado

		} else {
			while(app_conf.platos_default[j]){
				t_plato* plato = malloc(sizeof(t_plato));
				strcpy(plato->nombre,app_conf.platos_default[j]);
				plato->precio = 0;
				list_add(listaPlatos,plato);
				j++;
			}
		}

		rta_consultar_platos->cantPlatos = j;
		rta_consultar_platos->platos = listaPlatos;
		enviar_rta_consultar_platos(rta_consultar_platos,socket_cliente,logger);

		free(restaurante);

		break;

	case CREAR_PEDIDO:;
		recibir_mensaje_vacio(socket_cliente, logger);

		uint32_t id_pedido;

		cliente = buscarClienteConectado(id_cliente);
		t_info_restaurante* rest = buscarRestauranteConectado(cliente->restaurante_seleccionado);

		if(list_size(restaurantesConectados) > 0) {

			//TODO: Enviar CREAR_PEDIDO al restaurante seleccionado por el cliente
			enviar_mensaje_vacio(CREAR_PEDIDO, rest->socketEnvio, logger);
			tipo_rta = recibir_tipo_mensaje(rest->socketEnvio, logger);
			if (tipo_rta == RTA_CREAR_PEDIDO){
				uint32_t id_recibido = recibir_entero(rest->socketEnvio, logger);
				log_info(logger, "[RTA_CREAR_PEDIDO]ID_Pedido: %d", id_recibido);
				id_pedido = id_recibido;
			}

		} else {

			id_pedido = generar_id_pedido();
			log_info(logger, "[DEFAULT_CREAR_PEDIDO]ID_Pedido generado: %d", id_pedido);
		}

		// Agrego el pedido al Cliente
		uint32_t* puntero_pedido = malloc(sizeof(uint32_t));
		*puntero_pedido = id_pedido;
		list_add(cliente->pedidos, puntero_pedido);
		log_info(logger, "[CREAR_PEDIDO]ID_Pedido: %d asociado a %s", id_pedido, cliente->id);

		// Envío GUARDAR_PEDIDO a Comanda
		socket_comanda = conectar_a_comanda_simple();
		t_guardar_pedido* msg_guardar_pedido = calloc(1,sizeof(t_guardar_pedido));
		strcpy(msg_guardar_pedido->restaurante, rest->id);
		msg_guardar_pedido->id_pedido = id_pedido;
		log_info(logger, "Parametros a enviar: Restaurante: %s, ID_Pedido: %d", msg_guardar_pedido->restaurante, msg_guardar_pedido->id_pedido);
		enviar_guardar_pedido(msg_guardar_pedido, socket_comanda, logger);
		tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
		if (tipo_rta == RTA_GUARDAR_PEDIDO){
			uint32_t resultado = recibir_entero(socket_comanda, logger);
			log_info(logger, "[RTA_GUARDAR_PEDIDO]Resultado: %s",resultado? "OK":"FAIL");

			if(resultado != 1) {
				id_pedido = -1; // TODO: VALIDAR ESTO
			}
		}

		enviar_entero(RTA_CREAR_PEDIDO,id_pedido,socket_cliente,logger);

		break;

	case ANADIR_PLATO:;

		uint32_t resultado_anadir_plato = 1;
		t_anadir_plato* anadirPlato = recibir_anadir_plato(socket_cliente,logger);
		log_info(logger,"PLATO:%s",anadirPlato->plato);
		log_info(logger,"ID_PEDIDO:%d",anadirPlato->id_pedido);

		// Enviar ANADIR_PLATO al restaurante seleccionado
		if(list_size(restaurantesConectados) > 0) {

			//TODO: Enviar ANADIR_PLATO al restaurante seleccionado por el cliente
			t_info_restaurante* info_rest = buscarRestauranteConectado(cliente->restaurante_seleccionado);
			t_anadir_plato* msg_anadir_plato = calloc(1,sizeof(t_anadir_plato));
			strcpy(msg_anadir_plato->plato, anadirPlato->plato);
			msg_anadir_plato->id_pedido = anadirPlato->id_pedido;
			log_info(logger, "Parametro a enviar: Plato: %s, ID_Pedido: %d", msg_anadir_plato->plato, msg_anadir_plato->id_pedido);
			enviar_anadir_plato(msg_anadir_plato, info_rest->socketEnvio, logger);
			free(msg_anadir_plato);
			tipo_rta = recibir_tipo_mensaje(info_rest->socketEnvio, logger);
			if (tipo_rta == RTA_ANADIR_PLATO){
				resultado_anadir_plato = recibir_entero(info_rest->socketEnvio, logger);
				log_info(logger, "[RTA_ANADIR_PLATO]Resultado Restaurante: %s",resultado_anadir_plato? "OK":"FAIL");
			}
		}

		if(resultado_anadir_plato) {
			// Informar a Comanda la ADHESION de dicho plato
			// Envío GUARDAR_PLATO a Comanda
			socket_comanda = conectar_a_comanda_simple();
			t_guardar_plato* msg_guardar_plato = calloc(1,sizeof(t_guardar_plato));
			strcpy(msg_guardar_plato->restaurante, cliente->restaurante_seleccionado);
			msg_guardar_plato->id_pedido = anadirPlato->id_pedido;
			strcpy(msg_guardar_plato->plato, anadirPlato->plato);
			msg_guardar_plato->cantPlato = 1;
			log_info(logger, "Parametros a enviar: Restaurante: %s, ID_Pedido: %d, Comida: %s, Cantidad: %d", msg_guardar_plato->restaurante, msg_guardar_plato->id_pedido, msg_guardar_plato->plato, msg_guardar_plato->cantPlato);
			enviar_guardar_plato(msg_guardar_plato, socket_comanda, logger);
			tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
			if (tipo_rta == RTA_GUARDAR_PLATO){
				resultado_anadir_plato = recibir_entero(socket_cliente, logger);
				log_info(logger, "[RTA_GUARDAR_PLATO]Resultado Comanda: %s",resultado_anadir_plato? "OK":"FAIL");
			}
		}

		enviar_entero(RTA_ANADIR_PLATO,resultado_anadir_plato,socket_cliente,logger);

		free(anadirPlato);

		break;

//	case PLATO_LISTO:;
//		t_plato_listo* platoListo = recibir_plato_listo(socket_cliente,logger);
//		log_info(logger,"RESTAURANTE:%s",platoListo->restaurante);
//		log_info(logger,"PLATO:%s",platoListo->plato);
//		log_info(logger,"ID_PEDIDO:%d",platoListo->id_pedido);
//		uint32_t resultado_plato_listo = 0;
//		enviar_entero(RTA_PLATO_LISTO,resultado_plato_listo,socket_cliente,logger);
//
//		free(platoListo);
//
//		break;

	case CONFIRMAR_PEDIDO:;

		t_info_restaurante* info_rest;
		t_rta_obtener_pedido* respuesta;
		uint32_t resultado_confirmar_pedido;
		if(list_size(restaurantesConectados) > 0)
			//TODO: Enviar ANADIR_PLATO al restaurante seleccionado por el cliente
			info_rest = buscarRestauranteConectado(cliente->restaurante_seleccionado);

		t_confirmar_pedido* confirmarPedido = recibir_confirmar_pedido(socket_cliente,logger);
		log_info(logger,"RESTAURANTE:%s",cliente->restaurante_seleccionado);
		log_info(logger,"ID_PEDIDO:%d",confirmarPedido->id_pedido);

		/* Obtener pedido de COMANDA */
		socket_comanda = conectar_a_comanda_simple();
		t_obtener_pedido* msg_obtener_pedido_aux = calloc(1,sizeof(t_obtener_pedido));
		strcpy(msg_obtener_pedido_aux->restaurante, cliente->restaurante_seleccionado);
		msg_obtener_pedido_aux->id_pedido = confirmarPedido->id_pedido;
		log_info(logger, "Parametros a enviar: Restaurante: %s, ID_Pedido: %d", msg_obtener_pedido_aux->restaurante, msg_obtener_pedido_aux->id_pedido);
		enviar_obtener_pedido(msg_obtener_pedido_aux, socket_comanda, logger);
		free(msg_obtener_pedido_aux);
		tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
		if (tipo_rta == RTA_OBTENER_PEDIDO){
			respuesta = recibir_rta_obtener_pedido(socket_comanda, logger);
		}

		if(respuesta->estado == PENDIENTE) {
			log_info(logger, "Parametro a enviar: ID_Pedido: %d", confirmarPedido->id_pedido);
			enviar_confirmar_pedido(confirmarPedido, info_rest->socketEnvio, logger);
			free(confirmarPedido);
			tipo_rta = recibir_tipo_mensaje(info_rest->socketEnvio, logger);
			if (tipo_rta == RTA_CONFIRMAR_PEDIDO){
				resultado_confirmar_pedido = recibir_entero(info_rest->socketEnvio, logger);
				log_info(logger, "[RTA_CONFIRMAR_PEDIDO]Resultado: %s",resultado_confirmar_pedido? "OK":"FAIL");
			}
		}
		free(respuesta);
		enviar_entero(RTA_CONFIRMAR_PEDIDO,resultado_confirmar_pedido,socket_cliente,logger);

		break;
	case CONSULTAR_PEDIDO:;
		uint32_t idPedido = recibir_entero(socket_cliente,logger);
		log_info(logger,"ID_PEDIDO:%d",idPedido);

		socket_comanda = conectar_a_comanda_simple();
		t_obtener_pedido* msg_obtener_pedido = calloc(1,sizeof(t_obtener_pedido));
		strcpy(msg_obtener_pedido->restaurante, cliente->restaurante_seleccionado);
		msg_obtener_pedido->id_pedido = idPedido;
		log_info(logger, "Parametros a enviar: Restaurante: %s, ID_Pedido: %d", msg_obtener_pedido->restaurante, msg_obtener_pedido->id_pedido);
		enviar_obtener_pedido(msg_obtener_pedido, socket_comanda, logger);
		free(msg_obtener_pedido);
		tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
		t_rta_consultar_pedido* rta_consultar_pedido;
		if (tipo_rta == RTA_OBTENER_PEDIDO){
			t_rta_obtener_pedido* respuesta = recibir_rta_obtener_pedido(socket_comanda, logger);
			// De momento se deja para tradear
			log_info(logger, "[RTA_OBTENER_PEDIDO]Estado del pedido: %s", estado_string(respuesta->estado));
			for(int i = 0; i < respuesta->cantComidas; i++){
				t_comida* comida_i = list_get(respuesta->comidas, i);
				log_info(logger, "[RTA_OBTENER_PEDIDO]Comida %d: %s, Cant. total: %d, Cant. lista: %d", i+1, comida_i->nombre, comida_i->cantTotal, comida_i->cantLista);
			}
			// De momento se deja para tradear

			rta_consultar_pedido = malloc(sizeof(t_rta_consultar_pedido));
			strcpy(rta_consultar_pedido->restaurante, cliente->restaurante_seleccionado);
			rta_consultar_pedido->cantComidas = respuesta->cantComidas;
			rta_consultar_pedido->comidas = respuesta->comidas;
			rta_consultar_pedido->estado = respuesta->estado;
			free(respuesta);
		}

		enviar_rta_consultar_pedido(rta_consultar_pedido,socket_cliente,logger);
		list_destroy_and_destroy_elements(rta_consultar_pedido->comidas, free);
		free(rta_consultar_pedido);

		break;
	default:
		break;
	}

	if(socket_comanda != -1) {
		close(socket_comanda);
	}
}

void procesar_handshake_inicial(t_handshake_inicial* handshake_inicial, int socket_emisor){

	switch(handshake_inicial->tipoProceso){

	case CLIENTE:{
		log_info(logger, "[handshake_inicial] Cliente");
		t_info_cliente* new_client = calloc(1,sizeof(t_info_cliente));
		strcpy(new_client->id, handshake_inicial->id);
		new_client->pos_x = handshake_inicial->posX;
		new_client->pos_y = handshake_inicial->posY;
		new_client->socketEscucha = socket_emisor;
		new_client->pedidos = list_create();
		pthread_mutex_lock(&mutexClientes);
		list_add(clientesConectados, new_client);
		pthread_mutex_unlock(&mutexClientes);
		break;
	}
	case RESTAURANTE:{
		log_info(logger, "[handshake_inicial] Restaurante");
		t_info_restaurante* new_resto = calloc(1,sizeof(t_info_restaurante));
		strcpy(new_resto->id, handshake_inicial->id);
		new_resto->pos_x = handshake_inicial->posX;
		new_resto->pos_y = handshake_inicial->posY;
		new_resto->socketEscucha = socket_emisor;
		pthread_t hilo_escucha;
		pthread_create(&hilo_escucha, NULL, (void*) iniciar_conexion_escucha, &socket_emisor);
		pthread_mutex_lock(&mutexRestaurantes);
		list_add(restaurantesConectados, new_resto);
		pthread_mutex_unlock(&mutexRestaurantes);
		log_info(logger, "[handshake_inicial] Restaurante %s agregado", new_resto->id);
		break;
	}
	default:
		log_error(logger, "[Handshake Inicial] Tipo de proceso no soportado por App.");
		close(socket_emisor);
		break;
	}
}

void atenderConexion(int* socket_emisor) {

	t_tipoMensaje tipoMensaje = recibir_tipo_mensaje(*socket_emisor, logger);

	if (tipoMensaje == -1){
		close(*socket_emisor);
		free(socket_emisor);
		pthread_exit(NULL);
	}

	log_info(logger, "Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipoMensaje));

	switch(tipoMensaje) {

		case HANDSHAKE_INICIAL:{
			t_handshake_inicial* handshake_inicial = recibir_handshake_inicial(*socket_emisor, logger);
			procesar_handshake_inicial(handshake_inicial, *socket_emisor);
			free(handshake_inicial);
			uint32_t miTipoProceso = APP;
			enviar_entero(RTA_HANDSHAKE, miTipoProceso, *socket_emisor, logger);
			break;
		}
		case HANDSHAKE:{
			t_handshake* handshake = recibir_handshake(*socket_emisor, logger);
			/*SI SE CONECTA UN CLIENTE, RECIBO EL MENSAJE QUE QUIERA ENVIAR*/
			if(handshake->tipoProceso == CLIENTE){
				log_info(logger, "Recibiendo mensaje de cliente");
				procesarMensaje(*socket_emisor, handshake->id);
				close(*socket_emisor);
			}
			/*SI SE CONECTA UN RESTAURANTE, GUARDO EL SOCKET COMO SOCKET DE ENVIO*/
			else if(handshake->tipoProceso == RESTAURANTE){
				log_info(logger, "Recibiendo mensaje de restaurante");
				t_info_restaurante* resto_buscado = buscarRestauranteConectado(handshake->id);
				if (resto_buscado != NULL)
					resto_buscado->socketEnvio = *socket_emisor;//No se cierra el socket y se lo guarda.
				else{
					log_warning(logger, "[Socket envio] No se entontro el restaurante %s en la lista de conectados.", handshake->id);
					close(*socket_emisor);
				}
			}
			free(handshake);
			break;
		}
		default:
			log_error(logger, "Tipo de mensaje no admitido.");
			close(*socket_emisor);
			break;
	}
	free(socket_emisor);
}

void crearServidor() {

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP_APP, app_conf.puerto_escucha, &hints, &servinfo);

	for (p=servinfo; p != NULL; p = p->ai_next)
	{
		if ((socket_app = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;

		if (bind(socket_app, p->ai_addr, p->ai_addrlen) == -1) {
			close(socket_app);
			continue;
		}
		break;
	}

	listen(socket_app, SOMAXCONN);

	freeaddrinfo(servinfo);

	log_trace(logger, "SERVIDOR | Escuchando conexiones");

	while(1)
		esperar_cliente(socket_app);

//	/*Abro socket*/
//	socket_app = definirSocket(logger);
//
//	/*Bind & Listen*/
//	if (bindearSocketYEscuchar(socket_app, IP_APP , app_conf.puerto_escucha,
//			logger) <= 0)
//		_exit_with_error("BIND", NULL, logger);
//
//	/*Atiendo solicitudes creando un hilo para cada una*/
//	while(1){
//		pthread_t hilo_conexion;
//		int* socketCliente = malloc(sizeof(int));
//
//		*socketCliente = aceptarConexiones(socket_app, logger);
//
//		if(socketCliente < 0)
//			log_error(logger, "Error al realizar ACCEPT.\n");
//
//		pthread_create(&hilo_conexion,NULL,(void*)atenderConexion,socketCliente);
//		pthread_detach(hilo_conexion);
//	}
}

void esperar_cliente(int socket_servidor){
	pthread_t hilo_conexion;

	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int* new_socket = malloc(sizeof(int));

	*new_socket = accept(socket_servidor, (void*) &dir_cliente, (void*) &tam_direccion);

	pthread_create(&hilo_conexion, NULL, (void*)atenderConexion, new_socket);
	pthread_detach(hilo_conexion);
}

uint32_t generar_id_pedido() {

	return rand() % 1000;
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


char* estado_string(uint32_t estado_num) {

	switch(estado_num){
		case PENDIENTE:
			return "PENDIENTE";
		case CONFIRMADO:
			return "CONFIRMADO";
		case TERMINADO:
			return "TERMINADO";
		default:
			return "FAILED";
	}
}

void iniciar_conexion_escucha(int *socket_escucha){

	while(1){

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_escucha, logger);
		if (tipo_mensaje == -1){
			log_warning(logger, "Se desconecto del proceso server.");
			puts("Se desconecto del proceso server.");
			close(socket_escucha);
			pthread_exit(NULL);
		}

		log_info(logger, "[ACTUALIZACIONES] Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

		switch(tipo_mensaje){

			case PLATO_LISTO:{
				t_plato_listo* recibido = recibir_plato_listo(socket_escucha, logger);
				log_info(logger, "[PLATO_LISTO]Restaurante: %s, ID_Pedido: %d, Plato: %s", recibido->restaurante, recibido->id_pedido, recibido->plato);
				free(recibido);
				enviar_entero(RTA_PLATO_LISTO, 1, socket_escucha, logger);
			}
			break;
//			case FINALIZAR_PEDIDO:{
//				t_finalizar_pedido* recibido = recibir_finalizar_pedido(socket_escucha, logger);
//				log_info(logger, "[FINALIZAR_PEDIDO]Restaurante: %s, ID_Pedido: %d", recibido->restaurante, recibido->id_pedido);
//				free(recibido);
//				enviar_entero(RTA_FINALIZAR_PEDIDO, 1, socket_escucha, logger);
//			}
//			break;
			default:
				puts("No se reconoce el tipo de mensaje recibido");
				break;
//		}
	}
}
