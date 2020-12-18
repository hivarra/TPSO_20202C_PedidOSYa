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
	puts("================\nInicio APP\n================");
	/* 0. Setear config path */
	char* path_config = getConfigPath(argv[1]);
	/* 1. Configuración */
	cargar_configuracion_app(path_config);

	/* 2. Log */
	cargar_logger_app();
	mostrar_propiedades();

	/* 3. Conexion a Comanda */
	conectar_a_comanda();

	/* 4. Inicializaciones */
	inicializar();
	crear_hilo_PLP();
	crear_hilo_PCP();

	/*Extra. Liberar bien con ctrl+c*/
	signal(SIGINT, &signalHandler);

	/* 5. Servidor */
	crearServidor();

	return EXIT_FAILURE;
}

void inicializar() {

	iniciarListas();
	iniciarSemaforos();
	iniciarRepartidores();
	inicializarListaClientesRest();
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

		pthread_create(&repartidor->thread_repartidor, NULL, (void*)correr_repartidor, repartidor);
		pthread_detach(repartidor->thread_repartidor);

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

void signalHandler(int sig){
	liberar_lista(app_conf.repartidores);
	liberar_lista(app_conf.frecuencias_descanso);
	liberar_lista(app_conf.tiempos_descanso);
	liberar_lista(app_conf.platos_default);

	pthread_cancel(thread_PLP);
	pthread_cancel(thread_PCP);
	if(string_equals_ignore_case("HRRN", app_conf.algoritmo_planificacion))
		pthread_cancel(hilo_espera_cpu);

	void finalizar_repartidor(t_repartidor* repartidor){
		sem_destroy(&repartidor->sem_moverse);
		pthread_cancel(repartidor->thread_repartidor);
	}
	list_iterate(repartidores, (void*)finalizar_repartidor);
	puts("\nFinishing threads...");
	list_destroy_and_destroy_elements(repartidores, free);

	list_destroy_and_destroy_elements(pedidos_planificables, free);
	list_destroy_and_destroy_elements(listos, free);
	list_destroy_and_destroy_elements(ejecutando, free);
	list_destroy_and_destroy_elements(bloqueados, free);
	list_destroy_and_destroy_elements(finalizados, free);

	sem_destroy(&sem_repartidor_disponible);
	sem_destroy(&sem_pedidos);
	sem_destroy(&sem_ready);
	sem_destroy(&sem_limite_exec);

	pthread_mutex_destroy(&mutex_nuevos);
	pthread_mutex_destroy(&mutex_listos);
	pthread_mutex_destroy(&mutex_ejecutando);
	pthread_mutex_destroy(&mutex_bloqueados);
	pthread_mutex_destroy(&mutex_finalizados);
	pthread_mutex_destroy(&mutexClientes);
	pthread_mutex_destroy(&mutexRestaurantes);
	pthread_mutex_destroy(&mutex_id_rest_default);

	void destruir_cliente(t_info_cliente* cliente){
		close(cliente->socketEscucha);
		list_destroy_and_destroy_elements(cliente->pedidos,free);
		free(cliente);
	}
	void destruir_restaurante(t_info_restaurante* resto){
		close(resto->socketEscucha);
		close(resto->socketEnvio);
		free(resto);
	}
	list_destroy_and_destroy_elements(clientesConectados, (void*)destruir_cliente);
	list_destroy_and_destroy_elements(restaurantesConectados, (void*)destruir_restaurante);
	free(infoRestoDefault);

	close(socket_app);

	destruir_logger(logger);
	destruir_config(config);

	sleep(2);

	puts("\n================\nFin APP\n================");
	exit(EXIT_SUCCESS);
}
