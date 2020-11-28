/*
 * Plato.c
 *
 *  Created on: 6 nov. 2020
 *      Author: utnso
 */


#include "Plato.h"

int obtener_id_pcb(t_pcb* pcb){
	return pcb->id;
}
bool plato_sin_pasos_para_ejecutar(t_pcb* pcb){
	return list_is_empty(pcb->lista_pasos);
}
