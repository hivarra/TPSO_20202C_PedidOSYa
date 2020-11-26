/*
 * Swap.h
 *
 *  Created on: 26 oct. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_SWAP_H_
#define AUXILIARES_SWAP_H_

#include "MemoriaPrincipal.h"
#include "Utils.h"

int puntero_clock;
pthread_mutex_t mutex_swap;
pthread_mutex_t mutex_reemplazo;

void volcar_pagina_a_swap(t_entrada_pagina*, t_pagina*);
void inicializar_memoria_swap(void);
void reemplazo_de_pagina(t_entrada_pagina*);
//t_entrada_pagina* buscar_plato_en_swap(t_list*, char*);

#endif /* AUXILIARES_SWAP_H_ */
