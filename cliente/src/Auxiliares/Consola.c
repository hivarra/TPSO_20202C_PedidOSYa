/*
 * Consola.c
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#include "Consola.h"

char* comandos_str[] = { "ejecutar", "status", "finalizar", "metricas", "salir", NULL };

void leer_consola() {
	socket_envio = conectar_a_server_de_config();
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
void crear_hilo_recepcion_mensajes(){
	pthread_t hilo_recepcion;
	pthread_create(&hilo_recepcion,NULL,(void*)inicializar_conexion_recepcion,NULL);
	pthread_detach(hilo_recepcion);
}
void inicializar_conexion_recepcion(){
	socket_recepcion = conectar_a_server_de_config();

	while(1){
		t_mensaje* msg = recibirMensaje(socket_recepcion,logger);

		switch(msg->header.tipoMensaje){
			case RESPUESTA_OK_FAIL:;
				break;
			default:;
				log_info(logger, "No se reconoce el tipo de mensaje recibido");
				break;
		}
		destruirMensaje(msg);
	}
}
int conectar_a_server_de_config(){
	t_tipoProceso proceso_enum = tipo_proceso_string_to_enum(cliente_config.proceso_server);

	switch(proceso_enum){
	case APP :;
		socket_envio = conectar_a_servidor(cliente_config.ip_app, cliente_config.puerto_app, CLIENTE, APP, logger);
		printf("Se creo la conexión con APP. Socket: %d\n", socket_envio);
		break;
	case COMANDA :;
		socket_envio = conectar_a_servidor(cliente_config.ip_comanda, cliente_config.puerto_comanda, CLIENTE, COMANDA, logger);
		printf("Se creo la conexión con COMANDA. Socket: %d\n", socket_envio);
		break;
	case SINDICATO :;
		socket_envio = conectar_a_servidor(cliente_config.ip_sindicato, cliente_config.puerto_sindicato, CLIENTE, SINDICATO, logger);
		printf("Se creo la conexión con SINDICATO. Socket: %d\n", socket_envio);
		break;
	default:;
		log_info(logger, "El proceso al que se intenta conectar no existe capo...");
		break;
	}
	if(socket_envio<0)
		log_error(logger,"Error al conectarse a %s", cliente_config.proceso_server);

	return socket_envio;
}
void procesar_solicitud(char** parametros){
	t_tipoMensaje mensaje_enum = tipo_mensaje_string_to_enum(parametros[1]);
	t_tipoProceso proceso_enum = tipo_proceso_string_to_enum(cliente_config.proceso_server);

	log_info(logger, "Mensaje a enviar: %s\n", parametros[1]);

	switch(mensaje_enum) {
		case CONSULTAR_RESTAURANTES:;
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
			t_guardar_pedido* guardar_pedido = malloc(sizeof(t_guardar_pedido));
			strcpy(guardar_pedido->nombre_restaurante, parametros[0]);
			guardar_pedido->id_pedido = atoi(parametros[1]);

			enviarMensaje(CLIENTE, mensaje_enum, sizeof(t_guardar_pedido), guardar_pedido, socket_envio, proceso_enum, logger);

			free(guardar_pedido);

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
			close(socket_envio);
			close(socket_recepcion);
			break;
		case TERMINAR_PEDIDO:;
			break;
		case SALIR:;
			puts("Muchas gracias por utilizar PedidOS Ya!. Vuelva pronto!\n");
			break;
		default:;
			printf("No se reconoce el mensaje %s .\n", parametros[1]);
			break;
	}
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
		printf("Parametro %d:%s\n", i, parametros[i]);
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