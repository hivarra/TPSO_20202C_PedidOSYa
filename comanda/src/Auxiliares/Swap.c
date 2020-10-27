/*
 * Swap.c
 *
 *  Created on: 26 oct. 2020
 *      Author: utnso
 */
#include "Swap.h"

void inicializar_memoria_swap(){
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
	init_bitmap_ms();
	if (mem_principal_global.algoritmo_reemplazo == CLOCK_MEJ)
		puntero_clock = 0;
}

void volcar_pagina_a_swap(t_entrada_pagina* entrada_pagina){
	entrada_pagina->presencia = 0;

	t_pagina* pagina_mp = memoria_fisica+entrada_pagina->nro_frame_mp*sizeof(t_pagina);

	if (entrada_pagina->modificado){
		memoria_swap = fopen(path_memoria_swap, "r+");
		fseek(memoria_swap, entrada_pagina->nro_frame_ms*sizeof(t_pagina), SEEK_SET);//Pongo el puntero al archivo sobre el comienzo de la pagina
		fwrite(pagina_mp, sizeof(t_pagina), 1, memoria_swap);
		fclose(memoria_swap);
		entrada_pagina->modificado = 0;
	}
}

void traer_pagina_de_swap(uint32_t frame_mp, t_entrada_pagina* entrada_pagina){

	fseek(memoria_swap, entrada_pagina->nro_frame_ms*sizeof(t_pagina), SEEK_SET);//Pongo el puntero al archivo sobre el comienzo de la pagina
	fread(memoria_fisica+frame_mp*sizeof(t_pagina), sizeof(t_pagina), 1, memoria_swap);//Copio el frame desde swap a la memoria fisica
	entrada_pagina->nro_frame_mp = frame_mp;
	entrada_pagina->presencia = 1;
}

t_entrada_pagina* entrada_uso0_modificado0(){
	t_entrada_pagina* entrada_buscada = NULL;

	for(int i = 0; i < list_size(lista_entradas_paginas); i++){
		t_entrada_pagina* entrada_i = list_get(lista_entradas_paginas, 0);
		if (!entrada_i->uso && !entrada_i->modificado){
			puntero_clock = i;
			entrada_buscada = entrada_i;
			break;
		}
	}
	return entrada_buscada;
}

t_entrada_pagina* entrada_uso0_modificado1(){
	t_entrada_pagina* entrada_buscada = NULL;

	for(int i = 0; i < list_size(lista_entradas_paginas); i++){
		t_entrada_pagina* entrada_i = list_get(lista_entradas_paginas, 0);
		if (!entrada_i->uso && entrada_i->modificado){
			puntero_clock = i;
			entrada_buscada = entrada_i;
			break;
		}
		else
			entrada_i->uso = 0;
	}
	return entrada_buscada;
}

void reemplazo_de_pagina(t_entrada_pagina* entrada_pagina){
	t_entrada_pagina* entrada_pagina_buscada;

	if (mem_principal_global.algoritmo_reemplazo == LRU){
		uint64_t oldLRU = UINT64_MAX;
		for (int i= 0; i < sizeof(lista_entradas_paginas); i++){
			t_entrada_pagina* entrada_i = list_get(lista_entradas_paginas, i);

			if (entrada_i->ultimo_uso < oldLRU){
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
	volcar_pagina_a_swap(entrada_pagina_buscada);
	traer_pagina_de_swap(entrada_pagina_buscada->nro_frame_mp, entrada_pagina);
}

t_entrada_pagina* buscar_plato_en_swap(t_list* lista_paginas_swap, char* plato){

	t_entrada_pagina* entrada_pag_buscada = NULL;

	bool pagina_swap_contiene_plato(t_entrada_pagina* entrada_pag){
		reemplazo_de_pagina(entrada_pag);
		t_pagina* pag = memoria_fisica+entrada_pag->nro_frame_mp*sizeof(t_pagina);
		return (strcmp(pag->nombre_comida, plato)==0);
	}

	entrada_pag_buscada = list_find(lista_paginas_swap, (void*)pagina_swap_contiene_plato);

	return entrada_pag_buscada;
}


