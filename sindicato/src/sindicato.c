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

	puts("================\nInicio SINDICATO\n================");

	/* 0. Setear config path */
	char* path_config = getConfigPath(argv[1]);

	/* 1. Configuración */
	cargarConfigSindicato(path_config);

	/* 2. Log */
	cargar_logger_sindicato();
	mostrar_propiedades();

	/* 3. File System */
	montarFileSystem();

	/*Extra. Liberar bien con ctrl+c*/
	signal(SIGINT, &signalHandler);

	/* 4. Escuchando conexiones*/
	pthread_create(&thread_server, NULL, (void*)escuchar_conexiones_sindicato, NULL);
	pthread_detach(thread_server);

	/* 5. Crear hilo para consola */
	crear_consola();
	signalHandler(SIGINT);

	return EXIT_FAILURE;
}

void signalHandler(int sig){

	void _destruir_semaforos_pedidos(pthread_mutex_t* semaforo){
		pthread_mutex_destroy(semaforo);
		free(semaforo);
	}

	pthread_cancel(thread_server);
	puts("\nFinishing listening thread...");
	sleep(2);

	close(socket_servidor);

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

	puts("================\nFin SINDICATO\n================");

	exit(EXIT_SUCCESS);
}
