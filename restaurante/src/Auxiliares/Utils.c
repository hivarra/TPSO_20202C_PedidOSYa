/*
 * Utils.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Utils.h"

void signalHandler(int sig){

	destruir_logger(logger);
	destruir_config(config);

	if(socket_servidor != -1)
		close(socket_servidor);

	list_destroy_and_destroy_elements(clientes_conectados, free);

	puts("Fin RESTAURANTE");

	exit(EXIT_SUCCESS);
}
