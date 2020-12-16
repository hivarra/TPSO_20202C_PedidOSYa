/*
 * Swap.c
 *
 *  Created on: 26 oct. 2020
 *      Author: utnso
 */
#include "Swap.h"

void inicializar_memoria_swap(){
	FILE* memoria_swap;
	/*CREO LA CARPETA PARA EL SWAP*/
	path_memoria_swap = getParentPath();
	string_append(&path_memoria_swap, "/Swap");
	int result_mkdir = mkdir(path_memoria_swap, 0777);
	/*CREO EL SWAP*/
	string_append(&path_memoria_swap, "/Comanda_SwapMemory.swp");
	if (result_mkdir==0 || result_mkdir==-1){//para que mkdir se trabe y espere a que la carpeta se cree
		memoria_swap = fopen(path_memoria_swap, "w+");
		truncate(path_memoria_swap, mem_principal_global.tamanio_swap);
		fclose(memoria_swap);
	}
	puntero_clock = 0;
	pthread_mutex_init(&mutex_swap, NULL);
	pthread_mutex_init(&mutex_reemplazo, NULL);
}

void volcar_pagina_a_swap(t_entrada_pagina* entrada_pagina, t_pagina* pagina){
	FILE* memoria_swap;
	entrada_pagina->presencia = 0;

	if (entrada_pagina->modificado){
		memoria_swap = fopen(path_memoria_swap, "r+");
		fseek(memoria_swap, entrada_pagina->nro_frame_ms*sizeof(t_pagina), SEEK_SET);//Pongo el puntero al archivo sobre el comienzo de la pagina
		pthread_mutex_lock(&mutex_swap);
		fwrite(pagina, sizeof(t_pagina), 1, memoria_swap);
		fclose(memoria_swap);
		pthread_mutex_unlock(&mutex_swap);
		entrada_pagina->modificado = 0;
	}
}

void traer_pagina_de_swap(uint32_t frame_mp, t_entrada_pagina* entrada_pagina){
	FILE* memoria_swap;
	memoria_swap = fopen(path_memoria_swap, "r");
	fseek(memoria_swap, entrada_pagina->nro_frame_ms*sizeof(t_pagina), SEEK_SET);//Pongo el puntero al archivo sobre el comienzo de la pagina
	pthread_mutex_lock(&mutex_swap);
	fread(memoria_fisica+frame_mp*sizeof(t_pagina), sizeof(t_pagina), 1, memoria_swap);//Copio el frame desde swap a la memoria fisica
	fclose(memoria_swap);
	pthread_mutex_unlock(&mutex_swap);
	entrada_pagina->nro_frame_mp = frame_mp;
	entrada_pagina->presencia = 1;
}

t_entrada_pagina* entrada_uso0_modificado0(){
	t_entrada_pagina* entrada_buscada = NULL;

	for(int i = 0; i < list_size(lista_entradas_paginas); i++){
		t_entrada_pagina* entrada_i = list_get(lista_entradas_paginas, i);
		if (!entrada_i->uso && !entrada_i->modificado && entrada_i->presencia){
			entrada_buscada = entrada_i;
			if(i < list_size(lista_entradas_paginas)-1)
				puntero_clock = i+1;
			else
				puntero_clock = 0;//Si es la ultima entrada, pone el puntero al principio
			break;
		}
	}
	return entrada_buscada;
}

t_entrada_pagina* entrada_uso0_modificado1(){
	t_entrada_pagina* entrada_buscada = NULL;

	for(int i = 0; i < list_size(lista_entradas_paginas); i++){
		t_entrada_pagina* entrada_i = list_get(lista_entradas_paginas, i);
		if (entrada_i->presencia){
			if (!entrada_i->uso && entrada_i->modificado){
				entrada_buscada = entrada_i;
				if(i < list_size(lista_entradas_paginas)-1)
					puntero_clock = i+1;
				else
					puntero_clock = 0;//Si es la ultima entrada, pone el puntero al principio
				break;
			}
			else
				entrada_i->uso = 0;
		}
	}
	return entrada_buscada;
}

