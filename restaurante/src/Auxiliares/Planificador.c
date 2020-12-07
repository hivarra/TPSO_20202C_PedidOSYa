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
char* estado_pcb_enum_a_string(t_estado_pcb estado_enum){
	if(estado_enum == READY)
		return "READY";
	else if (estado_enum == NEW)
		return "NEW";
	else if (estado_enum == BLOCKED_POR_HORNO)
		return "BLOCKED_POR_HORNO";
	else if (estado_enum == BLOCKED_POR_REPOSAR)
		return "BLOCKED_POR_REPOSAR";
	else if (estado_enum == EXEC)
		return "EXEC";
	else
		return "EXIT";
}
t_pcb* generar_pcb(uint32_t id_pedido,t_rta_obtener_receta* rta_obtener_receta){

	void loggear_pasos(t_paso_receta* paso_receta){
		log_info(logger,"[CREACION PLATO] PASO:%s",paso_receta->accion);
		log_info(logger,"[CREACION PLATO] TIEMPO:%d",paso_receta->tiempo);
	}

	t_pcb* pcb = calloc(1,sizeof(t_pcb));
	pcb->id = generar_id_pcb();
	pcb->id_pedido = id_pedido;
	pcb->lista_pasos = rta_obtener_receta->pasos;
	strcpy(pcb->nombre_plato,rta_obtener_receta->nombre);
	pcb->estado = NEW;
	pcb->quantum = 0;
	pcb->cocinero_asignado = -1;//TODO: Cambiar tipo de dato
	pthread_mutex_init(&pcb->mutex_pcb,NULL);

	log_info(logger,"[CREACION PLATO] SE CREA PCB CON:");
	log_info(logger,"[CREACION PLATO] ID_PCB:%d",pcb->id);
	log_info(logger,"[CREACION PLATO] ID_PEDIDO:%d",pcb->id_pedido);
	log_info(logger,"[CREACION PLATO] NOMBRE_PLATO:%s",pcb->nombre_plato);
	log_info(logger,"[CREACION PLATO] ESTADO:%s",estado_pcb_enum_a_string(pcb->estado));
	log_info(logger,"[CREACION PLATO] QUANTUM:%d",pcb->quantum);
	log_info(logger,"[CREACION PLATO] LISTA_PASOS:");

	list_iterate(pcb->lista_pasos,(void*)loggear_pasos);

	return pcb;
}
void agregar_pcb_en_cola_ready_con_afinidad(t_pcb* pcb,int id_afinidad){
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = READY;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	pthread_mutex_lock(&mutex_colas_ready[id_afinidad]);
	list_add(lista_colas_ready[id_afinidad],pcb);
	pthread_mutex_unlock(&mutex_colas_ready[id_afinidad]);
}
t_afinidad* obtener_id_afinidad(char nombre_plato[L_PLATO]){
//	log_info(logger,"[OBTENER_ID_AFINIDAD]");
	bool igual_nombre_plato(t_afinidad* afinidad){
		return string_equals_ignore_case(afinidad->nombre_afinidad,nombre_plato);
	}
	t_afinidad* afinidad_buscada = list_find(AFINIDADES_MAESTRO,(void*)igual_nombre_plato);

	//Si no tiene afinidad, se debe agregar a cola ready sin afinidad ("N")
	bool es_sin_afinidad(t_afinidad* afinidad){
		return string_equals_ignore_case(afinidad->nombre_afinidad,"N");
	}
	if(afinidad_buscada == NULL)
		afinidad_buscada = list_find(AFINIDADES_MAESTRO,(void*)es_sin_afinidad);

	return afinidad_buscada;
}
void pasar_pcb_a_ready(t_pcb* pcb){
//	log_info(logger,"[PASAR_PCB_A_READY]");
	t_afinidad* afinidad = obtener_id_afinidad(pcb->nombre_plato);
	agregar_pcb_en_cola_ready_con_afinidad(pcb,afinidad->id_afinidad);
	log_info(logger,"Se agrega pcb en cola READY con AFINIDAD:%s",afinidad->nombre_afinidad);
}
void pasar_pcb_a_blocked_por_reposar(t_pcb* pcb){
	log_info(logger,"Se agrega pcb con plato:%s en cola E/S REPOSAR",pcb->nombre_plato);
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = BLOCKED_POR_REPOSAR;
	t_paso_receta* paso = obtener_siguiente_paso(pcb);

	aplicar_retardo(paso->tiempo);
	pasar_pcb_a_ready(pcb);
	pthread_mutex_unlock(&pcb->mutex_pcb);
}
void agregar_a_cola_bloqueados_horno(t_pcb* pcb){
	pthread_mutex_lock(&mutex_cola_bloqueados_prehorno);
	list_add(cola_bloqueados_prehorno,pcb);
	pthread_mutex_unlock(&mutex_cola_bloqueados_prehorno);
}
bool hay_hornos_disponibles(){
	pthread_mutex_lock(&mutex_cola_bloqueados_prehorno);
	bool no_hay_hornos_disponibles = list_size(cola_bloqueados_prehorno) <= metadata_restaurante.cantidad_hornos;
	pthread_mutex_unlock(&mutex_cola_bloqueados_prehorno);

	return no_hay_hornos_disponibles;
}
t_pcb* obtener_proximo_pcb_a_hornear(){
	pthread_mutex_lock(&mutex_cola_bloqueados_prehorno);
	t_pcb* pcb = list_remove(cola_bloqueados_prehorno,0);
	pthread_mutex_unlock(&mutex_cola_bloqueados_prehorno);

	return pcb;
}
void enviar_pcb_a_horno(t_pcb* pcb){
	log_info(logger,"Se agrega pcb con plato:%s en cola E/S HORNO",pcb->nombre_plato);
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = BLOCKED_POR_HORNO;
	t_paso_receta* paso = obtener_siguiente_paso(pcb);

	aplicar_retardo(paso->tiempo);
	eliminar_paso_realizado(paso);
	pthread_mutex_unlock(&pcb->mutex_pcb);
}
void sacar_pcb_de_horno(t_pcb* pcb){
	bool es_pcb(t_pcb* pcb_aux){
		return pcb_aux->id == pcb->id;
	}
	pthread_mutex_lock(&mutex_cola_bloqueados_prehorno);
	list_remove_by_condition(cola_bloqueados_prehorno,(void*)es_pcb);
	pthread_mutex_unlock(&mutex_cola_bloqueados_prehorno);

	pasar_pcb_a_ready(pcb);
}
void planificar_hornos(){

	while(1){
		sem_wait(&sem_hornear_plato);
		sem_wait(&sem_hay_espacio_en_horno);
		t_pcb* pcb = obtener_proximo_pcb_a_hornear();

		log_info(logger,"[PLANIFICAR_HORNOS] Se envia PLATO:%s de ID_PEDIDO:%d al horno",pcb->nombre_plato,pcb->id_pedido);
		enviar_pcb_a_horno(pcb);
		sacar_pcb_de_horno(pcb);
		log_info(logger,"[PLANIFICAR_HORNOS] PLATO%s de ID_PEDIDO:%d sale del horno",pcb->nombre_plato,pcb->id_pedido);
		sem_post(&sem_hay_espacio_en_horno);
	}
}
void pasar_pcb_a_blocked_por_horno(t_pcb* pcb){
	log_info(logger,"Se agrega pcb con plato:%s en cola E/S HORNO",pcb->nombre_plato);
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = BLOCKED_POR_HORNO;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	agregar_a_cola_bloqueados_horno(pcb);

	sem_post(&sem_hornear_plato);
}
void agregar_pcb_a_cola_exit(t_pcb* pcb){
	pthread_mutex_lock(&mutex_cola_exit);
	list_add(cola_exit,pcb);
	pthread_mutex_unlock(&mutex_cola_exit);
}
void pasar_pcb_a_exit(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = EXIT;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	agregar_pcb_a_cola_exit(pcb);
}
void pasar_pcb_a_exec(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = EXEC;
	pthread_mutex_unlock(&pcb->mutex_pcb);
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
	log_info(logger,"[obtener_proximo_pcb_a_ejecutar_por_FIFO]");
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
		sem_post(&sem_realizar_paso[pcb->cocinero_asignado]);
		log_info(logger,"[EJECUTAR_PCB] SEM_POST de cocinero con ID:%d para que ejecute PLATO:%s del pedido con ID_PEDIDO:%d",pcb->cocinero_asignado,pcb->nombre_plato,pcb->id_pedido);

		sem_wait(&finCPUbound);
		log_info(logger,"Fin CPU Bound");
	}
}
void inicializar_ciclo_planificacion(t_list* comidas){
	//SE REALIZA SEM_POST DE SEMAFOROS DE COLAS READY
	bool NO_EXISTEN_COCINEROS_CON_AFINIDAD = false;

	for(int i=0;i<list_size(comidas);i++){
		t_comida* comida = list_get(comidas,i);

		bool es_afinidad(t_afinidad* afinidad){
			return string_equals_ignore_case(afinidad->nombre_afinidad,comida->nombre);
		}
		t_afinidad* afinidad = list_find(AFINIDADES_MAESTRO,(void*)es_afinidad);
		if(afinidad != NULL)
			sem_post(&sem_cola_ready[afinidad->id_afinidad]);
		else
			NO_EXISTEN_COCINEROS_CON_AFINIDAD = true;
	}
	if(NO_EXISTEN_COCINEROS_CON_AFINIDAD){
		bool es_sin_afinidad(t_afinidad* afinidad){
			return string_equals_ignore_case(afinidad->nombre_afinidad,"N");
		}
		t_afinidad* sin_afinidad = list_find(AFINIDADES_MAESTRO,(void*)es_sin_afinidad);
		sem_post(&sem_cola_ready[sin_afinidad->id_afinidad]);
	}

}
void asignar_cocinero_a_pcb(t_pcb* pcb){
	//SE ASIGNA COCINERO CON AFINIDAD PARA LUEGO EJECUTAR PLATO
	bool cocinero_con_afinidad(t_cocinero* cocinero){
		return string_equals_ignore_case(pcb->nombre_plato,cocinero->afinidad);
	}
	t_cocinero* cocinero_a_asignar = list_find(lista_cocineros,(void*)cocinero_con_afinidad);
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->cocinero_asignado = cocinero_a_asignar->id;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	pthread_mutex_lock(&cocinero_a_asignar->mutex_cocinero);
	cocinero_a_asignar->pcb = pcb;
	pthread_mutex_unlock(&cocinero_a_asignar->mutex_cocinero);
}
bool pedido_esta_terminado(uint32_t id_pedido){
	bool pedido_terminado = false;

	bool es_pedido(t_pedido_pcb* pedido_pcb){
		return pedido_pcb->id_pedido == id_pedido;
	}
	t_list* lista_filtrada = list_filter(pedidos_pcbs,(void*)es_pedido);
	for(int i=0;i<list_size(lista_filtrada);i++){
		t_pedido_pcb* pedido_pcb = list_get(lista_filtrada,i);

		bool pcb_esta_en_exit(t_pcb* pcb){
			return pcb->id == pedido_pcb->id_pcb;
		}
		pedido_terminado = list_any_satisfy(cola_exit,(void*)pcb_esta_en_exit);
	}
	free(lista_filtrada);
	return pedido_terminado;
}
void liberar_pcbs_de_pedido(uint32_t id_pedido){
	bool es_pedido(t_pcb* pcb){
			return pcb->id_pedido == id_pedido;
	}
	pthread_mutex_lock(&mutex_cola_exit);
	t_list* lista_filtrada = list_filter(cola_exit,(void*)es_pedido);
	list_destroy_and_destroy_elements(lista_filtrada,free);
	pthread_mutex_unlock(&mutex_cola_exit);
}
void planificar_platos(int* id_cola_ready){
	log_info(logger,"[HILO PLANIFICAR_PLATOS] Planificador con ID:%d corriendo",*id_cola_ready);
	while(1){
		sem_wait(&sem_cola_ready[*id_cola_ready]);
		t_pcb* pcb = obtener_proximo_pcb_a_ejecutar(*id_cola_ready);

		asignar_cocinero_a_pcb(pcb);

		log_info(logger,"[HILO PLANIFICAR_PLATOS] Se procede a ejecutar PCB con ID_PEDIDO:%d,NOMBRE_PLATO:%s",pcb->id_pedido,pcb->nombre_plato);
		ejecutar_pcb(pcb,*id_cola_ready);
//		log_info(logger,"[HILO PLANIFICAR_PLATOS] Se finaliza ejecución del PLATO:%s",pcb->nombre_plato);

		if(plato_sin_pasos_para_ejecutar(pcb)){
			pasar_pcb_a_estado(pcb,EXIT);
			informar_plato_listo(pcb->id_pedido,pcb->nombre_plato);
			if(pedido_esta_terminado(pcb->id_pedido)){
				enviar_terminar_pedido_a_sindicato(pcb->id_pedido);
				liberar_pcbs_de_pedido(pcb->id_pedido);
			}
		}
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
//	log_info(logger,"[INICIALIZAR_ALGORITMO]");
	if (strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "FIFO") == 0)
			setear_algoritmo_FIFO();
		else
			setear_algoritmo_RR();

	RETARDO_CICLO_CPU = restaurante_conf.retardo_ciclo_cpu;
}
void inicializar_colas_ready(){
//	log_info(logger,"[INICIALIZAR_COLAS_READY]");
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
	}
	free(afinidades_distinct);
}
void inicializar_sem_mutex(){
//	log_info(logger,"[INICIALIZAR_SEM_MUTEX]");
	pthread_mutex_init(&mutex_id_pcb,NULL);
	pthread_mutex_init(&mutex_id_pedidos,NULL);
	pthread_mutex_init(&mutex_cola_bloqueados_prehorno,NULL);
	pthread_mutex_init(&mutex_cola_exit,NULL);
}
void inicializar_sem_contadores(){
//	log_info(logger,"[INICIALIZAR_SEM_CONTADORES]");
	sem_init(&sem_hornear_plato,0,0);
	sem_init(&sem_hay_espacio_en_horno,0,metadata_restaurante.cantidad_hornos);

	sem_realizar_paso = malloc(sizeof(sem_t)*list_size(metadata_restaurante.afinidades_cocineros));
	for(int i=0;i<list_size(metadata_restaurante.afinidades_cocineros);i++){
		sem_init(&sem_realizar_paso[i],0,0);
	}

	sem_cola_ready = malloc(sizeof(sem_t)*list_size(AFINIDADES_MAESTRO));
	for(int i=0;i<list_size(AFINIDADES_MAESTRO);i++){
			sem_init(&sem_cola_ready[i],0,0);
	}
}
void inicializar_hilos_cocineros(){
	log_info(logger,"[INICIALIZAR_HILOS_COCINEROS]");
	lista_cocineros = list_create();

	for(int i=0;i<list_size(metadata_restaurante.afinidades_cocineros);i++){
		t_cocinero* cocinero = malloc(sizeof(t_cocinero));
		strcpy(cocinero->afinidad,list_get(metadata_restaurante.afinidades_cocineros,i));
		cocinero->id = i;
		cocinero->pcb = NULL;
		pthread_mutex_init(&cocinero->mutex_cocinero,NULL);
		list_add(lista_cocineros,cocinero);

		pthread_t thread_cocinero;
		pthread_create(&thread_cocinero,NULL,(void*)hilo_cocinero,cocinero);
		pthread_detach(thread_cocinero);
		log_info(logger,"Se crea hilo_cocinero con afinidad:%s y id:%d",cocinero->afinidad,cocinero->id);
	}
}
void inicializar_hilos_planificadores(){
	log_info(logger,"[INICIALIZAR_CICLO_PLANIFICACION]");

	for(int i=0;i<list_size(AFINIDADES_MAESTRO);i++){
		//SE CREA HILO PLANIFICADOR PARA CADA COLA READY
		t_afinidad* afinidad = list_get(AFINIDADES_MAESTRO,i);
		pthread_t hilo_planificador;
		pthread_create(&hilo_planificador,NULL,(void*)planificar_platos,&afinidad->id_afinidad);
		pthread_detach(hilo_planificador);
		log_info(logger,"Se crea hilo planificador para cola ready con afinidad:%s con id:%d",afinidad->nombre_afinidad,afinidad->id_afinidad);
	}
}
void inicializar_cola_exit(){
	cola_exit = list_create();
}
void inicializar_cola_bloqueados_prehorno(){
	cola_bloqueados_prehorno = list_create();
}
void inicializar_planificador(){
//	log_info(logger,"[INICIALIZAR_PLANIFICADOR]");

	inicializar_algoritmo();
	inicializar_colas_ready();
	inicializar_cola_exit();
	inicializar_cola_bloqueados_prehorno();
	inicializar_sem_contadores();
	inicializar_sem_mutex();
	inicializar_hilos_planificadores();
	inicializar_hilos_cocineros();
}
void agregar_a_lista_pedidos_pcbs(uint32_t id_pedido, uint32_t id_pcb){
	t_pedido_pcb* pedido_pcb = malloc(sizeof(t_pedido_pcb));
	pedido_pcb->id_pcb = id_pcb;
	pedido_pcb->id_pedido = id_pedido;

	pthread_mutex_lock(&mutex_pedidos_pcbs);
	list_add(pedidos_pcbs,pedido_pcb);
	pthread_mutex_unlock(&mutex_pedidos_pcbs);
}
void crear_y_agregar_pcb_a_cola_ready(uint32_t id_pedido,t_rta_obtener_receta* rta_obtener_receta,uint32_t cantidad_total){
	int i=0;

	while(i<cantidad_total){
		t_pcb* pcb = generar_pcb(id_pedido,rta_obtener_receta);
		pasar_pcb_a_estado(pcb,READY);
		agregar_a_lista_pedidos_pcbs(pcb->id_pedido,pcb->id);
//		log_info(logger,"[CREAR_Y_AGREGAR_PCB_A_COLA_READY] Se crea PCB de PLATO:%s con ID_PCB:%d",pcb->nombre_plato,pcb->id);
		i++;
	}
}
void aplicar_retardo(int tiempo_a_consumir){
	int retardo = RETARDO_CICLO_CPU*tiempo_a_consumir;
	sleep(retardo);
}
