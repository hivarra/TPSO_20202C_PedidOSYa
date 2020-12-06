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

//	while(1){
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_cliente, logger);
		log_info(logger, "Se recibe tipo de mensaje: %s", get_nombre_mensaje(tipo_mensaje));

		switch(tipo_mensaje){
		case CONSULTAR_RESTAURANTES:;
			recibir_mensaje_vacio(socket_cliente,logger);
			t_rta_consultar_restaurantes* rta_consultar_restaurantes = obtenerRestaurantes();
//			if(rta_consultar_restaurantes == NULL)
//				log_info(logger,"PRUEBAAA");
//			else{
			imprimir_lista_strings(rta_consultar_restaurantes->restaurantes,"LISTA RESTAURANTES A ENVIAR");
			log_info(logger,"CANTIDAD RESTOS:%d",rta_consultar_restaurantes->cantRestaurantes);
//			}


			enviar_rta_consultar_restaurantes(rta_consultar_restaurantes,socket_cliente,logger);
			list_destroy_and_destroy_elements(rta_consultar_restaurantes->restaurantes, free);
			free(rta_consultar_restaurantes);

			break;
		case SELECCIONAR_RESTAURANTE:;
			char* restauranteSeleccionado = recibir_seleccionar_restaurante(socket_cliente,logger);
			log_info(logger,"RESTAURANTE SELECCIONADO:%s",restauranteSeleccionado);

			t_info_cliente* cliente = buscarClienteConectado(id_cliente);
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

			if(list_size(restaurantesConectados) > 0) {

				//TODO: Buscar el restaurante seleccionado por el cliente
				t_info_cliente* cliente = buscarClienteConectado(id_cliente);
				t_info_restaurante* rest = buscarRestauranteConectado(cliente->restaurante_seleccionado);

				//TODO: Enviar CREAR_PEDIDO al restaurante seleccionado por el cliente
				enviar_mensaje_vacio(CREAR_PEDIDO, rest->socketEscucha, logger);
				t_tipoMensaje tipo_rta = recibir_tipo_mensaje(rest->socketEscucha, logger);
				if (tipo_rta == RTA_CREAR_PEDIDO){
					uint32_t id_recibido = recibir_entero(rest->socketEscucha, logger);
					log_info(logger, "[RTA_CREAR_PEDIDO]ID_Pedido: %d", id_recibido);
					id_pedido = id_recibido;
				}

			} else {

				id_pedido = 10;
				log_info(logger, "ID_Pedido generado: %d", id_pedido);
			}



			enviar_entero(RTA_CREAR_PEDIDO,id_pedido,socket_cliente,logger);

			break;

		case ANADIR_PLATO:;
			t_anadir_plato* anadirPlato = recibir_anadir_plato(socket_cliente,logger);
			log_info(logger,"PLATO:%s",anadirPlato->plato);
			log_info(logger,"ID_PEDIDO:%d",anadirPlato->id_pedido);
			uint32_t resultado_anadir_plato = 0;
			enviar_entero(RTA_ANADIR_PLATO,resultado_anadir_plato,socket_cliente,logger);

			free(anadirPlato);

			break;

		case PLATO_LISTO:;
			t_plato_listo* platoListo = recibir_plato_listo(socket_cliente,logger);
			log_info(logger,"RESTAURANTE:%s",platoListo->restaurante);
			log_info(logger,"PLATO:%s",platoListo->plato);
			log_info(logger,"ID_PEDIDO:%d",platoListo->id_pedido);
			uint32_t resultado_plato_listo = 0;
			enviar_entero(RTA_PLATO_LISTO,resultado_plato_listo,socket_cliente,logger);

			free(platoListo);

			break;

		case CONFIRMAR_PEDIDO:;
			t_confirmar_pedido* confirmarPedido = recibir_confirmar_pedido(socket_cliente,logger);
			log_info(logger,"RESTAURANTE:%s",confirmarPedido->restaurante);
			log_info(logger,"ID_PEDIDO:%d",confirmarPedido->id_pedido);
			uint32_t resultado_confirmar_pedido = 0;
			enviar_entero(RTA_CONFIRMAR_PEDIDO,resultado_confirmar_pedido,socket_cliente,logger);

			free(confirmarPedido);

			break;
		case CONSULTAR_PEDIDO:;
			uint32_t idPedido = recibir_entero(socket_cliente,logger);
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
			enviar_rta_consultar_pedido(rta_consultar_pedido,socket_cliente,logger);

			free(rta_consultar_pedido);

			break;
		default:
			break;
		}
//	}
}

void procesar_handshake_inicial(t_handshake_inicial* handshake_inicial, int socket_emisor){

	switch(handshake_inicial->tipoProceso){

	case CLIENTE:{
		t_info_cliente* new_client = calloc(1,sizeof(t_info_cliente));
		strcpy(new_client->id, handshake_inicial->id);
		new_client->pos_x = handshake_inicial->posX;
		new_client->pos_y = handshake_inicial->posY;
		new_client->socketEscucha = socket_emisor;
		//new_client->pedidos = list_create();
		pthread_mutex_lock(&mutexClientes);
		list_add(clientesConectados, new_client);
		pthread_mutex_unlock(&mutexClientes);
		break;
	}
	case RESTAURANTE:{
		t_info_restaurante* new_resto = calloc(1,sizeof(t_info_restaurante));
		strcpy(new_resto->id, handshake_inicial->id);
		new_resto->pos_x = handshake_inicial->posX;
		new_resto->pos_y = handshake_inicial->posY;
		new_resto->socketEscucha = socket_emisor;
		pthread_mutex_lock(&mutexRestaurantes);
		list_add(restaurantesConectados, new_resto);
		pthread_mutex_unlock(&mutexRestaurantes);
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
