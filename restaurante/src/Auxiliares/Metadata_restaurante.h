/*
 * Metadata_restaurante.h
 *
 *  Created on: 2 nov. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_METADATA_RESTAURANTE_H_
#define AUXILIARES_METADATA_RESTAURANTE_H_

typedef struct{
	int pos_x;
	int pos_y;
	int cantidad_hornos;
	t_list* afinidades_cocineros;
	t_list* platos;
}t_metadata_restaurante;

uint32_t id_pedidos;//Id actual del ultimo pedido realizado(manipular con el semaforo)
pthread_mutex_t mutex_id_pedidos;//Semaforo para incrementar el id_pedidos

t_metadata_restaurante metadata_restaurante;

#endif /* AUXILIARES_METADATA_RESTAURANTE_H_ */
