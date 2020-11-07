/*
 * Plato.h
 *
 *  Created on: 6 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_PLATO_H_
#define AUXILIARES_PLATO_H_

#include "shared.h"

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
	uint32_t estado;
	uint32_t cocinero_asignado;
	uint32_t quantum;
	t_list* lista_pasos;
}t_pcb;


#endif /* AUXILIARES_PLATO_H_ */
