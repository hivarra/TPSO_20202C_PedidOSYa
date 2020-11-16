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

	inicializar();
	/* 3. Conexion*/
	/*TODO:Conectarse a comanda*/

	/*TODO:Quedarse escuchando peticiones de cliente y repartidores*/
	pthread_create(&hilo_servidor,NULL,(void*)crearServidor,NULL);

//	inicializar();

	pthread_join(hilo_servidor, NULL);
	destruir_logger(logger);
	destruir_config(config);
	puts("Fin APP");
	return EXIT_SUCCESS;
}

void inicializar() {

	iniciarListas();
	inicializarListaClientesRest();
	incializarRestoDefault();
	iniciarSemaforos();
//	iniciarRestauranteDefault();
	iniciarRepartidores();
}

void iniciarListas() {
	//restaurantes = list_create();
}

void iniciarSemaforos() {

	pthread_mutex_init(&mutexClientes, NULL);
	pthread_mutex_init(&mutexRestaurantes, NULL);
}

//void iniciarRestauranteDefault() {
//
//	restaurante_default = malloc(sizeof(t_restaurante));
//	strcpy(restaurante_default->nombre, "DEFAULT");
//	restaurante_default->posX = app_conf.pos_rest_default_x;
//	restaurante_default->posY = app_conf.pos_rest_default_y;
//}

void iniciarRepartidores() {

	int i = 0;
	while(app_conf.repartidores[i] != NULL) {

		char** coordenada = string_split(app_conf.repartidores[i], "|");

		t_repartidor* repartidor = malloc(sizeof(t_repartidor));
		repartidor->id = i + 1;
		repartidor->posX = atoi(coordenada[0]);
		repartidor->posY = atoi(coordenada[1]);
		repartidor->frecuenciaDescanso = atoi(app_conf.frecuencias_descanso[i]);
		repartidor->tiempoDescanso = atoi(app_conf.tiempos_descanso[i]);
		liberar_lista(coordenada);
		i++;

		imprimirRepartidor(repartidor);
	}

}

void imprimirRepartidor(t_repartidor* repartidor) {

	log_info(logger, "Repartidor N° %d | PosX: %d | PosY: %d | Descanso: %d | Duración: %d", repartidor->id, repartidor->posX, repartidor->posY, repartidor->frecuenciaDescanso, repartidor->tiempoDescanso);

}
