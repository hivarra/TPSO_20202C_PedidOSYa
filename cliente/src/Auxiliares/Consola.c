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

int procesar_comando(char *line) {

	printf("Se ingresó: %s\n", line);

	char* comando = strtok(line, " ");
	char* parametro;

	t_tipoMensaje mensaje_e = tipo_mensaje_string_to_enum(comando);
	if (mensaje_e == -1) {
		log_error(logger, "CONSOLA: Se ingresó un comando desconocido: %s.", comando);
	} else {
		log_info(logger, "CONSOLA: Se ejecutó el comando %s.", comando);
	}

	printf("Valor del enum: %d\n", mensaje_e);

	switch(mensaje_e) {
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
			break;
		case TERMINAR_PEDIDO:;
			break;
		case SALIR:;
			puts("Muchas gracias por utilizar PedidOS Ya!. Vuelva pronto!\n");
			return -1;
		default:;
			printf("No se reconoce el comando %s .\n", comando);

	}

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
