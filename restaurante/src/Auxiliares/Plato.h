/*
 * Plato.h
 *
 *  Created on: 6 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_PLATO_H_
#define AUXILIARES_PLATO_H_

#include "shared.h"
#include "protocolo.h"

typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCKED_POR_REPOSAR,
	BLOCKED_POR_HORNO,
	EXIT
}t_estado_pcb;

typedef struct{
	uint32_t id;
	uint32_t id_pedido;
	char nombre_plato[L_PLATO];
	t_estado_pcb estado;
	uint32_t cocinero_asignado;
	t_list* lista_pasos;//lista de t_paso_receta
	pthread_mutex_t mutex_pasos;
	pthread_mutex_t mutex_pcb;
	uint32_t quantum_consumido;
}t_pcb;

bool plato_sin_pasos_para_ejecutar(t_pcb* pcb);

#endif /* AUXILIARES_PLATO_H_ */
