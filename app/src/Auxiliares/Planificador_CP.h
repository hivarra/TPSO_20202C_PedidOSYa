/*
 * Planificador_CP.h
 *
 *  Created on: 1 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_PLANIFICADOR_CP_H_
#define AUXILIARES_PLANIFICADOR_CP_H_

#include "Planificador_LP.h"

void* planificador_fifo();
void* planificador_hrrn();
void* planificador_sjf();

double calcular_estimacion_SJF(t_pcb* pcb);
void actualizar_estimaciones_SJF();
t_pcb* sacar_pcb_de_listos_por_SJF();
double calcular_estimacion_HRRN(t_pcb* pcb);
void actualizar_estimaciones_HRRN();
t_pcb* sacar_pcb_de_listos_por_HRRN();

#endif /* AUXILIARES_PLANIFICADOR_CP_H_ */
