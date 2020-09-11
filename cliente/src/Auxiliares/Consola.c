/*
 * Consola.c
 *
 *  Created on: 6 sep. 2020
 *      Author: utnso
 */

#include "Consola.h"

char* comandos_str[] = { "ejecutar", "status", "finalizar", "metricas", "salir", NULL };

int crearConsola() {

	char * line;
	rl_attempted_completion_function = character_name_completion;

	int estado;

	while (1) {
		line = readline("Ingrese un comando> ");
		if (line) {
			add_history(line);
		}

		estado = procesar_comando(line);

		if (estado != 0) {
			return EXIT_FAILURE;
		}
	}
}

void procesar_mensaje(char** parametros){
	int socket = inicializar_conexion(parametros[0]);
	procesar_solicitud(socket, parametros);
}

int inicializar_conexion(char* proceso){
	t_tipoProceso proceso_enum = tipo_proceso_string_to_enum(proceso);
	int socket;

	switch(proceso_enum){
	case APP :;
		socket = conectar_a_servidor(cliente_config.ip_app, cliente_config.puerto_app, CLIENTE, APP, logger);
		printf("Se creo la conexión con APP. Socket: %d\n", socket);
		break;
	case COMANDA :;
		socket = conectar_a_servidor(cliente_config.ip_comanda, cliente_config.puerto_comanda, CLIENTE, COMANDA, logger);
		printf("Se creo la conexión con COMANDA. Socket: %d\n", socket);
		break;
	case SINDICATO :;
		socket = conectar_a_servidor(cliente_config.ip_sindicato, cliente_config.puerto_sindicato, CLIENTE, SINDICATO, logger);
		printf("Se creo la conexión con SINDICATO. Socket: %d\n", socket);
		break;
	default:;
		log_info(logger, "El proceso al que se intenta conectar no existe capo...");
		break;
	}
	return socket;
}

void procesar_solicitud(int socket, char** parametros){
	t_tipoMensaje mensaje_enum = tipo_mensaje_string_to_enum(parametros[1]);

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
			close(socket);
			break;
		case TERMINAR_PEDIDO:;
			break;
		case SALIR:;
			puts("Muchas gracias por utilizar PedidOS Ya!. Vuelva pronto!\n");
			break;
//			return -1;
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
	if (validar(parametros[0])){
		log_info(logger, "Proceso inválido");
		return -1;
	}
	if(validar(parametros[1])){
		log_info(logger, "Mensaje inválido");
		return -1;
	}
	pthread_t hilo_envio_mensajes;
	pthread_create(&hilo_envio_mensajes, NULL, (void*)procesar_mensaje, parametros);

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
