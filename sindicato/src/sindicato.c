/*
 ============================================================================
 Name        : sindicato.c
 Author      : thread_away
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "sindicato.h"

int main(int argc, char **argv) {

	puts("Inicio Proceso SINDICATO");

	/* 0. Setear config path */
	char* path_config = getConfigPath(argv[1]);

	/* 1. Configuración */
	cargarConfigSindicato(path_config);

	/* 2. Log */
	char* path_log = getLogPath(PATH_LOG);
	logger = configurar_logger(path_log, "sindicato");
	mostrar_propiedades();
	/* 3. File System */
	montarFileSystem();
	/* 4. Inicializar conexiones*/
	/*TODO: levantar server para recibir peticiones de restaurantes y clientes*/

//	destruir_config(config);
//	destruir_logger(logger);

	puts("Fin Proceso SINDICATO");
	return EXIT_SUCCESS;
}
