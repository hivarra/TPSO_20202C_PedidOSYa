/*
 * planificador.c
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */

#include "Planificador.h"

/**PLANIFICACION**/
uint32_t generar_id_pcb(){
	uint32_t id_pcb;
	pthread_mutex_lock(&mutex_id_pcb);
	id_pcb = id_pcb_global++;
	pthread_mutex_unlock(&mutex_id_pcb);
	return id_pcb;
}
t_pcb* generar_pcb(uint32_t id_pedido,t_rta_obtener_receta* rta_obtener_receta){
	t_pcb* pcb = malloc(sizeof(t_pcb));
	pcb->id = generar_id_pcb();
	pcb->id_pedido = id_pedido;
	pcb->lista_pasos = rta_obtener_receta->pasos;
	strcpy(pcb->nombre_plato,rta_obtener_receta->nombre);
	pcb->quantum = 0;
	pcb->cocinero_asignado = -1;

	return pcb;
}
void agregar_pcb_en_cola_ready_con_afinidad(t_pcb* pcb,int id_afinidad){
	pthread_mutex_lock(&mutex_colas_ready[id_afinidad]);
	list_add(lista_colas_ready[id_afinidad],pcb);
	pthread_mutex_unlock(&mutex_colas_ready[id_afinidad]);
}
int obtener_id_afinidad(char nombre_plato[L_PLATO]){
	bool igual_nombre_plato(t_afinidad* afinidad){
		return string_equals_ignore_case(afinidad->nombre_afinidad,nombre_plato);
	}
	t_afinidad* afinidad_buscada = list_find(AFINIDADES_MAESTRO,(void*)igual_nombre_plato);

	return afinidad_buscada->id_afinidad;
}
void pasar_pcb_a_ready(t_pcb* pcb){
	int id_afinidad = obtener_id_afinidad(pcb->nombre_plato);
	log_info(logger,"Se procede a agregar pcb en cola READY con AFINIDAD:%s",pcb->nombre_plato);
	agregar_pcb_en_cola_ready_con_afinidad(pcb,id_afinidad);
}
void pasar_pcb_a_estado(t_pcb* pcb, t_estado_pcb estado){
	switch(estado){
		case READY:;
			pasar_pcb_a_ready(pcb);
		break;
		case EXEC:;
			/*TODO:*/
		break;
		case BLOCKED_POR_REPOSAR:;
			/*TODO:*/
		break;
		case BLOCKED_POR_HORNO:;
			/*TODO:*/
		break;
		case EXIT:;
			/*TODO:*/
		break;
		default:;
			log_info(logger, "[ERROR pasar_pcb_a_estado] Estado inexistente");
		break;
	}
}
t_pcb* obtener_proximo_pcb_a_ejecutar_por_FIFO(int id_cola_ready){
	pthread_mutex_lock(&mutex_colas_ready[id_cola_ready]);
	t_pcb* pcb = list_remove(lista_colas_ready[id_cola_ready], 0);
	pthread_mutex_unlock(&mutex_colas_ready[id_cola_ready]);

	return pcb;
}
void ejecutar_pcb(t_pcb* pcb){
	pasar_pcb_a_estado(pcb, EXEC);
	bool desalojo=false;
	/*TODO:CREAR FUNCIONES*/
//	while(!cocinero_termino_de_ejecutar(pcb)){
//		if(evaluar_desalojo(pcb)){
//			desalojo=true;
//			desalojar_pcb(pcb);
//			break;
//		}
//		log_info(logger,"CPU asignada a PCB con ID:%d, NOMBRE_PLATO:%s",pcb->id,pcb->nombre_plato);
//		sem_post(&sem_realizar_paso[pcb->cocinero_asignado]);
//
//		sem_wait(&finCPUbound);
//		log_info(logger,"Fin CPU Bound");
//	}
}
void planificar_platos(int* id_cola_ready){
	while(1){
		sem_wait(&sem_planificar_platos);
		t_pcb* pcb = obtener_proximo_pcb_a_ejecutar(*id_cola_ready);

		log_info(logger,"Se procede a ejecutar el PLATO:%s",pcb->nombre_plato);
//		ejecutar_pcb(pcb);
		log_info(logger,"Se finaliza ejecución del PLATO:%s",pcb->nombre_plato);
	}
}
/**INICIALIZACIONES**/
bool evaluar_desalojo_por_RR(t_pcb* pcb,int id_cola_ready){
	return pcb->quantum > QUANTUM && !list_is_empty(lista_colas_ready[id_cola_ready]);
}
void setear_algoritmo_FIFO(){
	algoritmo_planificacion = FIFO;
	obtener_proximo_pcb_a_ejecutar = obtener_proximo_pcb_a_ejecutar_por_FIFO;
	evaluar_desalojo = false;
}
void setear_algoritmo_RR(){
	algoritmo_planificacion = RR;
	obtener_proximo_pcb_a_ejecutar = obtener_proximo_pcb_a_ejecutar_por_FIFO;
	evaluar_desalojo = evaluar_desalojo_por_RR;
}
void inicializar_algoritmo(){
	if (strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "FIFO") == 0)
			setear_algoritmo_FIFO();
		else
			setear_algoritmo_RR();

	RETARDO_CICLO_CPU = restaurante_conf.retardo_ciclo_cpu;
}
void inicializar_colas_ready(){
	t_list* afinidades_distinct = list_duplicate(metadata_restaurante->afinidades_cocineros);
	//SE CREA LISTA CON AFINIDADES NO REPETIDAS
	for(int i=0;i<list_size(afinidades_distinct);i++){
		char* afinidad = list_get(afinidades_distinct,i);

		bool es_afinidad(char* una_afinidad){
			return string_equals_ignore_case(afinidad,una_afinidad);
		}
		int cantidad_afinidad = list_count_satisfying(afinidades_distinct,(void*)es_afinidad);

		while(cantidad_afinidad > 1){
			list_remove_by_condition(afinidades_distinct,(void*)es_afinidad);
			cantidad_afinidad = list_count_satisfying(afinidades_distinct,(void*)es_afinidad);
		}
	}
	mutex_colas_ready = malloc(sizeof(pthread_mutex_t)*list_size(afinidades_distinct));
	//SE CREA LISTA CON LAS AFINIDADES
	AFINIDADES_MAESTRO = list_create();
	pthread_mutex_init(&mutex_afinidades_maestro,NULL);

	for(int i=0;i<=list_size(afinidades_distinct);i++){
		//SE GUARDAN LAS DISTINTAS AFINIDADES
		t_afinidad* afinidad = malloc(sizeof(t_afinidad));
		afinidad->id_afinidad = i;
		strcpy(afinidad->nombre_afinidad,list_get(afinidades_distinct,i));
		list_add(AFINIDADES_MAESTRO,afinidad);
		//SE CREA COLA Y MUTEX DE LA AFINIDAD
		lista_colas_ready[i] = list_create();
		pthread_mutex_init(&mutex_colas_ready[i],NULL);
		//SE CREA HILO PLANIFICADOR DE LA AFINIDAD
		pthread_t hilo_planificador;
		pthread_create(&hilo_planificador,NULL,(void*)planificar_platos,&i);
		pthread_detach(hilo_planificador);
	}
}
void inicializar_colas_hornos(){
	mutex_colas_hornos = malloc(sizeof(pthread_mutex_t)*metadata_restaurante->cantidad_hornos);

	for(int i=0;i<=metadata_restaurante->cantidad_hornos;i++){
		lista_colas_hornos[i] = list_create();
		pthread_mutex_init(&mutex_colas_hornos[i],NULL);
	}
}
void inicializar_sem_mutex(){
	pthread_mutex_init(&mutex_id_pcb,NULL);
}
void inicializar_sem_contadores(){
	sem_init(&sem_planificar_platos,0,1);
	sem_realizar_paso =malloc(sizeof(sem_t)*list_size(metadata_restaurante->afinidades_cocineros));
}
//void inicializar_hilo_planificador(){
//	int contar_sin_afinidad(char* afinidad){
//		return string_equals_ignore_case(afinidad,"N");
//	}
//	int cant_sin_afinidad = list_count_satisfying(metadata_restaurante->afinidades_cocineros,(void*)contar_sin_afinidad);
//	int cantidad_afinidades = list_size(metadata_restaurante->afinidades_cocineros)-cant_sin_afinidad+1;
//
//	for(int i=0;i<=cantidad_afinidades;i++){
//		pthread_t hilo_planificador;
//		pthread_create(&hilo_planificador,NULL,(void*)planificar_platos,i);
//		pthread_detach(hilo_planificador);
//	}
//}
void inicializar_hilos_cocineros(){
	for(int i=0;i<=list_size(metadata_restaurante->afinidades_cocineros);i++){
		pthread_t thread_cocinero;
		pthread_create(&thread_cocinero,NULL,(void*)hilo_cocinero,NULL);
		pthread_detach(thread_cocinero);
	}
}
void inicializar_hilos(){
//	inicializar_hilo_planificador();
	inicializar_hilos_cocineros();
}
void inicializar_planificador(){
	inicializar_algoritmo();
	inicializar_colas_ready();
	inicializar_colas_hornos();
	inicializar_sem_contadores();
	inicializar_sem_mutex();
	inicializar_hilos();
}
void inicializar_planificacion(uint32_t id_pedido,t_rta_obtener_receta* rta_obtener_receta){
	t_pcb* pcb = generar_pcb(id_pedido,rta_obtener_receta);
	pasar_pcb_a_estado(pcb,READY);
	log_info(logger,"Creación de PLATO:%s con ID_PCB:%d",pcb->nombre_plato,pcb->id);
}
