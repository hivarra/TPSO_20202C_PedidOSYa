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

	void loggear_pasos(t_paso_receta* paso_receta){
		log_info(logger,"[CREACION PLATO] PASO:%s",paso_receta->accion);
		log_info(logger,"[CREACION PLATO] TIEMPO:%d",paso_receta->tiempo);
	}

	t_pcb* pcb = calloc(1,sizeof(t_pcb));
	pcb->id = generar_id_pcb();
	pcb->id_pedido = id_pedido;
	pcb->lista_pasos = duplicar_lista_pasos(rta_obtener_receta->pasos);
	strcpy(pcb->nombre_plato,rta_obtener_receta->nombre);
	pcb->estado = NEW;
	pcb->cocinero_asignado = -1;//TODO: Cambiar tipo de dato
	pcb->quantum_consumido = 0;
	pthread_mutex_init(&pcb->mutex_pcb,NULL);
	pthread_mutex_init(&pcb->mutex_pasos,NULL);

	log_info(logger,"[CREACION PLATO] SE CREA PCB CON:");
	log_info(logger,"[CREACION PLATO] ID_PCB:%d",pcb->id);
	log_info(logger,"[CREACION PLATO] ID_PEDIDO:%d",pcb->id_pedido);
	log_info(logger,"[CREACION PLATO] NOMBRE_PLATO:%s",pcb->nombre_plato);
	log_info(logger,"[CREACION PLATO] ESTADO:%s",estado_pcb_enum_a_string(pcb->estado));
	log_info(logger,"[CREACION PLATO] LISTA_PASOS:");

	list_iterate(pcb->lista_pasos,(void*)loggear_pasos);

	return pcb;
}
void agregar_pcb_en_cola_ready_con_afinidad(t_pcb* pcb,int id_afinidad){
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = READY;
	pcb->quantum_consumido=0;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	pthread_mutex_lock(&mutex_colas_ready[id_afinidad]);
	list_add(lista_colas_ready[id_afinidad],pcb);
	pthread_mutex_unlock(&mutex_colas_ready[id_afinidad]);
}

