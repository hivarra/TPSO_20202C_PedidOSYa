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
	puts("Inicio COMANDA\n");

	/* 1. Configuración */
	char* path_config = getConfigPath(argv[1]);
	cargar_configuracion_comanda(path_config);
	free(path_config);

	/* 2. Log */
	cargar_logger_comanda();
	mostrar_propiedades();

	/* 3. Inicializo memoria*/
	inicializar_memoria();
	init_bitmap_mp();
	inicializar_memoria_swap();
	init_bitmap_ms();

	/*Extra. Liberar bien con ctrl+c*/
	signal(SIGINT, &signalHandler);

	/* 4. Escuchando conexiones*/
	escuchar_conexiones_comanda();
	//escuchar__conexiones_comanda();

	return EXIT_FAILURE;
}
