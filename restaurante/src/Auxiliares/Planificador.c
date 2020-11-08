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
void agregar_pcb_en_cola_ready_normal(t_pcb* pcb){
	t_list* cola_ready_normal = dictionary_get(dictionary_colas_ready,"NORMAL");
	pthread_mutex_lock(&mutex_cola_ready_normal);
	list_add(cola_ready_normal,pcb);
	pthread_mutex_unlock(&mutex_cola_ready_normal);
}
void pasar_pcb_a_ready(t_pcb* pcb){
	t_list* cola_ready_con_afinidad  = dictionary_get(dictionary_colas_ready,pcb->nombre_plato);
	if(cola_ready_con_afinidad == NULL){
		log_info(logger,"No existe cola READY con afinidad:%s",pcb->nombre_plato);
		log_info(logger,"Se procede a agregar pcb en cola READY NORMAL");
		agregar_pcb_en_cola_ready_normal(pcb);
	}
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
//t_pcb* obtener_proximo_pcb_a_ejecutar_por_FIFO(){
	/*TODO: OBTENER EL DE MENOR TIEMPO DE LLEGADA DEL DICTIONARY*/
	/*TODO: COMPARARLO CONTRA EL DE MENOR TIEMPO DE COLA READY NORMAL*/
	/*TODO: QUEDARME CON EL DE MENOR TIEMPO DE LLEGADA*/
//}
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
void planificar_platos(){
	while(1){
		sem_wait(&sem_planificar_platos);
//		t_pcb* pcb = obtener_proximo_pcb_a_ejecutar();

//		log_info(logger,"Se procede a ejecutar el PLATO:%s",pcb->nombre_plato);
//		ejecutar_pcb(pcb);
//		log_info(logger,"Se finaliza ejecución del PLATO:%s",pcb->nombre_plato);
	}
}
/**INICIALIZACIONES**/
void inicializar_algoritmo(){
	if (strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "FIFO") == 0)
			algoritmo_planificacion = FIFO;
		else
			algoritmo_planificacion = RR;

	RETARDO_CICLO_CPU = restaurante_conf.retardo_ciclo_cpu;
}
void inicializar_colas_ready(){
	dictionary_colas_ready = dictionary_create();

	for(int i=0;i<=list_size(metadata_restaurante->afinidades_cocineros);i++){
		char* afinidad = list_get(metadata_restaurante->afinidades_cocineros,i);
		if(string_equals_ignore_case(afinidad,"N")){
			t_list* cola_ready = list_create();
			dictionary_put(dictionary_colas_ready,afinidad,cola_ready);
		}
	}
	t_list* cola_ready_normal = list_create();
	dictionary_put(dictionary_colas_ready,"NORMAL",cola_ready_normal);
}
void inicializar_colas_hornos(){
	for(int i=0;i<=metadata_restaurante->cantidad_hornos;i++){
		t_list* cola_horno = list_create();
		list_add(lista_colas_hornos,cola_horno);
	}
}
void inicializar_sem_mutex(){
	pthread_mutex_init(&mutex_dictionary_ready,NULL);
	pthread_mutex_init(&mutex_cola_ready_normal,NULL);
	pthread_mutex_init(&mutex_colas_hornos,NULL);
	pthread_mutex_init(&mutex_lista_cocineros,NULL);
	pthread_mutex_init(&mutex_id_pcb,NULL);
}
void inicializar_sem_contadores(){
	sem_init(&sem_planificar_platos,0,1);
	sem_realizar_paso =malloc(sizeof(sem_t)*list_size(metadata_restaurante->afinidades_cocineros));
}
void inicializar_hilo_planificador(){
	pthread_t hilo_planificador;
	pthread_create(&hilo_planificador,NULL,(void*)planificar_platos,NULL);
	pthread_detach(hilo_planificador);
}
void inicializar_hilos_cocineros(){
	for(int i=0;i<=list_size(metadata_restaurante->afinidades_cocineros);i++){
		pthread_t thread_cocinero;
		pthread_create(&thread_cocinero,NULL,(void*)hilo_cocinero,NULL);
		pthread_detach(thread_cocinero);
	}
}
void inicializar_hilos(){
	inicializar_hilo_planificador();
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
