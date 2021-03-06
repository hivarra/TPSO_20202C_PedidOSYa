/*
 * Conexion.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#include "Conexion.h"

void iniciar_conexion_escucha(t_info_restaurante*);
uint32_t generar_id_pedido(void);

void conectar_a_comanda(){
	int socket_handshake = crear_conexion(app_conf.ip_comanda, app_conf.puerto_comanda);
	if (socket_handshake == -1){
		log_error(logger, "No se pudo conectar a Comanda.");
		puts("No se pudo conectar a Comanda.");
		exit(-1);//Termina el programa
	}
	else{
		/*Realizo el handshake inicial*/
		t_handshake_inicial* handshake_inicial = calloc(1,sizeof(t_handshake_inicial));
		strcpy(handshake_inicial->id, "App"); //No importa.
		handshake_inicial->posX = 0;//No importa este valor.
		handshake_inicial->posY = 0;//No importa este valor.
		handshake_inicial->tipoProceso = APP;
		enviar_handshake_inicial(handshake_inicial, socket_handshake, logger);
		free(handshake_inicial);

		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake, logger);
		if(tipo_mensaje == RTA_HANDSHAKE){
			uint32_t respuesta_entero = recibir_entero(socket_handshake, logger);
			if (respuesta_entero == COMANDA){
				log_trace(logger, "[Conexion Inicial] Se conecto exitosamente con Comanda.");
			}
			else{
				log_error(logger, "[Conexion Inicial] El proceso que respondio no es Comanda.");
				puts("[Conexion Inicial] El proceso que respondio no es Comanda.");
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
}

void inicializarListaClientesRest(){
	clientesConectados = list_create();
	restaurantesConectados = list_create();
	incializarRestoDefault();
	pthread_mutex_init(&mutex_id_rest_default, NULL);
	id_rest_default = 0;
}

void procesarMensaje(int socket_cliente, char* id_cliente){

	int socket_comanda = -1;
	t_tipoMensaje tipo_rta;
	t_info_cliente* cliente = buscarClienteConectado(id_cliente);
	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_cliente, logger);
//	log_trace(logger, "[From Cliente] Se recibe tipo de mensaje %s desde el cliente %s", get_nombre_mensaje(tipo_mensaje), id_cliente);

	switch(tipo_mensaje){
	case CONSULTAR_RESTAURANTES:{
		recibir_mensaje_vacio(socket_cliente,logger);
		log_trace(logger, "[Recepcion Mensaje] Se recibio CONSULTAR_RESTAURANTES desde el cliente %s.", id_cliente);

		t_rta_consultar_restaurantes* rta_consultar_restaurantes = obtenerRestaurantes();
//		void imprimir_rest(char* resto){
//			log_trace(logger, "\t%s", resto);
//		}
//		list_iterate(rta_consultar_restaurantes->restaurantes, (void*)imprimir_rest);

		enviar_rta_consultar_restaurantes(rta_consultar_restaurantes,socket_cliente,logger);
		list_destroy_and_destroy_elements(rta_consultar_restaurantes->restaurantes, free);
		free(rta_consultar_restaurantes);
	}
	break;

	case SELECCIONAR_RESTAURANTE:{
		uint32_t resultadoSeleccionar = 0;
		char* restauranteSeleccionado = recibir_seleccionar_restaurante(socket_cliente,logger);
		log_trace(logger, "[Recepcion Mensaje] Se recibio SELECCIONAR_RESTAURANTE: Cliente: %s, Restaurante: %s.", id_cliente,restauranteSeleccionado);
		t_info_restaurante* info_restaurante = buscarRestauranteConectado(restauranteSeleccionado);

		if(info_restaurante != NULL || string_equals_ignore_case(restauranteSeleccionado, infoRestoDefault->id)) {
			strcpy(cliente->restaurante_seleccionado, restauranteSeleccionado);
			resultadoSeleccionar = 1;
		}
		else
			log_warning(logger, "[SELECCIONAR_RESTAURANTE] Restaurante invalido.");
		enviar_entero(RTA_SELECCIONAR_RESTAURANTE,resultadoSeleccionar,socket_cliente,logger);
		free(restauranteSeleccionado);
	}
	break;

	case CONSULTAR_PLATOS:{
		char* ene = recibir_consultar_platos(socket_cliente,logger);
		free(ene);//NO IMPORTA
		log_trace(logger, "[Recepcion Mensaje] Se recibio CONSULTAR_PLATOS: Cliente: %s, Restaurante seleccionado: %s.", id_cliente,cliente->restaurante_seleccionado);
		t_rta_consultar_platos* respuesta;

		if(string_equals_ignore_case(cliente->restaurante_seleccionado, infoRestoDefault->id)){
			respuesta = malloc(sizeof(t_rta_consultar_platos));
			respuesta->platos = list_create();
			int i = 0;
			while(app_conf.platos_default[i] != NULL){
				t_plato* plato_i = calloc(1,sizeof(t_plato));
				strcpy(plato_i->nombre, app_conf.platos_default[i]);
				plato_i->precio = 0;//Se agrega este parametro por compatibilidad con API
				list_add(respuesta->platos, plato_i);
				i++;
			}
			respuesta->cantPlatos = i;
		}
		else {
			t_info_restaurante* info_restaurante = buscarRestauranteConectado(cliente->restaurante_seleccionado);
			if(info_restaurante != NULL){
				char* msg_consultar_platos = calloc(1,L_ID);
				strcpy(msg_consultar_platos, "N");//EL RESTAURANTE NO NECESITA ESTE PARAMETRO
				enviar_consultar_platos(msg_consultar_platos, info_restaurante->socketEnvio, logger);
				free(msg_consultar_platos);
				t_tipoMensaje tipo_rta = recibir_tipo_mensaje(info_restaurante->socketEnvio, logger);
				if (tipo_rta == RTA_CONSULTAR_PLATOS)
					respuesta = recibir_rta_consultar_platos(info_restaurante->socketEnvio, logger);
			}
			else{
				respuesta = malloc(sizeof(t_rta_consultar_platos));
				respuesta->platos = list_create();
				respuesta->cantPlatos = 0;
				log_warning(logger, "[CONSULTAR_PLATOS] Restaurante invalido.");
			}
		}
//		log_trace(logger, "[CONSULTAR_PLATOS] Platos del restaurante %s:", restaurante);
//		for(int i = 0; i < respuesta->cantPlatos; i++){
//			t_plato* plato_i = list_get(respuesta->platos, i);
//			log_trace(logger, "\tPlato %d: %s, Precio: %d", i+1, plato_i->nombre, plato_i->precio);
//		}
		enviar_rta_consultar_platos(respuesta, socket_cliente, logger);
		list_destroy_and_destroy_elements(respuesta->platos, free);
		free(respuesta);
	}
	break;

	case CREAR_PEDIDO:{
		recibir_mensaje_vacio(socket_cliente, logger);
		log_trace(logger, "[Recepcion Mensaje] Se recibio CREAR_PEDIDO desde el cliente %s.", id_cliente);
		uint32_t id_pedido = 0;

		if(string_equals_ignore_case(cliente->restaurante_seleccionado, infoRestoDefault->id))
			id_pedido = generar_id_pedido();
		else{
			t_info_restaurante* rest = buscarRestauranteConectado(cliente->restaurante_seleccionado);
			if (rest != NULL){
				enviar_mensaje_vacio(CREAR_PEDIDO, rest->socketEnvio, logger);
				tipo_rta = recibir_tipo_mensaje(rest->socketEnvio, logger);
				if (tipo_rta == RTA_CREAR_PEDIDO){
					id_pedido = recibir_entero(rest->socketEnvio, logger);
					if (!id_pedido)
						log_warning(logger, "[CREAR_PEDIDO] Error al crear pedido del restaurante: %s.", cliente->restaurante_seleccionado);
				}
			}
			else
				log_warning(logger, "[CREAR_PEDIDO] El cliente %s no ha seleccionado ningun restaurante.", cliente->id);
		}

		if (id_pedido){
			// Envío GUARDAR_PEDIDO a Comanda
			socket_comanda = conectar_a_comanda_simple();
			t_guardar_pedido* msg_guardar_pedido = calloc(1,sizeof(t_guardar_pedido));
			strcpy(msg_guardar_pedido->restaurante, cliente->restaurante_seleccionado);
			msg_guardar_pedido->id_pedido = id_pedido;
//			log_trace(logger, "[GUARDAR_PEDIDO] Parametros a enviar a Comanda: Restaurante: %s, ID_Pedido: %d", msg_guardar_pedido->restaurante, msg_guardar_pedido->id_pedido);
			enviar_guardar_pedido(msg_guardar_pedido, socket_comanda, logger);
			free(msg_guardar_pedido);
			tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
			if (tipo_rta == RTA_GUARDAR_PEDIDO){
				uint32_t resultado = recibir_entero(socket_comanda, logger);
//				log_trace(logger, "[RTA_GUARDAR_PEDIDO] Resultado de Comanda: %s.",resultado? "OK":"FAIL");
				if(resultado == OK){
					// Agrego el pedido al Cliente
					uint32_t* puntero_pedido = malloc(sizeof(uint32_t));
					*puntero_pedido = id_pedido;
					list_add(cliente->pedidos, puntero_pedido);
//					log_trace(logger, "[CREAR_PEDIDO]ID_Pedido: %d agregado al cliente %s.", id_pedido, cliente->id);
				}
				else
					log_warning(logger, "[CREAR_PEDIDO] No se pudo crear el pedido %d, cliente %s en Comanda.", id_pedido, cliente->id);;
			}
		}
		enviar_entero(RTA_CREAR_PEDIDO,id_pedido,socket_cliente,logger);
	}
	break;

	case ANADIR_PLATO:{
		uint32_t resultado_anadir_plato = 0;
		t_anadir_plato* anadirPlato = recibir_anadir_plato(socket_cliente,logger);
		log_trace(logger, "[Recepcion Mensaje] Se recibio ANADIR_PLATO: Cliente: %s, ID_Pedido: %d, Plato: %s.", id_cliente,anadirPlato->id_pedido,anadirPlato->plato);

		// Enviar ANADIR_PLATO al restaurante seleccionado
		if(list_size(restaurantesConectados) > 0) {
			t_info_restaurante* info_rest = buscarRestauranteConectado(cliente->restaurante_seleccionado);
			if(info_rest != NULL){
//				log_trace(logger, "[ANADIR_PLATO] Parametro a enviar al resto %s: Plato: %s, ID_Pedido: %d", cliente->restaurante_seleccionado, msg_anadir_plato->plato, msg_anadir_plato->id_pedido);
				enviar_anadir_plato(anadirPlato, info_rest->socketEnvio, logger);
				tipo_rta = recibir_tipo_mensaje(info_rest->socketEnvio, logger);
				if (tipo_rta == RTA_ANADIR_PLATO){
					resultado_anadir_plato = recibir_entero(info_rest->socketEnvio, logger);
//					log_trace(logger, "[RTA_ANADIR_PLATO] Resultado desde el restaurante: %s", resultado_anadir_plato? "OK":"FAIL");
				}
			}
		}
		else if(string_equals_ignore_case(cliente->restaurante_seleccionado, infoRestoDefault->id)){
			int i = 0;
			while(app_conf.platos_default[i] != NULL){
				if (string_equals_ignore_case(anadirPlato->plato, app_conf.platos_default[i])){
					resultado_anadir_plato = 1;
					break;
				}
				i++;
			}
		}
		else
			log_warning(logger, "[ANADIR_PLATO] El cliente %s no ha seleccionado ningun restaurante.", cliente->id);

		if(resultado_anadir_plato) {
			// Informar a Comanda la ADHESION de dicho plato
			// Envío GUARDAR_PLATO a Comanda
			socket_comanda = conectar_a_comanda_simple();
			t_guardar_plato* msg_guardar_plato = calloc(1,sizeof(t_guardar_plato));
			strcpy(msg_guardar_plato->restaurante, cliente->restaurante_seleccionado);
			msg_guardar_plato->id_pedido = anadirPlato->id_pedido;
			strcpy(msg_guardar_plato->plato, anadirPlato->plato);
			msg_guardar_plato->cantPlato = 1;
//			log_trace(logger, "[GUARDAR_PLATO] Parametros a enviar a Comanda: Restaurante: %s, ID_Pedido: %d, Comida: %s, Cantidad: %d", msg_guardar_plato->restaurante, msg_guardar_plato->id_pedido, msg_guardar_plato->plato, msg_guardar_plato->cantPlato);
			enviar_guardar_plato(msg_guardar_plato, socket_comanda, logger);
			free(msg_guardar_plato);
			tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
			if (tipo_rta == RTA_GUARDAR_PLATO)
				resultado_anadir_plato = recibir_entero(socket_comanda, logger);
		}

		enviar_entero(RTA_ANADIR_PLATO,resultado_anadir_plato,socket_cliente,logger);
		free(anadirPlato);
	}
	break;

	case CONFIRMAR_PEDIDO:{

		uint32_t result_comanda = 0;
		uint32_t result_resto = 0;
		uint32_t result_final = 0;
		uint32_t pedido_pendiente = 0;

		t_confirmar_pedido* confirmarPedido = recibir_confirmar_pedido(socket_cliente,logger);
		log_trace(logger, "[Recepcion Mensaje] Se recibio CONFIRMAR_PEDIDO: Cliente: %s, ID_Pedido: %d.", id_cliente,confirmarPedido->id_pedido);
		strcpy(confirmarPedido->restaurante, cliente->restaurante_seleccionado);

		/*OBTENER_PEDIDO DE COMANDA Y PEDIDO PENDIENTE*/
		socket_comanda = conectar_a_comanda_simple();
		t_obtener_pedido* obtener_pedido = calloc(1,sizeof(t_obtener_pedido));
		strcpy(obtener_pedido->restaurante, confirmarPedido->restaurante);
		obtener_pedido->id_pedido = confirmarPedido->id_pedido;
		enviar_obtener_pedido(obtener_pedido, socket_comanda, logger);
		free(obtener_pedido);
		tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
		if (tipo_rta == RTA_OBTENER_PEDIDO){
			t_rta_obtener_pedido* rta_obtener_ped = recibir_rta_obtener_pedido(socket_comanda, logger);
			if (rta_obtener_ped->estado == PENDIENTE)
				pedido_pendiente = 1;
			list_destroy_and_destroy_elements(rta_obtener_ped->comidas, free);
			free(rta_obtener_ped);
		}

		/*RESULTADO DE CONFIRMAR_PEDIDO RESTAURANTE*/
		if(list_size(restaurantesConectados) > 0 && pedido_pendiente){
			t_info_restaurante* info_rest = buscarRestauranteConectado(cliente->restaurante_seleccionado);
			if(info_rest != NULL){
				enviar_confirmar_pedido(confirmarPedido, info_rest->socketEnvio, logger);
				tipo_rta = recibir_tipo_mensaje(info_rest->socketEnvio, logger);
				if (tipo_rta == RTA_CONFIRMAR_PEDIDO)
					result_resto = recibir_entero(info_rest->socketEnvio, logger);
			}
		}
		else if(string_equals_ignore_case(cliente->restaurante_seleccionado, infoRestoDefault->id) && pedido_pendiente)
			result_resto = 1;
		else
			log_warning(logger, "[CONFIRMAR_PEDIDO] El cliente %s no ha seleccionado ningun restaurante.", cliente->id);

		/*RESULTADO DE CONFIRMAR_PEDIDO COMANDA*/
		if(result_resto){
			close(socket_comanda);
			socket_comanda = conectar_a_comanda_simple();
			enviar_confirmar_pedido(confirmarPedido, socket_comanda, logger);
			tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
			if (tipo_rta == RTA_CONFIRMAR_PEDIDO)
				result_comanda = recibir_entero(socket_comanda, logger);
		}

		/*CREO EL PCB*/
		if(result_comanda && result_resto) {
			result_final = 1;
			crearPCB(cliente, confirmarPedido->id_pedido);
		}
		free(confirmarPedido);
		enviar_entero(RTA_CONFIRMAR_PEDIDO,result_final,socket_cliente,logger);
	}
	break;

	case CONSULTAR_PEDIDO:{
		uint32_t idPedido = recibir_entero(socket_cliente,logger);

		log_trace(logger, "[Recepcion Mensaje] Se recibio CONSULTAR_PEDIDO: Cliente: %s, ID_Pedido: %d.", id_cliente,idPedido);

		socket_comanda = conectar_a_comanda_simple();
		t_obtener_pedido* msg_obtener_pedido = calloc(1,sizeof(t_obtener_pedido));
		strcpy(msg_obtener_pedido->restaurante, cliente->restaurante_seleccionado);
		msg_obtener_pedido->id_pedido = idPedido;
//		log_trace(logger, "[CONSULTAR_PEDIDO] Parametros a enviar a Comanda: Restaurante: %s, ID_Pedido: %d", msg_obtener_pedido->restaurante, msg_obtener_pedido->id_pedido);
		enviar_obtener_pedido(msg_obtener_pedido, socket_comanda, logger);
		free(msg_obtener_pedido);
		tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
		t_rta_consultar_pedido* rta_consultar_pedido;
		if (tipo_rta == RTA_OBTENER_PEDIDO){
			t_rta_obtener_pedido* respuesta = recibir_rta_obtener_pedido(socket_comanda, logger);
//			log_trace(logger, "[RTA_OBTENER_PEDIDO] Estado del pedido: %s", estado_string(respuesta->estado));
//			for(int i = 0; i < respuesta->cantComidas; i++){
//				t_comida* comida_i = list_get(respuesta->comidas, i);
//				log_trace(logger, "\tComida %d: %s, Cant. total: %d, Cant. lista: %d", i+1, comida_i->nombre, comida_i->cantTotal, comida_i->cantLista);
//			}
			rta_consultar_pedido = calloc(1,sizeof(t_rta_consultar_pedido));
			strcpy(rta_consultar_pedido->restaurante, cliente->restaurante_seleccionado);
			rta_consultar_pedido->cantComidas = respuesta->cantComidas;
			rta_consultar_pedido->comidas = respuesta->comidas;
			rta_consultar_pedido->estado = respuesta->estado;
			free(respuesta);
		}
		enviar_rta_consultar_pedido(rta_consultar_pedido,socket_cliente,logger);
		list_destroy_and_destroy_elements(rta_consultar_pedido->comidas, free);
		free(rta_consultar_pedido);
	}
	break;

	default:
		log_error(logger, "[Mensaje Cliente] Mensaje no admitido.");
		break;
	}

	if(socket_comanda != -1)
		close(socket_comanda);
}

