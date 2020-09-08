/*
 ============================================================================
 Name        : sindicato.c
 Author      : thread_away
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "sindicato.h"

int main(int argc, char **argv) {

	puts("Inicio Proceso sindicato");

	/* 0. Setear config path */
	char* path_config = getConfigPath(argv[1]);

	/* 1. Configuración */
	cargarConfigSindicato(path_config);

	/* 2. Log */
	char* path_log = getLogPath("sindicato.log");
	logger = configurar_logger(path_log, "Sindicato");


	puts("Fin Proceso Gamecard");
	return EXIT_SUCCESS;
}
