/*
 ============================================================================
 Name        : cliente.c
 Author      : thread_away
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "cliente.h"

int main(int argc, char *argv[]) {
	puts("================\nInicio CLIENTE\n================");

	char* path_config = getConfigPath(argv[1]);
	configurarProceso(path_config);
	free(path_config);

	inicializar_conexion();

	/*Extra. Liberar bien con ctrl+c*/
	signal(SIGINT, &signalHandler);

	leer_consola();

	return EXIT_SUCCESS;
}

void configurarProceso(char* path_config) {
	/* 0. Configuración */
	cargarConfigCliente(path_config);
	/* 1. Log */
	cargar_logger_cliente();
}

void signalHandler(int sig){
	terminar_programa(0);
}
