/*
 * MMU.h
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_MMU_H_
#define AUXILIARES_MMU_H_

#include <protocolo.h>
#include "MemoriaPrincipal.h"
#include "Swap.h"
#include "Utils.h"

uint32_t procesar_guardar_pedido(t_guardar_pedido* info_guardar_pedido);
uint32_t procesar_guardar_plato(t_guardar_plato* info_guardar_plato);
t_rta_obtener_pedido* procesar_obtener_pedido(t_obtener_pedido* info_obtener_pedido);
uint32_t procesar_confirmar_pedido(t_confirmar_pedido* info_confirmar_pedido);
uint32_t procesar_plato_listo(t_plato_listo* info_plato_listo);
uint32_t procesar_finalizar_pedido(t_finalizar_pedido* info_finalizar_pedido);

#endif /* AUXILIARES_MMU_H_ */