void reemplazo_de_pagina(t_entrada_pagina* entrada_pagina){
	pthread_mutex_lock(&mutex_reemplazo);
	t_entrada_pagina* entrada_pagina_buscada;

	log_info(logger, "[REEMPLAZO] Comienzo de reemplazo de pagina. Posicion de Swap a traer: %d.", entrada_pagina->nro_frame_ms);

	int frame_libre_mp = get_free_frame_mp();
	if (frame_libre_mp == -1){

		if (mem_principal_global.algoritmo_reemplazo == LRU){
			uint64_t oldLRU = UINT64_MAX;
			for (int i = 0; i < list_size(lista_entradas_paginas); i++){
				t_entrada_pagina* entrada_i = list_get(lista_entradas_paginas, i);

				if (entrada_i->ultimo_uso < oldLRU && entrada_i->presencia){
					entrada_pagina_buscada = entrada_i;
					oldLRU = entrada_i->ultimo_uso;
				}
			}
		}
		else{/*CLOCK_MEJ*/
			t_list* l_auxiliar = list_take_and_remove(lista_entradas_paginas, puntero_clock);
			list_add_all(lista_entradas_paginas, l_auxiliar);
			list_destroy(l_auxiliar);
			puntero_clock = 0;
			t_entrada_pagina* entrada_clock = entrada_uso0_modificado0();//Recorre los marcos y selecciona el primero con (u=0;m=0)
			if(entrada_clock != NULL)
				entrada_pagina_buscada = entrada_clock;
			else{
				entrada_clock = entrada_uso0_modificado1();//Selecciona el 1ª con (u=0;m=1). Mientras recorre modifica el bit uso de 1 a 0
				if(entrada_clock != NULL)
					entrada_pagina_buscada = entrada_clock;
				else{
					entrada_clock = entrada_uso0_modificado0();
					if(entrada_clock != NULL)
						entrada_pagina_buscada = entrada_clock;
					else
						entrada_pagina_buscada = entrada_uso0_modificado1();//ACA LO ENCUENTRA SI O SI
				}
			}
		}
		t_pagina* pagina_reemplazar = memoria_fisica+entrada_pagina_buscada->nro_frame_mp*sizeof(t_pagina);
		volcar_pagina_a_swap(entrada_pagina_buscada, pagina_reemplazar);
		log_info(logger, "[REEMPLAZO] Frame seleccionado: %d (Plato: %s). Posicion de Swap a traer: %d.", entrada_pagina_buscada->nro_frame_mp, pagina_reemplazar, entrada_pagina->nro_frame_ms);
		traer_pagina_de_swap(entrada_pagina_buscada->nro_frame_mp, entrada_pagina);
	}
	else{
		log_info(logger, "[REEMPLAZO] Frame seleccionado: %d (LIBRE). Posicion de Swap a traer: %d.", frame_libre_mp, entrada_pagina->nro_frame_ms);
		traer_pagina_de_swap(frame_libre_mp, entrada_pagina);
	}
	pthread_mutex_unlock(&mutex_reemplazo);
}

//t_entrada_pagina* buscar_plato_en_swap(t_list* lista_paginas_swap, char* plato){
//
//	t_entrada_pagina* entrada_pag_buscada = NULL;
//
//	bool pagina_swap_contiene_plato(t_entrada_pagina* entrada_pag){
//		reemplazo_de_pagina(entrada_pag);
//		t_pagina* pag = memoria_fisica+entrada_pag->nro_frame_mp*sizeof(t_pagina);
//		actualizar_bits_de_uso(entrada_pag);
//		return (strcmp(pag->nombre_comida, plato) == 0);
//	}
//
//	entrada_pag_buscada = list_find(lista_paginas_swap, (void*)pagina_swap_contiene_plato);
//
//	return entrada_pag_buscada;
//}


