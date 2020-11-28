/*
 * Utils.c
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */
#include "Utils.h"

char* instruccion_str[] = { "BUSCAR_PEDIDO", "ESPERAR_PEDIDO", "ENTREGAR_PEDIDO", "DESCANSAR", NULL };

void imprimirRepartidor(t_repartidor* repartidor) {

	log_info(logger, "Repartidor N°%d | Posición: %d-%d | Objetivo: %d-%d | Descanso: %d | Duración: %d", repartidor->id, repartidor->posX, repartidor->posY, repartidor->objetivo_posX, repartidor->objetivo_posY, repartidor->frecuenciaDescanso, repartidor->tiempoDescanso);

}

void imprimirPCB(t_pcb* pcb) {

//	log_info(logger, "PCB | Pedido: %d | Repartidor: %d | Instrucción: %d |PosX: %d | PosY: %d", pcb->id_pedido, pcb->id_repartidor, pcb->instruccion, pcb->posX, pcb->posY);
	log_info(logger, "PCB | Pedido: %d | Repartidor: %d | Instrucción: %d | Posición: %d-%d", pcb->id_pedido, pcb->id_repartidor, pcb->instruccion, pcb->restaurante_posX, pcb->restaurante_posY);
}

void disponibilizar_repartidor(t_repartidor* repartidor) {

	repartidor->instruccion = BUSCAR_PEDIDO;
	repartidor->disponible = 1;
	sem_post(&sem_repartidor_disponible);

	log_info(logger, "Repartidor N°%d disponible", repartidor->id);
}

t_repartidor* repartidor_mas_cercano(int posX, int posY) {

	t_repartidor* repartidor_cercano ;
	int menor_distancia = 100000;

	int disponibles(t_repartidor* repartidor) {
		return repartidor->disponible;
	}

	t_list* repartidores_disponibles = list_filter(repartidores, (void*)disponibles);

	void mas_cercano(t_repartidor* repartidor) {

		int distancia_actual = distancia_a_posicion(repartidor, posX, posY);
		if(distancia_actual < menor_distancia) {
			menor_distancia = distancia_actual;
			repartidor_cercano = repartidor;
		}
	}

	list_iterate(repartidores_disponibles, (void*)mas_cercano);

	return repartidor_cercano;
}

int distancia_a_posicion(t_repartidor* repartidor, int posX, int posY) {

	int x = distancia_absoluta(repartidor->posX, posX);
	int y = distancia_absoluta(repartidor->posY, posY);
	return x + y;

}

int distancia_absoluta(int pos1, int pos2) {

	return fabs(pos1 - pos2);
}

void asignar_repartidor(t_repartidor* repartidor, t_pcb* pcb) {

	pcb->id_repartidor = repartidor->id;
	repartidor->disponible = 0;
	repartidor->objetivo_posX = pcb->restaurante_posX;
	repartidor->objetivo_posY = pcb->restaurante_posY;

	log_info(logger, "Repartidor N°%d asignado a pedido N°%d", repartidor->id, pcb->id_pedido);
}

t_repartidor* obtener_repartidor(int id_repartidor) {

	int es_igual(t_repartidor* repartidor) {
		return repartidor->id == id_repartidor;
	}

	return list_find(repartidores, (void*)es_igual);
}

char* get_nombre_instruccion(t_instruccion enum_instruccion) {

	return instruccion_str[enum_instruccion];
}

void aplicar_retardo_operacion() {

	sleep(app_conf.retardo_ciclo_cpu);
}

void ejecutarPCB(t_pcb* pcb) {

	pthread_mutex_lock(&mutex_ejecutando);
	list_add(ejecutando, pcb);
	pthread_mutex_unlock(&mutex_ejecutando);
	log_info(logger, "PCB | Pedido N°%d paso a Ejecutando por Repartidor N°%d", pcb->id_pedido, pcb->id_repartidor);
}

void bloquearPCB(t_repartidor* repartidor) {

//	int esElPCB(t_pcb* pcb) {
//
//		return pcb->id_repartidor == repartidor->id;
//	}
//
//	pthread_mutex_lock(&mutex_ejecutando);
//	t_pcb* pcb = list_remove_by_condition(ejecutando, (void*)esElPCB);
//	pthread_mutex_unlock(&mutex_ejecutando);

//	sem_post(&sem_limite_exec); //Libero espacio de EXEC por si hay algún PCB esperando

	t_pcb* pcb = sacarPCBDeEjecutando(repartidor);

	pthread_mutex_lock(&mutex_bloqueados);
	list_add(bloqueados, pcb);
	pthread_mutex_unlock(&mutex_bloqueados);

	log_info(logger, "PCB | Pedido N°%d bloqueado por Repartidor N°%d en Estado %s", pcb->id_pedido, repartidor->id, get_nombre_instruccion(repartidor->instruccion));

	if(repartidor->instruccion == DESCANSAR) {

		// Descansa, y pasa a READY
		log_info(logger, "Repartidor N°%d | Entra en descanso %d segundos", repartidor->id, repartidor->tiempoDescanso);
		sleep(repartidor->tiempoDescanso);
		repartidor->tiempoDescanso = atoi(app_conf.tiempos_descanso[repartidor->id - 1]);

		pthread_mutex_lock(&mutex_listos);
		list_add(listos, pcb);
		pthread_mutex_unlock(&mutex_listos);

		sem_post(&sem_ready);

	}
}

void retirarPedido(t_pcb* pcb) {

	t_repartidor* repartidor = obtener_repartidor(pcb->id_repartidor);
	repartidor->instruccion = ENTREGAR_PEDIDO;
	repartidor->objetivo_posX = pcb->cliente_posX;
	repartidor->objetivo_posY = pcb->cliente_posY;

	log_info(logger, "Repartidor N°%d retiró el Pedido N°%d", repartidor->id, pcb->id_pedido);
//	imprimirRepartidor(repartidor);

	int esElPCB(t_pcb* pcb) {
		return pcb->id_repartidor == repartidor->id;
	}

	pthread_mutex_lock(&mutex_bloqueados);
	list_remove_by_condition(bloqueados, (void*)esElPCB);
	pthread_mutex_unlock(&mutex_bloqueados);

	pthread_mutex_lock(&mutex_listos);
	list_add(listos, pcb);
	pthread_mutex_unlock(&mutex_listos);

	sem_post(&sem_ready);

}

t_pcb* sacarPCBDeEjecutando(t_repartidor* repartidor) {

	int esElPCB(t_pcb* pcb) {

		return pcb->id_repartidor == repartidor->id;
	}

	pthread_mutex_lock(&mutex_ejecutando);
	t_pcb* pcb = list_remove_by_condition(ejecutando, (void*)esElPCB);
	pthread_mutex_unlock(&mutex_ejecutando);

	sem_post(&sem_limite_exec);

	return pcb;
}

void finalizarPCB(t_repartidor* repartidor) {

	t_pcb* pcb = sacarPCBDeEjecutando(repartidor);

	pthread_mutex_lock(&mutex_finalizados);
	list_add(finalizados, pcb);
	pthread_mutex_unlock(&mutex_finalizados);

	log_info(logger, "PCB | Pedido N°%d finalizado", pcb->id_pedido);

	disponibilizar_repartidor(repartidor);
}
