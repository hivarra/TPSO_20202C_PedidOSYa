/*
 * MMU.c
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */


#include "MMU.h"

void inicializar_memoria(){
	setear_params_global_mem_principal();
	inicializar_memoria_principal();
}

void inicializar_tablas_segmentos(){
	tablas_segmentos = dictionary_create();
}

void inicializar_tabla_segmentos(char* nombre_restaurante){
	t_list* tabla_segmentos = list_create();
	dictionary_put(tablas_segmentos, nombre_restaurante, tabla_segmentos);
}

void inicializar_segmento_pedido(char* nombre_restaurante, int id_pedido){
	t_list* tabla_segmentos = dictionary_get(tablas_segmentos, nombre_restaurante);
	t_segmento* segmento = inicializar_segmento(id_pedido);
	list_add(tabla_segmentos, segmento);
}

t_segmento* inicializar_segmento(int id_pedido){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->id_pedido = id_pedido;
	segmento->nro_pagina = 0;
	segmento->nro_segmento = 0;
	segmento->tabla_paginas = list_create();

	return segmento;
}

void agregar_pagina_a_segmento(char* nombre_restaurante, int id_pedido, char* nombre_comida){
	t_list* tabla_segmentos = dictionary_get(tablas_segmentos, nombre_restaurante);
	int tiene_id_pedido(t_segmento* segmento){
		return segmento->id_pedido = id_pedido;
	}
	t_segmento* segmento = list_find(tabla_segmentos, (void*)tiene_id_pedido);
	t_entrada_pagina* entrada_pagina = inicializar_entrada_pagina(nombre_comida);
	list_add(segmento->tabla_paginas, entrada_pagina);
}

t_entrada_pagina* inicializar_entrada_pagina(char* nombre_comida){
	t_entrada_pagina* entrada_pagina = malloc(sizeof(t_entrada_pagina));
	entrada_pagina->nro_pagina = 0;
	entrada_pagina->nro_frame = 0;
	strcpy(entrada_pagina->plato, nombre_comida);
	entrada_pagina->ultimo_uso = 0;
	entrada_pagina->uso = 0;
	entrada_pagina->modificado = 0;

	return entrada_pagina;
}

void liberar_tablas(){
	/*TODO:*/
}

void liberar_memoria(){
	liberar_memoria_principal();
	liberar_tablas();
}