void procesar_handshake_inicial(t_handshake_inicial* handshake_inicial, int socket_emisor){

	switch(handshake_inicial->tipoProceso){

	case CLIENTE:{
		t_info_cliente* new_client = calloc(1,sizeof(t_info_cliente));
		strcpy(new_client->id, handshake_inicial->id);
		strcpy(new_client->restaurante_seleccionado, "N");
		new_client->pos_x = handshake_inicial->posX;
		new_client->pos_y = handshake_inicial->posY;
		new_client->socketEscucha = socket_emisor;
		new_client->pedidos = list_create();
		pthread_mutex_lock(&mutexClientes);
		list_add(clientesConectados, new_client);
		pthread_mutex_unlock(&mutexClientes);
		log_trace(logger, "[Conexion Inicial] Cliente %s agregado.", new_client->id);
		break;
	}
	case RESTAURANTE:{
		t_info_restaurante* new_resto = calloc(1,sizeof(t_info_restaurante));
		strcpy(new_resto->id, handshake_inicial->id);
		new_resto->pos_x = handshake_inicial->posX;
		new_resto->pos_y = handshake_inicial->posY;
		new_resto->socketEscucha = socket_emisor;
		pthread_create(&new_resto->hilo_escucha, NULL, (void*) iniciar_conexion_escucha, new_resto);
		pthread_detach(new_resto->hilo_escucha);
		pthread_mutex_lock(&mutexRestaurantes);
		list_add(restaurantesConectados, new_resto);
		pthread_mutex_unlock(&mutexRestaurantes);
		log_trace(logger, "[Conexion Inicial] Restaurante %s agregado.", new_resto->id);
		break;
	}
	default:
		log_error(logger, "[Conexion Inicial] Tipo de proceso no soportado por App.");
		close(socket_emisor);
		break;
	}
}

