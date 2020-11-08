/*
 * Cocinero.c
 *
 *  Created on: 6 nov. 2020
 *      Author: utnso
 */


#include "Cocinero.h"

t_paso_receta* obtener_siguiente_paso(t_pcb* pcb){
	t_paso_receta* siguiente_paso = list_get(pcb->lista_pasos,0);
	return siguiente_paso;
}
void hilo_cocinero(t_cocinero* cocinero){
	log_info(logger,"Cocinero con ID:%d creado",cocinero->id);

	while(1){
		sem_wait(&sem_realizar_paso[cocinero->id]);
		sleep(RETARDO_CICLO_CPU);

		t_paso_receta* paso_siguiente = obtener_siguiente_paso(cocinero->pcb);
		string_to_upper(paso_siguiente->accion);
		if(string_equals_ignore_case(paso_siguiente->accion,"REPOSAR")){
			/*TODO:PROCESAR REPOSAR*/
		}
		else if(string_equals_ignore_case(paso_siguiente->accion,"HORNEAR")){
			/*TODO:PROCESAR HORNEAR*/
		}
		else{
			/*TODO:PROCESAR OTRO PASO*/
		}
	}
}
