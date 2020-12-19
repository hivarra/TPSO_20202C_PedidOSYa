/*
 ============================================================================
 Name        : restaurante.c
 Author      : thread_away
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "restaurante.h"

int main(int argc, char *argv[]) {
	puts("===================\nInicio RESTAURANTE\n===================");

	/* 1. Configuración */
	char* path_config = getConfigPath(argv[1]);
	cargar_configuracion_restaurante(path_config);
	free(path_config);

	/* 2. Log */
	cargar_logger_restaurante();
	mostrar_propiedades();

	/* 3. Conexion*/
	conectar_a_sindicato();
	conectar_a_app();

	/* 4. Planificador*/
	inicializar_planificador();

	/*Extra. Liberar bien con ctrl+c*/
	signal(SIGINT, &signalHandler);

	/* 5. Servidor*/
	escuchar_clientes();

	return EXIT_FAILURE;
}
