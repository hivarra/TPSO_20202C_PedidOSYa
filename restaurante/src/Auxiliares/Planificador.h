/*
 * planificador.h
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_PLANIFICADOR_H_
#define AUXILIARES_PLANIFICADOR_H_

#include "Plato.h"
#include "Cocinero.h"
#include "Metadata_restaurante.h"
#include "Semaforos.h"
#include "Hilos.h"
#include <semaphore.h>

typedef enum{
	FIFO,
	RR
}t_algoritmo;

typedef struct{
	uint32_t id_afinidad;
	t_list* cola;
}t_cola_ready;

typedef struct{
	int id_afinidad;
	char nombre_afinidad[L_PLATO];
}t_afinidad;

t_list* AFINIDADES_MAESTRO;
t_list** lista_colas_ready;
uint32_t id_pcb_global;
t_list* cola_bloqueados_prehorno;
t_algoritmo algoritmo_planificacion;

int QUANTUM;

t_pcb* (*obtener_proximo_pcb_a_ejecutar)(int id_cola_ready);
bool (*evaluar_desalojo)(t_pcb*,int);

void inicializar_planificador(void);
void inicializar_planificacion(uint32_t id_pedido,t_rta_obtener_receta* rta_obtener_receta);
void pasar_pcb_a_estado(t_pcb* pcb, t_estado_pcb estado);
void aplicar_retardo(int tiempo_a_consumir);

#endif /* AUXILIARES_PLANIFICADOR_H_ */
