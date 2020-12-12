/*
 * Hilos.h
 *
 *  Created on: 8 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_HILOS_H_
#define AUXILIARES_HILOS_H_

pthread_mutex_t mutex_id_pcb;
pthread_mutex_t mutex_afinidades_maestro;
//pthread_mutex_t mutex_hornos;
pthread_mutex_t mutex_id_pedidos;//Semaforo para incrementar el id_pedidos
pthread_mutex_t mutex_cola_bloqueados_prehorno;
pthread_mutex_t mutex_cola_exit;
pthread_mutex_t* mutex_colas_ready;
pthread_mutex_t mutex_pedidos_pcbs;

pthread_mutex_t mutex_pedidos_app;
pthread_mutex_t mutex_cliente_conectados;
pthread_mutex_t mutex_cocineros;

#endif /* AUXILIARES_HILOS_H_ */
