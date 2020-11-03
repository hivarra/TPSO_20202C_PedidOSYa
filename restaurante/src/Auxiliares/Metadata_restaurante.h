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
	uint32_t cantidad_pedidos;
	t_list* cocineros;
	t_list* platos;
	t_list* clientes_conectados;//t_cliente
}t_metadata_restaurante;

t_metadata_restaurante* metadata_restaurante;

#endif /* AUXILIARES_METADATA_RESTAURANTE_H_ */
