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
//	mostrar_propiedades();

	inicializar();
	/* 3. Conexion*/
	conectar_a_comanda();

	/*TODO:Quedarse escuchando peticiones de cliente y repartidores*/
	pthread_create(&hilo_servidor,NULL,(void*)crearServidor,NULL);

//	inicializar();

	crear_hilo_PLP();
	crear_hilo_PCP();


//	prueba_planificacion();

	sleep(1000);
//	pthread_join(hilo_servidor, NULL);
	destruir_logger(logger);
	destruir_config(config);
	puts("Fin APP");
	return EXIT_SUCCESS;
}

// TODO: Inicio Prueba
void prueba_planificacion() {

	//TODO: Crear función para que genere un pcb/pedido a partir de un ID de pedido confirmado
	// Esta funcionalidad debería:
	// - Crear un t_pcb con id_repartidor = -1
	// - Mover el t_pcb a NEW
	// - Setear las coordenadas destino (del restaurante elegido por el cliente)
	// - Setear la instrucción: BUSCAR_PEDIDO

	// PEDIDO 1
	t_pcb* pcb1 = malloc(sizeof(t_pcb));

	pcb1->id_pedido = 1;
	pcb1->id_repartidor = -1;
	pcb1->instruccion = -1;
	pcb1->restaurante_posX = app_conf.pos_rest_default_x;
	pcb1->restaurante_posY = app_conf.pos_rest_default_y;
	pcb1->cliente_posX = 2;
	pcb1->cliente_posY = 2;
	list_add(pedidos_planificables, pcb1);
	sem_post(&sem_pedidos);
	log_info(logger, "Se recibió el pedido: %d", pcb1->id_pedido);
//	imprimirPCB(pcb1);

	//	PEDIDO 2
	t_pcb* pcb2 = malloc(sizeof(t_pcb));

	pcb2->id_pedido = 2;
	pcb2->id_repartidor = -1;
	pcb2->instruccion = -1;
	pcb2->restaurante_posX = 9;
	pcb2->restaurante_posY = 9;
	pcb2->cliente_posX = 6;
	pcb2->cliente_posY = 6;
	list_add(pedidos_planificables, pcb2);
	sem_post(&sem_pedidos);
	log_info(logger, "Se recibió el pedido: %d", pcb2->id_pedido);
//	imprimirPCB(pcb2);

	//	PEDIDO 3
	t_pcb* pcb3 = malloc(sizeof(t_pcb));

	pcb3->id_pedido = 3;
	pcb3->id_repartidor = -1;
	pcb3->instruccion = -1;
	pcb3->restaurante_posX = 1;
	pcb3->restaurante_posY = 1;
	pcb3->cliente_posX = 8;
	pcb3->cliente_posY = 9;
	list_add(pedidos_planificables, pcb3);
	sem_post(&sem_pedidos);
	log_info(logger, "Se recibió el pedido: %d", pcb3->id_pedido);
//	imprimirPCB(pcb3);

	sleep(20);
	log_info(logger, "LOS PEDIDOS 1 y 2 ESTAN LISTOS");


//	retirarPedido(pcb1);
//	retirarPedido(pcb2);
	pthread_t thread_retirar_pedido;
	pthread_create(&thread_retirar_pedido, NULL, (void*) retirarPedido, pcb1);
	pthread_create(&thread_retirar_pedido, NULL, (void*) retirarPedido, pcb2);

	sleep(40);
	log_info(logger, "EL PEDIDO 3 ESTA LISTO");
	pthread_create(&thread_retirar_pedido, NULL, (void*) retirarPedido, pcb3);
}

// TODO: Fin Prueba

void inicializar() {

	iniciarListas();
	inicializarListaClientesRest();
	incializarRestoDefault();
	iniciarSemaforos();
//	iniciarRestauranteDefault();
	iniciarRepartidores();
}

void iniciarListas() {
//	restaurantes = list_create();
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
//	pthread_mutex_init(&mutexClientesRestaurantes, 0);
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

	pthread_t thread_repartidor;
	int i = 0;
	while(app_conf.repartidores[i] != NULL) {

		char** coordenada = string_split(app_conf.repartidores[i], "|");

		t_repartidor* repartidor = malloc(sizeof(t_repartidor));
		repartidor->id = i + 1;
		repartidor->posX = atoi(coordenada[0]);
		repartidor->posY = atoi(coordenada[1]);
		repartidor->frecuenciaDescanso = atoi(app_conf.frecuencias_descanso[i]);
		repartidor->tiempoDescanso = atoi(app_conf.tiempos_descanso[i]);
//		repartidor->instruccion = BUSCAR_PEDIDO;
		sem_init(&repartidor->sem_moverse, 0, 0);
		repartidor->objetivo_posX = -1;
		repartidor->objetivo_posY = -1;
		repartidor->quantum = -1;
		disponibilizar_repartidor(repartidor);
		list_add(repartidores, repartidor);

		pthread_create(&thread_repartidor, NULL, (void*) correr_repartidor, repartidor);

		liberar_lista(coordenada);
		i++;

//		imprimirRepartidor(repartidor);

	}

}

//void imprimirRepartidor(t_repartidor* repartidor) {
//
//	log_info(logger, "Repartidor N° %d | PosX: %d | PosY: %d | Descanso: %d | Duración: %d", repartidor->id, repartidor->posX, repartidor->posY, repartidor->frecuenciaDescanso, repartidor->tiempoDescanso);
//
//}

//void imprimirPCB(t_pcb* pcb) {
//
//	log_info(logger, "PCB | Pedido: %d | Repartidor: %d | Instrucción: %d |PosX: %d | PosY: %d", pcb->id_pedido, pcb->id_repartidor, pcb->instruccion, pcb->posX, pcb->posY);
//}

void crear_hilo_PLP() {

	int hilo_plp = pthread_create(&thread_PLP, NULL, (void*)planificador_largo_plazo, NULL);

		if (hilo_plp == -1)
			log_error(logger, "No se pudo generar el hilo PLP");
}

void crear_hilo_PCP(){

	if(string_equals_ignore_case("FIFO", app_conf.algoritmo_planificacion)) {

		int hilo_pcp = pthread_create(&thread_PCP, NULL, (void*)planificador_fifo, NULL);

		if (hilo_pcp == -1)
			log_error(logger, "No se pudo generar el hilo PCP");
	}

	if(string_equals_ignore_case("HRRN", app_conf.algoritmo_planificacion)) {

		int hilo_pcp = pthread_create(&thread_PCP, NULL, (void*)planificador_hrrn, NULL);

		if (hilo_pcp == -1)
			log_error(logger, "No se pudo generar el hilo PCP");
	}

	if(string_equals_ignore_case("SJF", app_conf.algoritmo_planificacion)) {

		int hilo_pcp = pthread_create(&thread_PCP, NULL, (void*)planificador_sjf, NULL);

		if (hilo_pcp == -1)
			log_error(logger, "No se pudo generar el hilo PCP");
	}

}
