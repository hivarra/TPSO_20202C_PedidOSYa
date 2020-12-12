/*
 * Utils.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_UTILS_H_
#define AUXILIARES_UTILS_H_

#include <signal.h>
#include "Conexion.h"
#include "Planificador.h"

typedef struct{
	int id_afinidad;
	char nombre_afinidad[L_PLATO];
}t_afinidad;

typedef enum{
	FIFO,
	RR
}t_algoritmo;

t_algoritmo algoritmo_planificacion;

uint32_t RETARDO_CICLO_CPU;



void signalHandler(int);
t_afinidad* obtener_id_afinidad(char nombre_plato[L_PLATO]);
int min(int num1, int num2);

#endif /* AUXILIARES_UTILS_H_ */
