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

int main(int argc, char **argv) {
	puts("Inicio COMANDA");
	/* 0. Setear config path */
	char* path_config = getConfigPath(argv[1]);
	/* 1. Configuración */
	cargar_configuracion_comanda(path_config);
	/* 2. Log */
	cargar_logger_comanda();
	/* 3. Inicializo memoria*/
//	inicializar_MQ();
	/* 4. Escuchando conexiones*/
//	escuchar_conexiones();

	destruir_logger(logger);

	puts("Fin COMANDA");
	return EXIT_SUCCESS;
}
