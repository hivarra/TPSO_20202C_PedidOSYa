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

		// TODO: Mover a exec
		ejecutarPCB(pcb);
		sem_post(&repartidor->sem_moverse);




//		t_entrenador* entrenador = sacar_primer_entrenador_de_listos();
//		agregar_entrenador_a_ejecutando(entrenador);
////		imprimir_entrenador(entrenador);
//		int cpu_disponible = calcular_quantum_sin_restriccion(entrenador);
//		entrenador->quantum_disponible = cpu_disponible + 1; // Le sumo uno por el catch que tiene que mandar
//
//		log_info(logger, "PLANIF_CP | Entrenador %d tiene %d CPU disponible", entrenador->id, cpu_disponible);
//
//		entrenador->esta_limitado = 0;
//		sem_post(&entrenador->sem_moverse);
//
//		sem_wait(&sem_finaliza_movimiento);
//
//		// TODO: Lo saco de ejecutado
//		sacar_entrenador_de_ejecutando(entrenador);
//		if(entrenador->estado==EN_ESPERA){
//			agregar_entrenador_a_bloqueados(entrenador);
//		} else if (entrenador->estado==ATRAPAR) {
//			agregar_entrenador_a_bloqueados(entrenador);
//		} else if(cumplio_objetivo(entrenador)) {
//			// TODO: Si cumple objetivo pasa a FINALIZADOS
//			agregar_entrenador_a_finalizados(entrenador);
//		} else {
//			// TODO: Si no cumple, validar si lleno su capacidad
//			if(atrapo_cantidad_maxima(entrenador)) {
//				entrenador->estado = INTERBLOQUEO;
//			} else {
//
//				//TODO: Validar si está en espera a recibir mensaje CAUGHT de BROKER
//				entrenador->estado = DISPONIBLE;
//			}
//			agregar_entrenador_a_bloqueados(entrenador);
//		}
//
//		// TODO: Valido si se cumple objetivo global
//		if(listas_estan_vacias()) {
//			log_info(logger, "TEAM | Ganaste la liga pokemon capo!");
//			sem_post(&sem_metricas);
//		}
//
	}

	return NULL;
}

void* planificador_hrrn() {

	log_info(logger, "PLANIFICACION | Algoritmo: HRRN");
	return NULL;
}

void* planificador_sjf() {

	log_info(logger, "PLANIFICACION | Algoritmo: SJF sin desalojo");
	return NULL;
}
