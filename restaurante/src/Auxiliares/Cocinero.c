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
void realizar_otro_paso(t_cocinero* cocinero,t_paso_receta* paso){
	log_info(logger,"[REALIZAR_OTRO_PASO] Se ejecuta PASO:%s",paso->accion);

	pthread_mutex_lock(&cocinero->mutex_cocinero);
	aplicar_retardo(paso->tiempo);
	pthread_mutex_unlock(&cocinero->mutex_cocinero);

	sem_post(&finCPUbound);
}
void hilo_cocinero(t_cocinero* cocinero){

	while(1){
		sem_wait(&sem_realizar_paso[cocinero->id]);

		t_paso_receta* paso_siguiente = obtener_siguiente_paso(cocinero->pcb);
		string_to_upper(paso_siguiente->accion);
		log_info(logger,"[HILO_COCINERO] Cocinero con ID:%d ejecuta PASO:%s del PLATO:%s del ID_PEDIDO:%d",cocinero->id,paso_siguiente->accion,cocinero->pcb->id_pedido);

		if(string_equals_ignore_case(paso_siguiente->accion,"REPOSAR")){
			reposar(cocinero);
		}
		else if(string_equals_ignore_case(paso_siguiente->accion,"HORNEAR")){
			hornear(cocinero);
		}
		else{
			realizar_otro_paso(cocinero,paso_siguiente);
		}
		eliminar_paso_realizado(paso_siguiente);
	}
}
bool cocinero_esta_ejecutando(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pcb);
	t_estado_pcb estado = pcb->estado;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	return estado == EXEC;
}
