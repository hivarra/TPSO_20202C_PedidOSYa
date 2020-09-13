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

int main(void) {

	configurarProceso();

	crear_hilo_recepcion_mensajes();

	leer_consola();

	return EXIT_SUCCESS;
}

void configurarProceso() {

	/* 0. Configuración */
	cargarConfigCliente();

	/* 1. Log */
	logger = configurar_logger(cliente_config.archivo_log, "cliente");

}
