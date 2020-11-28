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
void eliminar_paso_realizado(t_paso_receta* paso){
	free(paso->accion);
	free(&paso->tiempo);
	free(paso);
}
void reposar(t_cocinero* cocinero){
	pasar_pcb_a_estado(cocinero->pcb,BLOCKED_POR_REPOSAR);
	sem_post(&finCPUbound);
}
void hornear(t_cocinero* cocinero){
	pasar_pcb_a_estado(cocinero->pcb,BLOCKED_POR_HORNO);
	sem_post(&finCPUbound);
}
void hilo_cocinero(t_cocinero* cocinero){
	log_info(logger,"Cocinero con ID:%d creado",cocinero->id);

	while(1){
		sem_wait(&sem_realizar_paso[cocinero->id]);
		sleep(RETARDO_CICLO_CPU);

		t_paso_receta* paso_siguiente = obtener_siguiente_paso(cocinero->pcb);
		string_to_upper(paso_siguiente->accion);

		if(string_equals_ignore_case(paso_siguiente->accion,"REPOSAR")){
			reposar(cocinero);
		}
		else if(string_equals_ignore_case(paso_siguiente->accion,"HORNEAR")){
			hornear(cocinero);
		}
		else{
			/*TODO:PROCESAR OTRO PASO*/
		}
		eliminar_paso_realizado(paso_siguiente);
	}
}
bool cocinero_esta_ejecutando(t_pcb* pcb){
	pthread_mutex_lock(&mutex_platos[pcb->id]);
	t_estado_pcb estado = pcb->estado;
	pthread_mutex_unlock(&mutex_platos[pcb->id]);

	return estado == EXEC;
}
