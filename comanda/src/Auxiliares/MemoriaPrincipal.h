/*
 * memoria_principal.h
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_MEMORIAPRINCIPAL_H_
#define AUXILIARES_MEMORIAPRINCIPAL_H_

#include "Config_y_log.h"

typedef enum{
	LRU,
	CLOCK_MEJ
}t_algoritmo_reemplazo;

typedef struct{
	int tamanio_memoria;
	int tamanio_swap;
	t_algoritmo_reemplazo algoritmo_reemplazo;
}t_params_global_mem_principal;

typedef struct{
	char nombre_comida[24];
	uint32_t cant_total;
	uint32_t cant_lista;
}__attribute__((packed)) t_pagina;

typedef struct{
	uint32_t id_pedido;
	uint32_t estado_pedido;
	uint32_t cant_platos_listos;
	t_list* tabla_paginas;
}t_segmento;

typedef struct{
	uint32_t nro_frame_mp;
	uint32_t nro_frame_ms;
	bool uso;
	bool modificado;
	bool presencia;
	uint64_t ultimo_uso;
}t_entrada_pagina;

void* memoria_fisica;
t_params_global_mem_principal mem_principal_global;
t_list* lista_entradas_paginas;
t_dictionary *tablas_segmentos;//key:nombre_restaurante
t_dictionary *semaforos_pedidos;//key:restaurante_idPedido

pthread_mutex_t mutex_lista_global;
pthread_mutex_t mutex_tablas;
pthread_mutex_t mutexSemaforosPedidos;

char* path_memoria_swap;

void inicializar_memoria(void);
void liberar_memoria(void);
void liberar_memoria_swap(void);

#endif /* AUXILIARES_MEMORIAPRINCIPAL_H_ */
