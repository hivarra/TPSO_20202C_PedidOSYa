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

	char* path_config = getConfigPath(argv[1]);
	configurarProceso(path_config);
	free(path_config);

	inicializar_conexion();

	leer_consola();

	return EXIT_SUCCESS;
}

void configurarProceso(char* path_config) {
	/* 0. Configuración */
	cargarConfigCliente(path_config);
	/* 1. Log */
	cargar_logger_cliente();
}
