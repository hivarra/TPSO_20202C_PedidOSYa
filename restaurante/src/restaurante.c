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

int main(int argc, char **argv) {
	puts("Inicio RESTAURANTE");
	/* 0. Setear config path */
	char* path_config = getConfigPath(argv[1]);
	/* 1. Configuración */
	cargar_configuracion_restaurante(path_config);
	/* 2. Log */
	char* path_log = getLogPath(restaurante_conf.archivo_log);
	logger = configurar_logger(path_log, "restaurante");
	mostrar_propiedades();
	/* 3. Conexion*/
	/*TODO:Conectarse a app ysindicato*/
	conectar_a_app();
	/*TODO:Quedarse escuchando peticiones de cliente*/

	sleep(20);
	destruir_logger(logger);
	puts("Fin RESTAURANTE");
	return EXIT_SUCCESS;
}