void atenderConexion(int* socket_emisor) {

	t_tipoMensaje tipoMensaje = recibir_tipo_mensaje(*socket_emisor, logger);

	if (tipoMensaje == -1) {
		close(*socket_emisor);
		free(socket_emisor);
		pthread_exit(NULL);
	}

	switch(tipoMensaje) {

		case HANDSHAKE_INICIAL:{
			t_handshake_inicial* handshake_inicial = recibir_handshake_inicial(*socket_emisor, logger);
			procesar_handshake_inicial(handshake_inicial, *socket_emisor);
			free(handshake_inicial);
			uint32_t miTipoProceso = APP;
			enviar_entero(RTA_HANDSHAKE, miTipoProceso, *socket_emisor, logger);
		}
		break;
		case HANDSHAKE:{
			t_handshake* handshake = recibir_handshake(*socket_emisor, logger);
			/*SI SE CONECTA UN CLIENTE, RECIBO EL MENSAJE QUE QUIERA ENVIAR*/
			if(handshake->tipoProceso == CLIENTE){
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
		}
		break;
		default:
			log_error(logger, "[Atender_conexion] Tipo de mensaje no admitido.");
			close(*socket_emisor);
			break;
	}
	free(socket_emisor);
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

void crearServidor() {

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(config_get_string_value(config, "IP_APP"), config_get_string_value(config, "PUERTO_ESCUCHA"), &hints, &servinfo);

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
}

uint32_t generar_id_pedido(){

	uint32_t id_generado;

	pthread_mutex_lock(&mutex_id_rest_default);
	id_rest_default++;
	id_generado = id_rest_default;
	pthread_mutex_unlock(&mutex_id_rest_default);

	return id_generado;
}

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

void iniciar_conexion_escucha(t_info_restaurante* resto){

	while(1){

		int socket_comanda;
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(resto->socketEscucha, logger);
		if (tipo_mensaje == -1){
			log_warning(logger, "Se desconecto del restaurante %s.", resto->id);
			printf("Se desconecto del restaurante %s.", resto->id);
			close(resto->socketEscucha);
			pthread_exit(NULL);
		}

		switch(tipo_mensaje){

			case PLATO_LISTO:{
				t_plato_listo* platoListo = recibir_plato_listo(resto->socketEscucha, logger);
				log_trace(logger, "[PLATO_LISTO] Restaurante: %s, ID_Pedido: %d, Plato: %s.", platoListo->restaurante, platoListo->id_pedido, platoListo->plato);
				enviar_entero(RTA_PLATO_LISTO, OK, resto->socketEscucha, logger);

				// Informo a Comanda sobre PLATO_LISTO
				socket_comanda = conectar_a_comanda_simple();
				enviar_plato_listo(platoListo, socket_comanda, logger);
				t_tipoMensaje tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
				if (tipo_rta == RTA_PLATO_LISTO){
					uint32_t resultado = recibir_entero(socket_comanda, logger);
					log_trace(logger, "[PLATO_LISTO] Se envio a Comanda la actualizacion. Resultado: %s.", resultado? "OK":"FAIL");
				}
				close(socket_comanda);

				// Valido con Comanda si el pedido está finalizado
				t_rta_obtener_pedido* respuesta;
				t_obtener_pedido* msg_obtener_pedido = calloc(1,sizeof(t_obtener_pedido));
				strcpy(msg_obtener_pedido->restaurante, platoListo->restaurante);
				msg_obtener_pedido->id_pedido = platoListo->id_pedido;
				socket_comanda = conectar_a_comanda_simple();
				enviar_obtener_pedido(msg_obtener_pedido, socket_comanda, logger);
				free(msg_obtener_pedido);
				tipo_rta = recibir_tipo_mensaje(socket_comanda, logger);
				if (tipo_rta == RTA_OBTENER_PEDIDO)
					respuesta = recibir_rta_obtener_pedido(socket_comanda, logger);
				if(respuesta->estado == TERMINADO) {
					// HABILITAR EL PEDIDO/PCB PARA SER RETIRADO
					log_trace(logger, "[PEDIDO_TERMINADO] Se obtuvo de Comanda: Restaurante: %s, Pedido: %d.",platoListo->restaurante, platoListo->id_pedido);
					//FIX GONZA: TOMABA EL PRIMER PEDIDO,PERO SE PUEDE REPETIR EL ID_PEDIDO PARA OTRO RESTAURANTE
					notificar_pedido_listo(platoListo->id_pedido,platoListo->restaurante);
				}
				close(socket_comanda);
				list_destroy_and_destroy_elements(respuesta->comidas, free);
				free(respuesta);
				free(platoListo);
			}
			break;
			default:
				log_error(logger, "[ACTUALIZACIONES] No se reconoce el tipo de mensaje recibido.");
				break;
		}
	}
}
