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

	//	printf("Checking si linkea shared lib...\npor ej. ParentPath:%s\n", getParentPath());

	configurarProceso();

	crearConsola();

	return EXIT_SUCCESS;
}

void configurarProceso() {

	/* 0. Configuración */
	cargarConfigCliente();

	/* 1. Log */
	logger = configurar_logger(cliente_config.archivo_log, "cliente");

}
