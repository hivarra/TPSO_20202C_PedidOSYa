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

	/*Extra. Liberar bien con ctrl+c*/
	signal(SIGINT, &signalHandler);

	/* 5. Escuchando conexiones*/
	escuchar_conexiones_sindicato();

	return EXIT_FAILURE;
}

void signalHandler(int sig){

	void _destruir_semaforos_pedidos(pthread_mutex_t* semaforo){
		pthread_mutex_destroy(semaforo);
		free(semaforo);
	}

	close(socket_servidor);

	//bitarray_destroy(bitmap);
	pthread_mutex_destroy(&mutex_bitmap);
	pthread_mutex_destroy(&mutexSemaforosPedidos);
	dictionary_destroy_and_destroy_elements(semaforos_pedidos, (void*)_destruir_semaforos_pedidos);
	munmap(bmap, cantidad_bloques/8);

	free(ruta_files);
	free(ruta_bloques);
	free(ruta_restaurantes);
	free(ruta_recetas);

	destruir_config(config);
	destruir_logger(logger);

	puts("Fin Proceso SINDICATO");

	exit(EXIT_SUCCESS);
}