void pasar_pcb_a_ready(t_pcb* pcb){
//	log_info(logger,"[PASAR_PCB_A_READY]");
	t_afinidad* afinidad = obtener_afinidad(pcb->nombre_plato);
	agregar_pcb_en_cola_ready_con_afinidad(pcb,afinidad->id_afinidad);
	log_info(logger,"[PLANIFICADOR]Se agrega PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d en cola READY con AFINIDAD:%s",pcb->nombre_plato,pcb->id,pcb->id_pedido,afinidad->nombre_afinidad);
}
void pasar_pcb_a_blocked_por_reposar(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = BLOCKED_POR_REPOSAR;
	pcb->quantum_consumido=0;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	t_paso_receta* paso = obtener_siguiente_paso(pcb);
	log_info(logger,"[REPOSAR] Se ejecuta %d rafagas de CPU de PASO:%s de PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d",paso->tiempo,paso->accion,pcb->nombre_plato,pcb->id,pcb->id_pedido);

	t_cocinero* cocinero=obtener_cocinero_por_id(pcb->cocinero_asignado);
	cocinero->disponible=true;

	t_afinidad* afinidad=obtener_afinidad(cocinero->afinidad);
	sem_post(&sem_fin_paso[afinidad->id_afinidad]);

	aplicar_retardo(paso->tiempo);
	pasar_pcb_a_ready(pcb);

	sem_post(&sem_cola_ready[afinidad->id_afinidad]);
}
void agregar_a_cola_bloqueados_horno(t_pcb* pcb){
	pthread_mutex_lock(&mutex_cola_bloqueados_prehorno);
	list_add(cola_bloqueados_prehorno,pcb);
	pthread_mutex_unlock(&mutex_cola_bloqueados_prehorno);
}
t_pcb* obtener_proximo_pcb_a_hornear(){
	pthread_mutex_lock(&mutex_cola_bloqueados_prehorno);
	t_pcb* pcb = list_remove(cola_bloqueados_prehorno,0);
	pthread_mutex_unlock(&mutex_cola_bloqueados_prehorno);

	return pcb;
}
void realizar_paso_horno(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pasos);
	t_paso_receta* paso = list_get(pcb->lista_pasos,0);
	pthread_mutex_unlock(&pcb->mutex_pasos);
	log_info(logger,"[HORNO] Se ejecuta %d rafagas de CPU de PASO:%s de PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d",paso->tiempo,paso->accion,pcb->nombre_plato,pcb->id,pcb->id_pedido);
	aplicar_retardo(paso->tiempo);
}
void ocupar_horno(int id_horno){
	bool es_horno(t_horno* horno){
		return horno->id_horno==id_horno;
	}
	pthread_mutex_lock(&mutex_lista_hornos);
	t_horno* horno=list_find(lista_hornos,(void*)es_horno);
	pthread_mutex_unlock(&mutex_lista_hornos);

	horno->disponible=false;
}
void liberar_horno(int id_horno){
	bool es_horno(t_horno* horno){
		return horno->id_horno==id_horno;
	}
	pthread_mutex_lock(&mutex_lista_hornos);
	t_horno* horno=list_find(lista_hornos,(void*)es_horno);
	pthread_mutex_unlock(&mutex_lista_hornos);

	horno->disponible=true;
}
void planificar_horno(int* id_horno){

	while(1){
		sem_wait(&sem_hornear_plato[*id_horno]);
		ocupar_horno(*id_horno);
		t_pcb* pcb = obtener_proximo_pcb_a_hornear();

		realizar_paso_horno(pcb);
		pthread_mutex_lock(&pcb->mutex_pasos);
		t_paso_receta*paso=list_get(pcb->lista_pasos,0);
		pthread_mutex_unlock(&pcb->mutex_pasos);

		eliminar_paso(paso,pcb);

		pasar_pcb_a_estado(pcb,READY);
		t_afinidad* afinidad = obtener_afinidad(pcb->nombre_plato);
		sem_post(&sem_cola_ready[afinidad->id_afinidad]);

		liberar_horno(*id_horno);
		sem_post(&sem_hay_espacio_en_horno);
	}
}
t_horno* obtener_horno_disponible(){
	bool horno_esta_disponible(t_horno* horno){
		return horno->disponible;
	}

	pthread_mutex_lock(&mutex_lista_hornos);
	t_horno* horno =list_find(lista_hornos,(void*)horno_esta_disponible);
	pthread_mutex_unlock(&mutex_lista_hornos);

	return horno;
}
void planificar_cola_hornos(){

	while(1){
		sem_wait(&sem_planificar_cola_hornos);
		sem_wait(&sem_hay_espacio_en_horno);
		t_horno* horno = obtener_horno_disponible();

		if(horno!=NULL)
			sem_post(&sem_hornear_plato[horno->id_horno]);
	}
}
void pasar_pcb_a_blocked_por_horno(t_pcb* pcb){
	log_info(logger,"[PLANIFICADOR] Se agrega pcb con PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d en cola E/S HORNO",pcb->nombre_plato,pcb->id,pcb->id_pedido);
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = BLOCKED_POR_HORNO;
	pcb->quantum_consumido=0;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	t_cocinero* cocinero=obtener_cocinero_por_id(pcb->cocinero_asignado);
	cocinero->disponible=true;

	agregar_a_cola_bloqueados_horno(pcb);

	t_afinidad* afinidad=obtener_afinidad(cocinero->afinidad);
	sem_post(&sem_fin_paso[afinidad->id_afinidad]);
	sem_post(&sem_planificar_cola_hornos);

}
void agregar_pcb_a_cola_exit(t_pcb* pcb){
	pthread_mutex_lock(&mutex_cola_exit);
	list_add(cola_exit,pcb);
	pthread_mutex_unlock(&mutex_cola_exit);
}
void pasar_pcb_a_exit(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->estado = EXIT;
	pcb->quantum_consumido=0;
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
	pthread_mutex_lock(&mutex_colas_ready[id_cola_ready]);
	t_pcb* pcb = list_remove(lista_colas_ready[id_cola_ready], 0);
	pthread_mutex_unlock(&mutex_colas_ready[id_cola_ready]);

	return pcb;
}
void desalojar_pcb(t_pcb* pcb){
	pasar_pcb_a_estado(pcb,READY);
}
bool pedido_esta_terminado(uint32_t id_pedido){
	bool pedido_terminado = false;

	bool es_pedido(t_pedido_pcb* pedido_pcb){
		return pedido_pcb->id_pedido == id_pedido;
	}
	bool es_pedido_2(t_pcb* pcb){
		return pcb->id_pedido == id_pedido;
	}

	pthread_mutex_lock(&mutex_pedidos_pcbs);
	t_list* lista_filtrada = list_filter(pedidos_pcbs,(void*)es_pedido);
	pthread_mutex_unlock(&mutex_pedidos_pcbs);

	pthread_mutex_lock(&mutex_cola_exit);
	t_list* lista_filtrada_exit = list_filter(cola_exit,(void*)es_pedido_2);
	pthread_mutex_unlock(&mutex_cola_exit);

	if(list_size(lista_filtrada) == list_size(lista_filtrada_exit))
		pedido_terminado = true;

	list_destroy(lista_filtrada);
	list_destroy(lista_filtrada_exit);

	return pedido_terminado;
}
void liberar_pcbs_de_pedido(uint32_t id_pedido){
	pthread_mutex_lock(&mutex_cola_exit);
	for(int i=0;i<list_size(cola_exit);i++){
		t_pcb* pcb = list_get(cola_exit,i);
		if(pcb->id_pedido == id_pedido){
			list_destroy(pcb->lista_pasos);
			list_remove_and_destroy_element(cola_exit,i,free);
		}
	}
	pthread_mutex_unlock(&mutex_cola_exit);

	pthread_mutex_lock(&mutex_pedidos_pcbs);
		for(int i=0;i<list_size(pedidos_pcbs);i++){
			t_pedido_pcb* tupla_pedido_pcb = list_get(pedidos_pcbs,i);
			if(tupla_pedido_pcb->id_pedido == id_pedido)
				list_remove_and_destroy_element(pedidos_pcbs,i,free);
		}
	pthread_mutex_unlock(&mutex_pedidos_pcbs);
}
void ejecutar_pcb(t_pcb* pcb, int id_cola_ready){
	pasar_pcb_a_estado(pcb, EXEC);

	while(cocinero_esta_ejecutando(pcb)){
		if(evaluar_desalojo(pcb,id_cola_ready)){
			log_info(logger,"[PLANIFICADOR] Se desaloja PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d",pcb->nombre_plato,pcb->id,pcb->id_pedido);
			desalojar_pcb(pcb);
			liberar_cocinero_asignado_a_plato(pcb);
			t_afinidad* afinidad = obtener_afinidad(pcb->nombre_plato);
			sem_post(&sem_cola_ready[afinidad->id_afinidad]);
			break;
		}
		sem_post(&sem_realizar_paso[pcb->cocinero_asignado]);
		sem_wait(&sem_fin_paso[id_cola_ready]);

		if(plato_sin_pasos_para_ejecutar(pcb)){
			pasar_pcb_a_estado(pcb,EXIT);
			liberar_cocinero_asignado_a_plato(pcb);
			enviar_plato_listo_a_sindicato(pcb->id_pedido,pcb->nombre_plato);
			enviar_plato_listo_a_modulo_solicitante(pcb->id_pedido,pcb->nombre_plato);
			if(pedido_esta_terminado(pcb->id_pedido)){
				enviar_terminar_pedido_a_sindicato(pcb->id_pedido);
				enviar_finalizar_pedido_a_cliente(pcb->id_pedido);
				liberar_pcbs_de_pedido(pcb->id_pedido);
				break;
			}
			else{
//				t_afinidad* afinidad = obtener_afinidad(pcb->nombre_plato);
				sem_post(&sem_cola_ready[id_cola_ready]);
			}
		}else if(pcb->estado == BLOCKED_POR_HORNO || pcb->estado == BLOCKED_POR_REPOSAR){
//			t_afinidad* afinidad = obtener_afinidad(pcb->nombre_plato);
			sem_post(&sem_cola_ready[id_cola_ready]);
		}
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
t_cocinero* obtener_cocinero_sin_afinidad(){
	bool no_tiene_afinidad(t_cocinero* cocinero){
		return string_equals_ignore_case("N",cocinero->afinidad);
	}
	t_cocinero* cocinero=list_find(lista_cocineros,(void*)no_tiene_afinidad);

	return cocinero;
}
void asignar_cocinero_a_pcb(t_cocinero*cocinero,t_pcb* pcb){
	//SE ASIGNA COCINERO CON AFINIDAD PARA LUEGO EJECUTAR PLATO
	pthread_mutex_lock(&cocinero->mutex_cocinero);
	cocinero->disponible=false;
	cocinero->pcb = pcb;
	pthread_mutex_unlock(&cocinero->mutex_cocinero);

	pthread_mutex_lock(&pcb->mutex_pcb);
	pcb->cocinero_asignado = cocinero->id;
	pthread_mutex_unlock(&pcb->mutex_pcb);
}
t_cocinero* buscar_cocinero_disponible(t_pcb* pcb){
	t_afinidad* afinidad=obtener_afinidad(pcb->nombre_plato);

	bool tiene_afinidad_y_esta_disponible(t_cocinero* cocinero){
		return string_equals_ignore_case(cocinero->afinidad,afinidad->nombre_afinidad) && cocinero->disponible;
	}
	t_cocinero* cocinero = list_find(lista_cocineros,(void*)tiene_afinidad_y_esta_disponible);

	return cocinero;
}
void planificar_platos(int* id_cola_ready){
	while(1){
		sem_wait(&sem_cola_ready[*id_cola_ready]);
		t_pcb* pcb = obtener_proximo_pcb_a_ejecutar(*id_cola_ready);

		if(pcb!=NULL){
			t_cocinero* cocinero_disponible = buscar_cocinero_disponible(pcb);

			if(cocinero_disponible!=NULL){
				asignar_cocinero_a_pcb(cocinero_disponible,pcb);
				ejecutar_pcb(pcb,*id_cola_ready);
			}
		}
	}
}
/**INICIALIZACIONES**/
bool evaluar_desalojo_por_RR(t_pcb* pcb,int id_cola_ready){

	pthread_mutex_lock(&pcb->mutex_pasos);
	t_paso_receta* paso=list_get(pcb->lista_pasos,0);
	pthread_mutex_unlock(&pcb->mutex_pasos);

//	log_info(logger,"[PLANIFICADOR] EVALUANDO DESALOJO");
//	log_info(logger,"[PLANIFICADOR] PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d",pcb->nombre_plato,pcb->id,pcb->id_pedido);
//	log_info(logger,"[PLANIFICADOR] PASO:%s",paso->accion);
//	log_info(logger,"[PLANIFICADOR] TIEMPO_PASO:%d",paso->tiempo);
//	log_info(logger,"[PLANIFICADOR] QUANTUM_CONSUMIDO:%d",pcb->quantum_consumido);

	if(string_equals_ignore_case(paso->accion,"HORNEAR"))
		return false;
	else
		return pcb->quantum_consumido >= QUANTUM;
}
bool no_evalua_desalojo(){
	return false;
}
void setear_algoritmo_FIFO(){
	algoritmo_planificacion = FIFO;
	obtener_proximo_pcb_a_ejecutar = obtener_proximo_pcb_a_ejecutar_por_FIFO;
	evaluar_desalojo = no_evalua_desalojo;
}
void setear_algoritmo_RR(){
	algoritmo_planificacion = RR;
	obtener_proximo_pcb_a_ejecutar = obtener_proximo_pcb_a_ejecutar_por_FIFO;
	evaluar_desalojo = evaluar_desalojo_por_RR;
	QUANTUM = restaurante_conf.quantum;
}
void inicializar_algoritmo(){
	if (strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "FIFO") == 0)
			setear_algoritmo_FIFO();
		else
			setear_algoritmo_RR();

	RETARDO_CICLO_CPU = restaurante_conf.retardo_ciclo_cpu;
}
void inicializar_colas_ready(){
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
//		log_info(logger,"Se agrega afinidad:%s con id_afinidad:%d a AFINIDADES_MAESTRO",afinidad->nombre_afinidad,afinidad->id_afinidad);
		//SE CREA COLA READY POR AFINIDAD
		lista_colas_ready[i] = list_create();
//		log_info(logger,"Se crea cola ready para afinidad:%s con id:%d",afinidad->nombre_afinidad,i);
		//SE CREA MUTEX PARA CADA COLA READY
		pthread_mutex_init(&mutex_colas_ready[i],NULL);
	}
	free(afinidades_distinct);
}
void inicializar_sem_mutex(){
	pthread_mutex_init(&mutex_id_pcb,NULL);
	pthread_mutex_init(&mutex_id_pedidos,NULL);
	pthread_mutex_init(&mutex_cola_bloqueados_prehorno,NULL);
	pthread_mutex_init(&mutex_cola_exit,NULL);
	pthread_mutex_init(&mutex_cocineros,NULL);
}
void inicializar_sem_contadores(){
//	sem_init(&sem_hornear_plato,0,0);
	sem_init(&sem_hay_espacio_en_horno,0,metadata_restaurante.cantidad_hornos);
	sem_init(&sem_planificar_cola_hornos,0,0);

	sem_realizar_paso = malloc(sizeof(sem_t)*list_size(metadata_restaurante.afinidades_cocineros));
	for(int i=0;i<list_size(metadata_restaurante.afinidades_cocineros);i++){
		sem_init(&sem_realizar_paso[i],0,0);
	}

	sem_cola_ready = malloc(sizeof(sem_t)*list_size(AFINIDADES_MAESTRO));
	sem_fin_paso = malloc(sizeof(sem_t)*list_size(AFINIDADES_MAESTRO));

	for(int i=0;i<list_size(AFINIDADES_MAESTRO);i++){
			sem_init(&sem_cola_ready[i],0,0);
			sem_init(&sem_fin_paso[i],0,0);
	}

	sem_hornear_plato = malloc(sizeof(sem_t)*metadata_restaurante.cantidad_hornos);
	for(int i=0;i<metadata_restaurante.cantidad_hornos;i++){
		sem_init(&sem_hornear_plato[i],0,0);
	}
}
void inicializar_hilos_cocineros(){
	lista_cocineros = list_create();

	for(int i=0;i<list_size(metadata_restaurante.afinidades_cocineros);i++){
		t_cocinero* cocinero = malloc(sizeof(t_cocinero));
		strcpy(cocinero->afinidad,list_get(metadata_restaurante.afinidades_cocineros,i));
		cocinero->id = i;
		cocinero->pcb = NULL;
		cocinero->disponible = true;
		strcpy(cocinero->afinidad,list_get(metadata_restaurante.afinidades_cocineros,i));
		pthread_mutex_init(&cocinero->mutex_cocinero,NULL);

		pthread_mutex_lock(&mutex_cocineros);
		list_add(lista_cocineros,cocinero);
		pthread_mutex_unlock(&mutex_cocineros);

		pthread_t thread_cocinero;
		pthread_create(&thread_cocinero,NULL,(void*)hilo_cocinero,cocinero);
		pthread_detach(thread_cocinero);
	}
}
void inicializar_hilos_planificadores(){
	for(int i=0;i<list_size(AFINIDADES_MAESTRO);i++){
		//SE CREA HILO PLANIFICADOR PARA CADA COLA READY
		t_afinidad* afinidad = list_get(AFINIDADES_MAESTRO,i);
		pthread_t hilo_planificador;
		pthread_create(&hilo_planificador,NULL,(void*)planificar_platos,&afinidad->id_afinidad);
		pthread_detach(hilo_planificador);
	}
}
void inicializar_hilo_horno(){
	lista_hornos=list_create();

	pthread_t hilo_planificador_hornos;
	pthread_create(&hilo_planificador_hornos,NULL,(void*)planificar_cola_hornos,NULL);
	pthread_detach(hilo_planificador_hornos);

	pthread_mutex_init(&mutex_lista_hornos,NULL);
	mutex_hornos = malloc(sizeof(pthread_mutex_t)*metadata_restaurante.cantidad_hornos);

	for(int i=0;i<metadata_restaurante.cantidad_hornos;i++){
		t_horno* horno = malloc(sizeof(t_horno));
		horno->id_horno = i;
		horno->disponible = true;

		pthread_mutex_lock(&mutex_lista_hornos);
		list_add(lista_hornos,horno);
		pthread_mutex_unlock(&mutex_lista_hornos);

		pthread_mutex_init(&mutex_hornos[i],NULL);

		pthread_t hilo_horno;
		pthread_create(&hilo_horno,NULL,(void*)planificar_horno,&horno->id_horno);
		pthread_detach(hilo_horno);
	}
}
void inicializar_cola_exit(){
	cola_exit = list_create();
}
void inicializar_cola_bloqueados_prehorno(){
	cola_bloqueados_prehorno = list_create();
}
void inicializar_planificador(){
	inicializar_algoritmo();
	inicializar_colas_ready();
	inicializar_cola_exit();
	inicializar_cola_bloqueados_prehorno();
	inicializar_sem_contadores();
	inicializar_sem_mutex();
	inicializar_hilos_planificadores();
	inicializar_hilos_cocineros();
	inicializar_hilo_horno();

	pedidos_pcbs = list_create();
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
		i++;
	}
}
void aplicar_retardo(int tiempo_a_consumir){
	int retardo = RETARDO_CICLO_CPU*tiempo_a_consumir;
	sleep(retardo);
}
