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
	cargar_logger_sindicato();
	mostrar_propiedades();

	/* 3. File System */
	montarFileSystem();

	/* 4. Crear hilo para consola */
	pthread_create(&thread_consola, NULL, (void*)crear_consola, NULL);
	pthread_detach(thread_consola);

	/* 5. Escuchando conexiones*/
	escuchar_conexiones_sindicato();

	destruir_config(config);
	destruir_logger(logger);

	puts("Fin Proceso SINDICATO");
	return EXIT_SUCCESS;
}
