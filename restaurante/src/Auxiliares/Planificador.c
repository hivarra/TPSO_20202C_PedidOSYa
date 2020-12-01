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
	t_pcb* pcb = calloc(1,sizeof(t_pcb));
	pcb->id = generar_id_pcb();
	pcb->id_pedido = id_pedido;
	pcb->lista_pasos = rta_obtener_receta->pasos;
	strcpy(pcb->nombre_plato,rta_obtener_receta->nombre);
	pcb->quantum = 0;
	pcb->cocinero_asignado = -1;//TODO: Cambiar tipo de dato

	return pcb;
}
void agregar_pcb_en_cola_ready_con_afinidad(t_pcb* pcb,int id_afinidad){
	pthread_mutex_lock(&mutex_colas_ready[id_afinidad]);
	list_add(lista_colas_ready[id_afinidad],pcb);
	pthread_mutex_unlock(&mutex_colas_ready[id_afinidad]);
}
int obtener_id_afinidad(char* nombre_plato){
	bool igual_nombre_plato(t_afinidad* afinidad){
		return string_equals_ignore_case(afinidad->nombre_afinidad,nombre_plato);
	}
	t_afinidad* afinidad_buscada = list_find(AFINIDADES_MAESTRO,(void*)igual_nombre_plato);

	return afinidad_buscada->id_afinidad;
}
void pasar_pcb_a_ready(t_pcb* pcb){
	int id_afinidad = obtener_id_afinidad(pcb->nombre_plato);
	log_info(logger,"Se agrega pcb en cola READY con AFINIDAD:%s",pcb->nombre_plato);
	agregar_pcb_en_cola_ready_con_afinidad(pcb,id_afinidad);
}
void pasar_pcb_a_blocked_por_reposar(t_pcb* pcb){
	log_info(logger,"Se agrega pcb con plato:%s en cola E/S REPOSAR",pcb->nombre_plato);
	pthread_mutex_lock(&mutex_platos[pcb->id]);
	pcb->estado = BLOCKED_POR_REPOSAR;
	t_paso_receta* paso = obtener_siguiente_paso(pcb);
	int tiempo = paso->tiempo;

	while(tiempo!=0){
		tiempo--;
		aplicar_retardo(tiempo);
	}
	pasar_pcb_a_ready(pcb);
	pthread_mutex_unlock(&mutex_platos[pcb->id]);
}
//void agregar_a_cola_bloqueados_horno(t_pcb* pcb){
//	pthread_mutex_lock(&mutex_cola_bloqueados_prehorno);
//	list_add(cola_bloqueados_prehorno,pcb);
//	pthread_mutex_unlock(&mutex_cola_bloqueados_prehorno);
//}
//bool no_hay_hornos_disponibles(){
//	pthread_mutex_lock(&mutex_cola_bloqueados_prehorno);
//	bool no_hay_hornos_disponibles = list_size(cola_bloqueados_prehorno) >= metadata_restaurante.cantidad_hornos;
//	pthread_mutex_unlock(&mutex_cola_bloqueados_prehorno);
//
//	return no_hay_hornos_disponibles;
//}
void pasar_pcb_a_blocked_por_horno(t_pcb* pcb){
	log_info(logger,"Se agrega pcb con plato:%s en cola E/S HORNO",pcb->nombre_plato);
	pthread_mutex_lock(&mutex_platos[pcb->id]);
	pcb->estado = BLOCKED_POR_HORNO;

	sem_wait(&sem_hornos);

	t_paso_receta* paso = obtener_siguiente_paso(pcb);
	int tiempo = paso->tiempo;

	while(tiempo!=0){
		tiempo--;
		aplicar_retardo(tiempo);
	}
	sem_post(&sem_hornos);
	pasar_pcb_a_ready(pcb);
	pthread_mutex_unlock(&mutex_platos[pcb->id]);
}
void pasar_pcb_a_exit(t_pcb* pcb){
	pthread_mutex_lock(&mutex_platos[pcb->id]);
	pcb->estado = EXIT;
	pthread_mutex_unlock(&mutex_platos[pcb->id]);
}
void pasar_pcb_a_exec(t_pcb* pcb){
	pthread_mutex_lock(&mutex_platos[pcb->id]);
	pcb->estado = EXEC;
	pthread_mutex_unlock(&mutex_platos[pcb->id]);
}
void pasar_pcb_a_estado(t_pcb* pcb, t_estado_pcb estado){
	switch(estado){
		case READY:;
			pasar_pcb_a_ready(pcb);
		break;
		case EXEC:;
			pasar_pcb_a_exec(pcb);
		break;
		case BLOCKED_POR_REPOSAR:;
			pasar_pcb_a_blocked_por_reposar(pcb);
		break;
		case BLOCKED_POR_HORNO:;
			pasar_pcb_a_blocked_por_horno(pcb);
		break;
		case EXIT:;
			pasar_pcb_a_exit(pcb);
		break;
		default:;
			log_warning(logger, "[ERROR pasar_pcb_a_estado] Estado inexistente");
		break;
	}
}
t_pcb* obtener_proximo_pcb_a_ejecutar_por_FIFO(int id_cola_ready){
	pthread_mutex_lock(&mutex_colas_ready[id_cola_ready]);
	t_pcb* pcb = list_remove(lista_colas_ready[id_cola_ready], 0);
	pthread_mutex_unlock(&mutex_colas_ready[id_cola_ready]);

	return pcb;
}
void desalojar_pcb(t_pcb* pcb){
	pasar_pcb_a_estado(pcb,READY);
}
void ejecutar_pcb(t_pcb* pcb, int id_cola_ready){
	pasar_pcb_a_estado(pcb, EXEC);

	while(cocinero_esta_ejecutando(pcb)){
		if(evaluar_desalojo(pcb,id_cola_ready)){
			desalojar_pcb(pcb);
			break;
		}
		log_info(logger,"CPU asignada a PCB con ID:%d, NOMBRE_PLATO:%s",pcb->id,pcb->nombre_plato);
		sem_post(&sem_realizar_paso[pcb->cocinero_asignado]);

		sem_wait(&finCPUbound);
		log_info(logger,"Fin CPU Bound");
	}
}
void finalizar_pcb(t_pcb* pcb){
	pasar_pcb_a_estado(pcb,EXIT);
}
void planificar_platos(int* id_cola_ready){
	while(1){
		sem_wait(&sem_planificar_platos);
		t_pcb* pcb = obtener_proximo_pcb_a_ejecutar(*id_cola_ready);

		log_info(logger,"Se procede a ejecutar el PLATO:%s",pcb->nombre_plato);
		ejecutar_pcb(pcb,*id_cola_ready);
		log_info(logger,"Se finaliza ejecución del PLATO:%s",pcb->nombre_plato);

//		if(plato_sin_pasos_para_ejecutar(pcb))
//			finalizar_pcb(pcb);
	}
}
/**INICIALIZACIONES**/
bool evaluar_desalojo_por_RR(t_pcb* pcb,int id_cola_ready){
	return pcb->quantum > QUANTUM && !list_is_empty(lista_colas_ready[id_cola_ready]);
}
void setear_algoritmo_FIFO(){
	log_info(logger,"Se setea algoritmo FIFO");
	algoritmo_planificacion = FIFO;
	obtener_proximo_pcb_a_ejecutar = obtener_proximo_pcb_a_ejecutar_por_FIFO;
	evaluar_desalojo = false;
}
void setear_algoritmo_RR(){
	log_info(logger,"Se setea algoritmo RR");
	algoritmo_planificacion = RR;
	obtener_proximo_pcb_a_ejecutar = obtener_proximo_pcb_a_ejecutar_por_FIFO;
	evaluar_desalojo = evaluar_desalojo_por_RR;
}
void inicializar_algoritmo(){
	log_info(logger,"[INICIALIZAR_ALGORITMO]");
	if (strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "FIFO") == 0)
			setear_algoritmo_FIFO();
		else
			setear_algoritmo_RR();

	RETARDO_CICLO_CPU = restaurante_conf.retardo_ciclo_cpu;
}
void inicializar_colas_ready(){
	log_info(logger,"[INICIALIZAR_COLAS_READY]");
	t_list* afinidades_distinct = list_duplicate(metadata_restaurante.afinidades_cocineros);
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

	lista_colas_ready =  malloc(sizeof(t_list)*list_size(metadata_restaurante.afinidades_cocineros));

	for(int i=0;i<list_size(afinidades_distinct);i++){
		//SE GUARDAN LAS DISTINTAS AFINIDADES
		t_afinidad* afinidad = malloc(sizeof(t_afinidad));
		afinidad->id_afinidad = i;
		strcpy(afinidad->nombre_afinidad,list_get(afinidades_distinct,i));
		list_add(AFINIDADES_MAESTRO,afinidad);
		log_info(logger,"Se agrega afinidad:%s con id_afinidad:%d a AFINIDADES_MAESTRO",afinidad->nombre_afinidad,afinidad->id_afinidad);
		//SE CREA COLA READY POR AFINIDAD
		lista_colas_ready[i] = list_create();
		log_info(logger,"Se crea cola ready para afinidad:%s con id:%d",afinidad->nombre_afinidad,i);
		//SE CREA MUTEX PARA CADA COLA READY
		pthread_mutex_init(&mutex_colas_ready[i],NULL);
		//SE CREA HILO PLANIFICADOR PARA CADA COLA READY
		pthread_t hilo_planificador;
		pthread_create(&hilo_planificador,NULL,(void*)planificar_platos,&i);
		pthread_detach(hilo_planificador);
		log_info(logger,"Se crea hilo planificador para cola ready con id:%d",i);

		free(afinidad);
	}
	free(afinidades_distinct);
}
//void inicializar_colas_hornos(){
//	mutex_colas_hornos = malloc(sizeof(pthread_mutex_t)*metadata_restaurante.cantidad_hornos);
//
//	for(int i=0;i<=metadata_restaurante.cantidad_hornos;i++){
//		pthread_mutex_init(&mutex_colas_hornos[i],NULL);
//	}
//}
void inicializar_sem_mutex(){
	log_info(logger,"[INICIALIZAR_SEM_MUTEX]");
	pthread_mutex_init(&mutex_id_pcb,NULL);
	pthread_mutex_init(&mutex_hornos,NULL);
//	pthread_mutex_init(&mutex_cola_bloqueados_prehorno,NULL);
}
void inicializar_sem_contadores(){
	log_info(logger,"[INICIALIZAR_SEM_CONTADORES]");
	sem_init(&sem_planificar_platos,0,1);
	sem_init(&sem_hornos,0,metadata_restaurante.cantidad_hornos);
	sem_realizar_paso =malloc(sizeof(sem_t)*list_size(metadata_restaurante.afinidades_cocineros));
}
void inicializar_hilos_cocineros(){
	log_info(logger,"[INICIALIZAR_HILOS_COCINEROS]");

	for(int i=0;i<=list_size(metadata_restaurante.afinidades_cocineros);i++){
		t_cocinero* cocinero = malloc(sizeof(t_cocinero));
		cocinero->afinidad = list_get(metadata_restaurante.afinidades_cocineros,i);
		cocinero->id = i;

		pthread_t thread_cocinero;
		pthread_create(&thread_cocinero,NULL,(void*)hilo_cocinero,cocinero);
		pthread_detach(thread_cocinero);
		log_info(logger,"Se crea hilo_cocinero con afinidad:%s y id:%d",cocinero->afinidad,cocinero->id);
	}
}
void inicializar_planificador(){
	log_info(logger,"[INICIALIZAR_PLANIFICADOR]");

	inicializar_algoritmo();
	inicializar_colas_ready();
//	inicializar_colas_hornos();
	inicializar_sem_contadores();
	inicializar_sem_mutex();
	inicializar_hilos_cocineros();
}
void inicializar_planificacion(uint32_t id_pedido,t_rta_obtener_receta* rta_obtener_receta){
	t_pcb* pcb = generar_pcb(id_pedido,rta_obtener_receta);
	pasar_pcb_a_estado(pcb,READY);
	log_info(logger,"Creación de PLATO:%s con ID_PCB:%d",pcb->nombre_plato,pcb->id);
}
void aplicar_retardo(int tiempo_a_consumir){
	int retardo = RETARDO_CICLO_CPU*tiempo_a_consumir;
	sleep(retardo);
}
