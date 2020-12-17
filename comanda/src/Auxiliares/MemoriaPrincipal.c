/*
 * MemoriaPrincipal.c
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */


#include "MemoriaPrincipal.h"

void setear_params_global_mem_principal(void);
void inicializar_memoria_principal(void);

void inicializar_memoria(){
	setear_params_global_mem_principal();
	inicializar_memoria_principal();
	tablas_segmentos = dictionary_create();
	semaforos_pedidos = dictionary_create();
	lista_entradas_paginas = list_create();
	pthread_mutex_init(&mutex_tablas, NULL);
	pthread_mutex_init(&mutex_lista_global, NULL);
	pthread_mutex_init(&mutexSemaforosPedidos, NULL);
}

void inicializar_memoria_principal(){
	memoria_fisica = calloc(1,mem_principal_global.tamanio_memoria);//Inicializa con /0
}

void setear_params_global_mem_principal(){
	mem_principal_global.tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	mem_principal_global.tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");

	if (string_equals_ignore_case(config_get_string_value(config, "ALGORITMO_REEMPLAZO"), "LRU"))
		mem_principal_global.algoritmo_reemplazo = LRU;
	else
		mem_principal_global.algoritmo_reemplazo = CLOCK_MEJ;
}

void _destruir_segmento(t_segmento* segmento){
	list_destroy_and_destroy_elements(segmento->tabla_paginas, free);
	free(segmento);
}

void _destruir_tabla_segmentos(t_list* tabla_segmentos){
	list_destroy_and_destroy_elements(tabla_segmentos, (void*)_destruir_segmento);
}

void liberar_memoria(){
	free(memoria_fisica);
	dictionary_destroy_and_destroy_elements(tablas_segmentos, (void*)_destruir_tabla_segmentos);
	list_destroy(lista_entradas_paginas);
}

void liberar_memoria_swap(){
	remove(path_memoria_swap);
	free(path_memoria_swap);
	char* directorio_swap = getParentPath();
	string_append(&directorio_swap, "/Swap");
	rmdir(directorio_swap);
	free(directorio_swap);
}
