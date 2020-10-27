/*
 * Swap.h
 *
 *  Created on: 26 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_SWAP_H_
#define AUXILIARES_SWAP_H_

#include "MemoriaPrincipal.h"

int puntero_clock;

void inicializar_memoria_swap(void);
void liberar_memoria_swap();
t_entrada_pagina* buscar_plato_en_swap(t_list*, char*);

#endif /* AUXILIARES_SWAP_H_ */
