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
void eliminar_paso_realizado(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pasos);
	list_remove_and_destroy_element(pcb->lista_pasos,0,free);
	pthread_mutex_unlock(&pcb->mutex_pasos);

}
void reposar(t_cocinero* cocinero){
	pasar_pcb_a_estado(cocinero->pcb,BLOCKED_POR_REPOSAR);
//	sem_post(&finCPUbound);
}
void hornear(t_cocinero* cocinero){
	pasar_pcb_a_estado(cocinero->pcb,BLOCKED_POR_HORNO);
//	sem_post(&finCPUbound);
}
void realizar_otro_paso(t_cocinero* cocinero,t_paso_receta* paso){
	log_info(logger,"[COCINERO] Se ejecuta PASO:%s de PLATO:%s,PCB:%d,ID_PEDIDO:%d",paso->accion,cocinero->pcb->nombre_plato,cocinero->pcb->id,cocinero->pcb->id_pedido);
	uint32_t tiempo=0;

	if(algoritmo_planificacion == FIFO)
		tiempo=paso->tiempo;
	else{
		tiempo = min(QUANTUM,paso->tiempo);
		cocinero->pcb->quantum_consumido=tiempo;
		paso->tiempo -= tiempo;
	}

	pthread_mutex_lock(&cocinero->mutex_cocinero);
	aplicar_retardo(tiempo);
	pthread_mutex_unlock(&cocinero->mutex_cocinero);
}
void eliminar_paso(t_paso_receta* paso,t_pcb* plato){
	if(algoritmo_planificacion==FIFO)
		eliminar_paso_realizado(plato);
	else{
		if(string_equals_ignore_case(paso->accion,"HORNEAR") || string_equals_ignore_case(paso->accion,"REPOSAR"))
			eliminar_paso_realizado(plato);
		else if (paso->tiempo==0)
			eliminar_paso_realizado(plato);
	}
}
void hilo_cocinero(t_cocinero* cocinero){

	while(1){
		sem_wait(&sem_realizar_paso[cocinero->id]);
		t_paso_receta* paso_siguiente = obtener_siguiente_paso(cocinero->pcb);
		log_info(logger,"[HILO_COCINERO] Cocinero con ID:%d ejecuta PASO:%s del PLATO:%s de ID_PCB:%d del ID_PEDIDO:%d",cocinero->id,paso_siguiente->accion,cocinero->pcb->nombre_plato,cocinero->pcb->id,cocinero->pcb->id_pedido);
		string_to_upper(paso_siguiente->accion);
//		log_info(logger,"[HILO_COCINERO] Cocinero con ID:%d ejecuta PASO:%s del PLATO:%s del ID_PEDIDO:%d",cocinero->id,paso_siguiente->accion,cocinero->pcb->nombre_plato,cocinero->pcb->id_pedido);

		if(string_equals_ignore_case(paso_siguiente->accion,"REPOSAR")){
			reposar(cocinero);
			log_info(logger,"[COCINERO] Fin de ejecución de PASO:%s de PLATO:%s del ID_PEDIDO:%d",paso_siguiente->accion,cocinero->pcb->nombre_plato,cocinero->pcb->id_pedido);
			pthread_mutex_lock(&cocinero->pcb->mutex_pasos);
			eliminar_paso(paso_siguiente,cocinero->pcb);
			pthread_mutex_unlock(&cocinero->pcb->mutex_pasos);
			sem_post(&sem_fin_paso);
		}
		else if(string_equals_ignore_case(paso_siguiente->accion,"HORNEAR")){
			hornear(cocinero);
		}
		else{
			realizar_otro_paso(cocinero,paso_siguiente);
			log_info(logger,"[COCINERO] Fin de ejecución de PASO:%s de PLATO:%s de ID_PCB:%d del ID_PEDIDO:%d",paso_siguiente->accion,cocinero->pcb->nombre_plato,cocinero->pcb->id,cocinero->pcb->id_pedido);
//			pthread_mutex_lock(&cocinero->pcb->mutex_pasos);
			eliminar_paso(paso_siguiente,cocinero->pcb);
//			pthread_mutex_unlock(&cocinero->pcb->mutex_pasos);
			sem_post(&sem_fin_paso);
		}
	}
}
bool cocinero_esta_ejecutando(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pcb);
	t_estado_pcb estado = pcb->estado;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	return estado == EXEC;
}
