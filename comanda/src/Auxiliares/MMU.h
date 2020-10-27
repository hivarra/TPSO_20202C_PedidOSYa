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

t_dictionary *tablas_segmentos;//key:nombre_restaurante

void inicializar_memoria();
void inicializar_tablas_segmentos();
t_segmento* inicializar_segmento(int id_pedido);
void agregar_pagina_a_segmento(char* nombre_restaurante, int id_pedido, char* nombre_comida);
t_entrada_pagina* inicializar_entrada_pagina(char* nombre_comida);
uint32_t procesar_guardar_pedido(t_guardar_pedido* info_guardar_pedido);
uint32_t procesar_guardar_plato(t_guardar_plato* info_guardar_plato);

#endif /* AUXILIARES_MMU_H_ */
