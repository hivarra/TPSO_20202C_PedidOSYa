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
t_tipoRespuesta recibir_tipo_respuesta(int socket_cliente) {
	t_tipoRespuesta tipo_respuesta;

	if (recv(socket_cliente, &tipo_respuesta, sizeof(t_tipoRespuesta), MSG_WAITALL) < 0) {
		log_error(logger, "Error al recibir tipo de mensaje de socket:%d", socket_cliente);
		close(socket_cliente);
		pthread_exit(NULL);
	}
	return tipo_respuesta;
}
void inicializar_conexion_recepcion(int* socket){
	socket_recepcion = conectar_a_server();
	enviarMensaje(SOCKET_ESCUCHA, &cliente_config.id_cliente,socket_recepcion, logger);

	while(1){
		t_tipoRespuesta tipo_respuesta = recibir_tipo_respuesta(socket_recepcion);
		log_info(logger,"PRUEBA:%d",tipo_respuesta);
		log_info(logger, "LLEGO ACA");

		switch(tipo_respuesta){
			case RTA_CONSULTAR_RESTAURANTES:;
				t_rta_consultar_restaurantes* rta_consultar_restaurantes = recibirRespuesta(socket_recepcion,RTA_CONSULTAR_RESTAURANTES,logger);
				log_info(logger,"CANTIDAD_RESTAURANTES:%d" ,rta_consultar_restaurantes->cantRestaurantes);
				imprimir_lista_strings(rta_consultar_restaurantes->restaurantes,"NOMBRES_RESTAURANTES");
				free(rta_consultar_restaurantes);
				break;
			default:;
				log_info(logger, "No se reconoce el tipo de respuesta recibida");
				break;
		}
	}
}
void crear_hilo_recepcion_mensajes(){
	pthread_t hilo_recepcion;
	pthread_create(&hilo_recepcion,NULL,(void*)inicializar_conexion_recepcion,NULL);
	pthread_detach(hilo_recepcion);
}
void  inicializar_conexion_envio(){
	socket_envio = conectar_a_server();
	enviarMensaje(SOCKET_ENVIO,&cliente_config.id_cliente,socket_envio,logger);
}
void crear_hilo_conexion_envio(){
	pthread_t hilo_envio;
	pthread_create(&hilo_envio,NULL,(void*)inicializar_conexion_envio,NULL);
	pthread_detach(hilo_envio);
}
int conectar_a_server(){
	int socket_aux;

	socket_aux = conectar_a_servidor(cliente_config.ip, cliente_config.puerto, cliente_config.id_cliente, CLIENTE, APP, logger);
	log_info(logger, "Conectando con servidor. Socket: %d\n", socket_aux);

	if(socket_aux<0){
		log_error(logger,"Error al conectarse a servidor.");
	}

	return socket_aux;
}
void procesar_solicitud(char** parametros){
	t_tipoMensaje mensaje_enum = tipo_mensaje_string_to_enum(parametros[0]);
	log_info(logger, "Mensaje a enviar: %s\n", parametros[0]);

	switch(mensaje_enum) {
		case CONSULTAR_RESTAURANTES:;
			int ret = enviarMensaje(CONSULTAR_RESTAURANTES, NULL, socket_envio,logger);
			log_info(logger,"PRUEBA RETORNO:%d",ret);

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
			printf("No se reconoce el mensaje %s .\n", parametros[0]);
			break;
	}
	log_info(logger, "Mensaje enviado: %s\n", parametros[0]);
}
int validar(char* parametro){
	return parametro==NULL;
}
int procesar_comando(char *line) {
	printf("Se ingresó: %s\n", line);
	char** parametros = string_n_split(line,string_length(line), " ");
	/*TODO:liberar parametros*/
	int i=0;
	while(parametros[i]!=NULL){
		printf("Parametro %d: %s\n", i, parametros[i]);
		i++;
	}
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
