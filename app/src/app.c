/*
 ============================================================================
 Name        : app.c
 Author      : thread_away
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "app.h"

int main(int argc, char **argv)  {
	puts("Inicio APP");
	/* 0. Setear config path */
	char* path_config = getConfigPath(argv[1]);
	/* 1. Configuración */
	cargar_configuracion_app(path_config);
	/* 2. Log */
	char* path_log = getLogPath(app_conf.archivo_log);
	logger = configurar_logger(path_log, "app");
	mostrar_propiedades();
	/* 3. Conexion*/
	/*TODO:Conectarse a comanda*/
	/*TODO:Quedarse escuchando peticiones de cliente y repartidores*/

	destruir_logger(logger);
	destruir_config(config);
	puts("Fin APP");
	return EXIT_SUCCESS;
}
