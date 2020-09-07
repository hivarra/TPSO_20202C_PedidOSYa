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

int main(int argc, char **argv) {

	//	printf("Checking si linkea shared lib...\npor ej. ParentPath:%s\n", getParentPath());

	configurarProceso(argv);

	crearConsola();

	return EXIT_SUCCESS;
}

void configurarProceso(char **argv) {

	/* 0. Setear config path */
	char* path_config = getConfigPath(argv[1]);

	/* 1. Configuración */
	cargarConfigCliente(path_config);

	/* 2. Log */
	char* path_log = getLogPath(cliente_config.archivo_log);
	logger = configurar_logger(path_log, "cliente");

}
