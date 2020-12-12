/*
 * Planificador_CP.c
 *
 *  Created on: 1 nov. 2020
 *      Author: utnso
 */

#include "Planificador_CP.h"

void* planificador_fifo() {

	log_info(logger, "Planificador Corto Plazo | Algoritmo: FIFO");

	while(1) {

		sem_wait(&sem_ready);
		sem_wait(&sem_limite_exec);

		log_info(logger, "PCP | Planificando");

		pthread_mutex_lock(&mutex_listos);
		t_pcb* pcb = list_remove(listos, 0);
		pthread_mutex_unlock(&mutex_listos);
		t_repartidor* repartidor = obtener_repartidor(pcb->id_repartidor);
		int distancia = distancia_a_posicion(repartidor, repartidor->objetivo_posX, repartidor->objetivo_posY);
		repartidor->quantum = distancia;

		log_info(logger, "PCP | Repartidor N°%d tiene que moverse %d posiciones", repartidor->id, distancia);

		ejecutarPCB(pcb);
		sem_post(&repartidor->sem_moverse);

	}

	return NULL;
}

void* planificador_hrrn() {

	log_info(logger, "PLANIFICACION | Algoritmo: HRRN");

	while(1) {

		sem_wait(&sem_ready);
		sem_wait(&sem_limite_exec);

		log_info(logger, "PCP | Planificando");

		t_pcb* pcb = sacar_pcb_de_listos_por_HRRN();
		pcb->tiempo_espera_ready = 0;
		t_repartidor* repartidor = obtener_repartidor(pcb->id_repartidor);
		int distancia = distancia_a_posicion(repartidor, repartidor->objetivo_posX, repartidor->objetivo_posY);
		repartidor->quantum = distancia;

		log_info(logger, "PCP | Repartidor N°%d tiene que moverse %d posiciones", repartidor->id, distancia);

		ejecutarPCB(pcb);
		sem_post(&repartidor->sem_moverse);
	}

	return NULL;
}

void* planificador_sjf() {

	log_info(logger, "PLANIFICACION | Algoritmo: SJF sin desalojo");

	while(1) {

		sem_wait(&sem_ready);
		sem_wait(&sem_limite_exec);

		log_info(logger, "PCP | Planificando");

		t_pcb* pcb = sacar_pcb_de_listos_por_SJF();
		t_repartidor* repartidor = obtener_repartidor(pcb->id_repartidor);
		int distancia = distancia_a_posicion(repartidor, repartidor->objetivo_posX, repartidor->objetivo_posY);
		repartidor->quantum = distancia;

		log_info(logger, "PCP | Repartidor N°%d tiene que moverse %d posiciones", repartidor->id, distancia);

		ejecutarPCB(pcb);
		sem_post(&repartidor->sem_moverse);
	}
	return NULL;
}

double calcular_estimacion_SJF(t_pcb* pcb) {

	t_repartidor* repartidor = obtener_repartidor(pcb->id_repartidor);
	double alpha = app_conf.alpha;
	int est_real = distancia_a_posicion(repartidor, repartidor->objetivo_posX, repartidor->objetivo_posY);
	double nueva_estimacion = (alpha * est_real) + ((1 - alpha) * pcb->ultima_estimacion);

	log_info(logger, "Repartidor N°%d | Estimacion | Ultima: %f | Real: %d | Nueva: %f", repartidor->id, pcb->ultima_estimacion, est_real, nueva_estimacion);

	pcb->ultima_estimacion = nueva_estimacion;

	return nueva_estimacion;
}

void actualizar_estimaciones_SJF() {
	void iterar(t_pcb* pcb) {
		pcb->ultima_estimacion = calcular_estimacion_SJF(pcb);
	}

	list_iterate(listos, (void*) iterar);
}

t_pcb* sacar_pcb_de_listos_por_SJF() {

	pthread_mutex_lock(&mutex_listos);

	actualizar_estimaciones_SJF();

	t_pcb* pcb_SJF = list_get(listos, 0);

	void asignar(t_pcb* pcb_lista) {
		if(pcb_SJF->ultima_estimacion > pcb_lista->ultima_estimacion) {
			pcb_SJF = pcb_lista;
		}
	}

	int comparar(t_pcb* pcb_lista) {
		return pcb_SJF->id_pedido == pcb_lista->id_pedido;
	}

	list_iterate(listos, (void*)asignar);
	list_remove_by_condition(listos, (void*)comparar);
	pthread_mutex_unlock(&mutex_listos);
	log_info(logger, "PCP | Repartidor N°%d eliminado de LISTOS", pcb_SJF->id_repartidor);
	return pcb_SJF;
}

double calcular_estimacion_HRRN(t_pcb* pcb) {

	t_repartidor* repartidor = obtener_repartidor(pcb->id_repartidor);
	int tiempo_espera_ready = pcb->tiempo_espera_ready;
	int proxima_rafaga = distancia_a_posicion(repartidor, repartidor->objetivo_posX, repartidor->objetivo_posY);
	double nueva_estimacion = (proxima_rafaga + tiempo_espera_ready) / proxima_rafaga;

	log_info(logger, "Repartidor N°%d | Estimacion HRRN: %f", repartidor->id, nueva_estimacion);

	pcb->ultima_estimacion = nueva_estimacion;

	return nueva_estimacion;
}

void actualizar_estimaciones_HRRN() {
	void iterar(t_pcb* pcb) {
		pcb->ultima_estimacion = calcular_estimacion_HRRN(pcb);
	}

	list_iterate(listos, (void*) iterar);
}

t_pcb* sacar_pcb_de_listos_por_HRRN() {

	pthread_mutex_lock(&mutex_listos);

	actualizar_estimaciones_HRRN();

	t_pcb* pcb_SJF = list_get(listos, 0);

	void asignar(t_pcb* pcb_lista) {
		if(pcb_SJF->ultima_estimacion < pcb_lista->ultima_estimacion) {
			pcb_SJF = pcb_lista;
		}
	}

	int comparar(t_pcb* pcb_lista) {
		return pcb_SJF->id_pedido == pcb_lista->id_pedido;
	}

	list_iterate(listos, (void*)asignar);
	list_remove_by_condition(listos, (void*)comparar);
	pthread_mutex_unlock(&mutex_listos);
	log_info(logger, "PCP | Repartidor N°%d eliminado de LISTOS", pcb_SJF->id_repartidor);
	return pcb_SJF;
}
