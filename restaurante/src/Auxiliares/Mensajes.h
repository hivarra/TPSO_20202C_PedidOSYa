/*
 * mensajes.h
 *
 *  Created on: 21 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_MENSAJES_H_
#define AUXILIARES_MENSAJES_H_

#include <protocolo.h>
#include "Conexion.h"

void procesar_socket_envio(t_socket_envio*, int*);
void procesar_socket_escucha(t_socket_escucha*, int*);

t_rta_consultar_platos* procesar_consultar_platos(void);
uint32_t procesar_crear_pedido(void);
uint32_t procesar_anadir_plato(t_anadir_plato*);
uint32_t procesar_confirmar_pedido(t_confirmar_pedido*);
t_rta_consultar_pedido* procesar_consultar_pedido(uint32_t);

#endif /* AUXILIARES_MENSAJES_H_ */
