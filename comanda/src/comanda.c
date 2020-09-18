/*
 ============================================================================
 Name        : comanda.c
 Author      : thread_away
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "comanda.h"

int main(int argc, char *argv[]) {
	puts("Inicio COMANDA");

	/* 1. Configuración */
	char* path_config = getConfigPath(argv[1]);
	cargar_configuracion_comanda(path_config);
	free(path_config);

	/* 2. Log */
	cargar_logger_comanda();
	mostrar_propiedades();

	/* 3. Inicializo memoria*/
	inicializar_memoria();

	/*Extra. Liberar bien con ctrl+c*/
	signal(SIGINT, &signalHandler);

	/* 4. Escuchando conexiones*/
	escuchar_conexiones();

	return EXIT_FAILURE;
}

void signalHandler(int sig){
	puts("Fin COMANDA");

	destruir_logger(logger);
	destruir_config(config);

	exit(EXIT_SUCCESS);
}
