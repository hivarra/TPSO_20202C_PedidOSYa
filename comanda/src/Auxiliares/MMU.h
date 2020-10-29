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

#endif /* AUXILIARES_MMU_H_ */
