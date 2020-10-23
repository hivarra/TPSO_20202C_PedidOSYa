/*
 * planificador.c
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */

#include "Planificador.h"

void inicializar_planificador(){
	if (strcmp(config_get_string_value(config, "ALGORITMO_PLANIFICACION"), "FIFO") == 0)
		algoritmo_planificacion = FIFO;
	else
		algoritmo_planificacion = RR;
}
