/*
 * MMU.h
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_MMU_H_
#define AUXILIARES_MMU_H_

#include "MemoriaPrincipal.h"

typedef struct{
	u_int32_t nro_segmento;
	u_int32_t id_pedido;
	t_list* tabla_paginas;
	u_int32_t nro_pagina;
} t_segmento;

typedef struct{
	u_int32_t nro_pagina;
	u_int32_t nro_frame;
	char plato[24];
	bool uso;
	bool modificado;
	uint64_t ultimo_uso;
} t_entrada_pagina;

t_dictionary *tablas_segmentos;//key:nombre_restaurante

void inicializar_memoria();
void inicializar_tablas_segmentos();
void inicializar_tabla_segmentos(char* nombre_restaurante);
void inicializar_segmento_pedido(char* nombre_restaurante, int id_pedido);
t_segmento* inicializar_segmento(int id_pedido);
void agregar_pagina_a_segmento(char* nombre_restaurante, int id_pedido, char* nombre_comida);
t_entrada_pagina* inicializar_entrada_pagina(char* nombre_comida);
void liberar_tablas();
void liberar_memoria();

#endif /* AUXILIARES_MMU_H_ */
