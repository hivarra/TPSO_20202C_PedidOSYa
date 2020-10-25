/*
 * MMU.h
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_MMU_H_
#define AUXILIARES_MMU_H_
#define L_PLATO_MEM 24

#include "MemoriaPrincipal.h"
#include "protocolo.h"

typedef struct{
	u_int32_t nro_segmento;
	u_int32_t id_pedido;
	t_list* tabla_paginas;
	u_int32_t nro_pagina;
} t_segmento;

typedef struct{
	u_int32_t nro_pagina;
	u_int32_t nro_frame;
	bool uso;
	bool modificado;
	uint64_t ultimo_uso;
} t_entrada_pagina;

t_dictionary *tablas_segmentos;//key:nombre_restaurante

void inicializar_memoria();
void inicializar_tablas_segmentos();
t_segmento* inicializar_segmento(int id_pedido);
void agregar_pagina_a_segmento(char* nombre_restaurante, int id_pedido, char* nombre_comida);
t_entrada_pagina* inicializar_entrada_pagina(char* nombre_comida);
void liberar_tablas();
void liberar_memoria();
uint32_t procesar_guardar_pedido(t_guardar_pedido* info_guardar_pedido);
uint32_t procesar_guardar_plato(t_guardar_plato* info_guardar_plato);

#endif /* AUXILIARES_MMU_H_ */
