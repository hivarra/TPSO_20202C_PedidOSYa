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
	mostrar_propiedades_pantalla();
	/* 2. Log */
	cargar_logger_app();

	/* 3. Conexion a Comanda */
	conectar_a_comanda();

	/* 4. Inicializaciones */
	inicializar();
	crear_hilo_PLP();
	crear_hilo_PCP();

//	prueba_planificacion();

	/* 5. Servidor */
	crearServidor();

	destruir_logger(logger);
	destruir_config(config);
	puts("Fin APP");
	return EXIT_SUCCESS;
}

void inicializar() {

	iniciarListas();
	iniciarSemaforos();
	iniciarRepartidores();
}

void iniciarListas() {
	repartidores = list_create();
	pedidos_planificables = list_create();
	listos = list_create();
	ejecutando = list_create();
	bloqueados = list_create();
	finalizados = list_create();
}

void iniciarSemaforos() {

	sem_init(&sem_repartidor_disponible, 0, 0);
	sem_init(&sem_pedidos, 0, 0);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_limite_exec, 0, app_conf.grado_multiprocesamiento);

	pthread_mutex_init(&mutex_nuevos, NULL);
	pthread_mutex_init(&mutex_listos, NULL);
	pthread_mutex_init(&mutex_ejecutando, NULL);
	pthread_mutex_init(&mutex_bloqueados, NULL);
	pthread_mutex_init(&mutex_finalizados, NULL);
	pthread_mutex_init(&mutexClientes, NULL);
	pthread_mutex_init(&mutexRestaurantes, NULL);
}

void iniciarRepartidores() {

	int i = 0;
	while(app_conf.repartidores[i] != NULL) {
		pthread_t thread_repartidor;

		char** coordenada = string_split(app_conf.repartidores[i], "|");

		t_repartidor* repartidor = malloc(sizeof(t_repartidor));
		repartidor->id = i + 1;
		repartidor->posX = atoi(coordenada[0]);
		repartidor->posY = atoi(coordenada[1]);
		repartidor->frecuenciaDescanso = atoi(app_conf.frecuencias_descanso[i]);
		repartidor->tiempoDescanso = atoi(app_conf.tiempos_descanso[i]);
		sem_init(&repartidor->sem_moverse, 0, 0);
		repartidor->objetivo_posX = -1;
		repartidor->objetivo_posY = -1;
		repartidor->quantum = -1;
		disponibilizar_repartidor(repartidor);
		list_add(repartidores, repartidor);

		pthread_create(&thread_repartidor, NULL, (void*) correr_repartidor, repartidor);
		pthread_detach(thread_repartidor);

		liberar_lista(coordenada);
		i++;
	}

}

void crear_hilo_PLP() {

	int hilo_plp = pthread_create(&thread_PLP, NULL, (void*)planificador_largo_plazo, NULL);
	pthread_detach(thread_PLP);

	if (hilo_plp == -1)
		log_error(logger, "No se pudo generar el hilo PLP");
}

void crear_hilo_PCP(){

	if(string_equals_ignore_case("FIFO", app_conf.algoritmo_planificacion)) {

		int hilo_pcp = pthread_create(&thread_PCP, NULL, (void*)planificador_fifo, NULL);
		pthread_detach(thread_PCP);

		if (hilo_pcp == -1)
			log_error(logger, "No se pudo generar el hilo PCP");
	}

	else if(string_equals_ignore_case("HRRN", app_conf.algoritmo_planificacion)) {

		int hilo_pcp = pthread_create(&thread_PCP, NULL, (void*)planificador_hrrn, NULL);
		pthread_detach(thread_PCP);

		if (hilo_pcp == -1)
			log_error(logger, "No se pudo generar el hilo PCP");
	}

	else if(string_equals_ignore_case("SJF", app_conf.algoritmo_planificacion)) {

		int hilo_pcp = pthread_create(&thread_PCP, NULL, (void*)planificador_sjf, NULL);
		pthread_detach(thread_PCP);

		if (hilo_pcp == -1)
			log_error(logger, "No se pudo generar el hilo PCP");
	}

}
