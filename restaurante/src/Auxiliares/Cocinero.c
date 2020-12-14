/*
 * Cocinero.c
 *
 *  Created on: 6 nov. 2020
 *      Author: utnso
 */


#include "Cocinero.h"

t_paso_receta* obtener_siguiente_paso(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pasos);
	t_paso_receta* siguiente_paso = list_get(pcb->lista_pasos,0);
	pthread_mutex_unlock(&pcb->mutex_pasos);

	return siguiente_paso;
}
void eliminar_paso_realizado(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pasos);
	list_remove_and_destroy_element(pcb->lista_pasos,0,free);
	pthread_mutex_unlock(&pcb->mutex_pasos);

}
void reposar(t_cocinero* cocinero){
	pasar_pcb_a_estado(cocinero->pcb,BLOCKED_POR_REPOSAR);
}
void hornear(t_cocinero* cocinero){
	pasar_pcb_a_estado(cocinero->pcb,BLOCKED_POR_HORNO);
}
void realizar_otro_paso(t_cocinero* cocinero,t_paso_receta* paso){
	uint32_t tiempo=0;

	if(algoritmo_planificacion == FIFO)
		tiempo=paso->tiempo;
	else{
		if(cocinero->pcb->quantum_consumido > 0){//paso_tiempo=3,
			tiempo=QUANTUM-cocinero->pcb->quantum_consumido;
			cocinero->pcb->quantum_consumido+=tiempo;
			paso->tiempo-=tiempo;
		}else{
			tiempo = min(QUANTUM,paso->tiempo);
			cocinero->pcb->quantum_consumido+=tiempo;
			paso->tiempo -= tiempo;
		}
	}
	pthread_mutex_lock(&cocinero->mutex_cocinero);
	aplicar_retardo(tiempo);
	pthread_mutex_unlock(&cocinero->mutex_cocinero);
	log_info(logger,"[COCINERO] Se ejecuta %d rafagas de CPU de PASO:%s de PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d",tiempo,paso->accion,cocinero->pcb->nombre_plato,cocinero->pcb->id,cocinero->pcb->id_pedido);
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
		log_info(logger,"PRUEBA ID_PCB:%d",cocinero->pcb->id);
		log_info(logger,"PRUEBA ESTADO:%s", estado_pcb_enum_a_string(cocinero->pcb->estado));
//		log_info(logger,"[HILO_COCINERO] Cocinero con ID:%d ejecuta PASO:%s del PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d",cocinero->id,paso_siguiente->accion,cocinero->pcb->nombre_plato,cocinero->pcb->id,cocinero->pcb->id_pedido);
		string_to_upper(paso_siguiente->accion);

		if(string_equals_ignore_case(paso_siguiente->accion,"REPOSAR")){
			reposar(cocinero);
			log_info(logger,"[COCINERO] Fin de ejecución de PASO:%s de PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d",paso_siguiente->accion,cocinero->pcb->nombre_plato,cocinero->id,cocinero->pcb->id_pedido);
			pthread_mutex_lock(&cocinero->pcb->mutex_pasos);
			eliminar_paso(paso_siguiente,cocinero->pcb);
			pthread_mutex_unlock(&cocinero->pcb->mutex_pasos);
		}
		else if(string_equals_ignore_case(paso_siguiente->accion,"HORNEAR")){
			hornear(cocinero);
		}
		else{
			realizar_otro_paso(cocinero,paso_siguiente);
			log_info(logger,"[COCINERO] Fin de ejecución de PASO:%s de PLATO:%s,ID_PCB:%d,ID_PEDIDO:%d",paso_siguiente->accion,cocinero->pcb->nombre_plato,cocinero->pcb->id,cocinero->pcb->id_pedido);
			eliminar_paso(paso_siguiente,cocinero->pcb);
			t_afinidad* afinidad=obtener_afinidad(cocinero->afinidad);
			sem_post(&sem_fin_paso[afinidad->id_afinidad]);
		}
	}
}
bool cocinero_esta_ejecutando(t_pcb* pcb){
	pthread_mutex_lock(&pcb->mutex_pcb);
	t_estado_pcb estado = pcb->estado;
	pthread_mutex_unlock(&pcb->mutex_pcb);

	return estado == EXEC;
}
t_cocinero* obtener_cocinero_por_id(uint32_t id_cocinero){
	bool tiene_id_cocinero(t_cocinero* cocinero){
		return cocinero->id == id_cocinero;
	}
	pthread_mutex_lock(&mutex_cocineros);
	t_cocinero* cocinero=list_find(lista_cocineros,(void*)tiene_id_cocinero);
	pthread_mutex_unlock(&mutex_cocineros);

	return cocinero;
}
void liberar_cocinero_asignado_a_plato(t_pcb* pcb){
	bool tiene_id_cocinero(t_cocinero* cocinero){
		return cocinero->id == pcb->cocinero_asignado;
	}
	pthread_mutex_lock(&mutex_cocineros);
	t_cocinero* cocinero=list_find(lista_cocineros,(void*)tiene_id_cocinero);
	pthread_mutex_unlock(&mutex_cocineros);

	pthread_mutex_lock(&cocinero->mutex_cocinero);
	cocinero->disponible=true;
	cocinero->pcb=NULL;
	pthread_mutex_unlock(&cocinero->mutex_cocinero);
}
