/*
 * Consola.c
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#include "Consola.h"

char* comandos_str[] = { "ejecutar", "status", "finalizar", "metricas", "salir", NULL };

void leer_consola() {
	_leer_consola();
}
void _leer_consola(){
	char* line = readline(">");

	while(strncmp(line, "", 1) != 0) {
			procesar_comando(line);
			free(line);
			line = readline(">");
	}
	free(line);
}
void inicializar_conexion_escucha(int* socket){
	socket_escucha = conectar_a_server();

	t_socket_escucha* socket_escucha_info = malloc(sizeof(t_socket_escucha));
	strcpy(socket_escucha_info->id,cliente_config.id_cliente);
	socket_escucha_info->tipoProceso = CLIENTE;
	enviar_socket_escucha(socket_escucha_info,socket_escucha,logger);
	free(socket_escucha_info);

	while(1){
		t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_escucha,logger);

		switch(tipo_mensaje){
			case RTA_CONSULTAR_RESTAURANTES:;
				t_rta_consultar_restaurantes* rta_consultar_restaurantes = recibir_rta_consultar_restaurantes(socket_escucha,logger);
				log_info(logger,"CANTIDAD_RESTAURANTES:%d",rta_consultar_restaurantes->cantRestaurantes);
				imprimir_lista_strings(rta_consultar_restaurantes->restaurantes,"RESTAURANTES");

				break;
			default:;
				log_info(logger, "No se reconoce el tipo de respuesta recibida");
				break;
		}
	}
}
void crear_hilo_conexion_escucha(){
	pthread_t hilo_escucha;
	pthread_create(&hilo_escucha,NULL,(void*)inicializar_conexion_escucha,NULL);
	pthread_detach(hilo_escucha);
}
int conectar_a_server(){
	int socket_aux;

	socket_aux = conectar_a_servidor(cliente_config.ip, cliente_config.puerto,logger);
	log_info(logger, "Conectando con servidor. Socket: %d\n", socket_aux);

	if(socket_aux<0){
		log_error(logger,"Error al conectarse a servidor.");
		puts("No se pudo conectar a proceso");
		return EXIT_FAILURE;
	}
	return socket_aux;
}
void inicializar_conexion(){
	/*REALIZO HANDSHAKE*/
	int socket_handshake = conectar_a_server();
	enviar_mensaje_vacio(HANDSHAKE, socket_handshake, logger);
	t_tipoMensaje tipo_mensaje = recibir_tipo_mensaje(socket_handshake,logger);
	if(tipo_mensaje == RTA_HANDSHAKE){
		log_info(logger,"Se recibe respuesta %s",get_nombre_mensaje(tipo_mensaje));
		uint32_t respuesta_entero = recibir_entero(socket_handshake,logger);
		tipo_proceso_server = respuesta_entero;
		log_info(logger,"FIN HANDSHAKE CON PROCESO: %s",get_nombre_proceso(tipo_proceso_server));
	}
	close(socket_handshake);
	/*SI ME CONECTO A APP/RESTAURANTE, ENVIO SOCKET_ENVIO Y SOCKET_ESCUCHA*/
	if(tipo_proceso_server == APP || tipo_proceso_server == RESTAURANTE){
		socket_envio = conectar_a_server();
		t_socket_envio* param_socket_envio = malloc(sizeof(t_socket_envio));
		strcpy(param_socket_envio->id,cliente_config.id_cliente);
		param_socket_envio->posX = cliente_config.posicion_x;
		param_socket_envio->posY = cliente_config.posicion_y;
		param_socket_envio->tipoProceso = CLIENTE;
		enviar_socket_envio(param_socket_envio,socket_envio,logger);
		free(param_socket_envio);
		crear_hilo_conexion_escucha();
	}
	/*SI ME CONECTO A COMANDA/SINDICATO, ME COMUNICO POR SOCKET_UNIDIRECCIONAL*/
	if(tipo_proceso_server == COMANDA || tipo_proceso_server == SINDICATO){
		socket_unidireccional = conectar_a_server();
	}
}
void procesar_solicitud_app_restaurante(char** parametros){
	t_tipoMensaje tipo_mensaje = tipo_mensaje_string_to_enum(parametros[0]);
	log_info(logger, "Mensaje a enviar: %s\n", parametros[0]);

	switch(tipo_mensaje) {
		case CONSULTAR_RESTAURANTES:;
			enviar_mensaje_vacio(CONSULTAR_RESTAURANTES,socket_envio,logger);
			break;
		case SELECCIONAR_RESTAURANTE:;
			break;
		case OBTENER_RESTAURANTE:;
			break;
		case CONSULTAR_PLATOS:;
			break;
		case CREAR_PEDIDO:;
			break;
		case GUARDAR_PEDIDO:;
			break;
		case ANADIR_PLATO:;
			break;
		case GUARDAR_PLATO:;
			break;
		case CONFIRMAR_PEDIDO:;
			break;
		case PLATO_LISTO:;
			break;
		case CONSULTAR_PEDIDO:;
			break;
		case OBTENER_PEDIDO:;
			break;
		case FINALIZAR_PEDIDO:;
			break;
		case TERMINAR_PEDIDO:;
			break;
		default:;
			printf("No se reconoce el mensaje.\n");
			break;
	}
	log_info(logger, "Mensaje enviado: %s\n", get_nombre_mensaje(tipo_mensaje));
}
void procesar_solicitud_comanda_sindicato(char** parametros){
	t_tipoMensaje tipo_mensaje = tipo_mensaje_string_to_enum(parametros[0]);
	log_info(logger, "Mensaje a enviar: %s\n", get_nombre_mensaje(tipo_mensaje));

	switch(tipo_mensaje) {
		case CONSULTAR_RESTAURANTES:;
			enviar_mensaje_vacio(CONSULTAR_RESTAURANTES,socket_unidireccional,logger);
			t_rta_consultar_restaurantes* rta_consultar_restaurantes = recibir_rta_consultar_restaurantes(socket_unidireccional,logger);
			log_info(logger,"CANTIDAD_RESTAURANTES:%d",rta_consultar_restaurantes->cantRestaurantes);
			imprimir_lista_strings(rta_consultar_restaurantes->restaurantes,"RESTAURANTES");

			break;
		case SELECCIONAR_RESTAURANTE:;
			break;
		case OBTENER_RESTAURANTE:;
			break;
		case CONSULTAR_PLATOS:;
			break;
		case CREAR_PEDIDO:;
			break;
		case GUARDAR_PEDIDO:;
			t_guardar_pedido* msg_guardar_pedido = malloc(sizeof(t_guardar_pedido));
			strcpy(msg_guardar_pedido->restaurante,parametros[1]);
			msg_guardar_pedido->id_pedido = atoi(parametros[2]);
			log_info(logger,"PARAMETROS QUE SE ENVIAN");
			log_info(logger,"ID_PEDIDO:%d",msg_guardar_pedido->id_pedido);
			log_info(logger,"NOMBRE_RESTAURANTE:%s",msg_guardar_pedido->restaurante);
			enviar_guardar_pedido(msg_guardar_pedido,socket_unidireccional,logger);
			free(msg_guardar_pedido);
			/*FIXEAR ESTA PARTE*/
			uint32_t resultado = recibir_entero(socket_unidireccional,logger);
			log_info(logger,"RESULTADO:%s",resultado? "OK":"FAIL");

			break;
		case ANADIR_PLATO:;
			break;
		case GUARDAR_PLATO:;
			break;
		case CONFIRMAR_PEDIDO:;
			break;
		case PLATO_LISTO:;
			break;
		case CONSULTAR_PEDIDO:;
			break;
		case OBTENER_PEDIDO:;
			break;
		case FINALIZAR_PEDIDO:;
			break;
		case TERMINAR_PEDIDO:;
			break;
		default:;
			printf("No se reconoce el mensaje.\n");
			break;
	}
}
void procesar_solicitud(char** parametros){
	if(tipo_proceso_server == APP || tipo_proceso_server == RESTAURANTE)
		procesar_solicitud_app_restaurante(parametros);
	else if(tipo_proceso_server == COMANDA || tipo_proceso_server == SINDICATO)
		procesar_solicitud_comanda_sindicato(parametros);
}
int validar(char* parametro){
	return parametro==NULL;
}
int procesar_comando(char *line) {
	char** parametros = string_n_split(line,string_length(line), " ");
	/*TODO:liberar parametros*/
	if(validar(parametros[0])){
		log_info(logger, "Mensaje inválido");
		return -1;
	}
	procesar_solicitud(parametros);

	return 0;
}
// Auxiliares de Consola
char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}
char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "ejecutar", "status", "finalizar", "metricas",
			"salir", NULL };

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((name = character_names[list_index++])) {
		if (strncmp(name, text, len) == 0) {
			return strdup(name);
		}
	}
	return NULL;
}
