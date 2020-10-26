/*
 * MMU.c
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */


#include "MMU.h"

void inicializar_memoria(){
	setear_params_global_mem_principal();
	tablas_segmentos = dictionary_create();
	inicializar_memoria_principal();
}

void inicializar_tablas_segmentos(){
	tablas_segmentos = dictionary_create();
}

t_list* inicializar_tabla_segmentos(char* nombre_restaurante){
	t_list* tabla_segmentos = list_create();
	dictionary_put(tablas_segmentos, nombre_restaurante, tabla_segmentos);
	return tabla_segmentos;
}

t_segmento* inicializar_segmento_pedido(t_list* tabla_segmentos,int id_pedido){
	t_segmento* segmento = inicializar_segmento(id_pedido);
	list_add(tabla_segmentos, segmento);
	return segmento;
}

t_segmento* inicializar_segmento(int id_pedido){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	segmento->id_pedido = id_pedido;
	segmento->tabla_paginas = list_create();

	return segmento;
}

int existe_tabla_de_segmentos_de_restaurante(char* restaurante){
	return dictionary_has_key(tablas_segmentos,restaurante);
}
int existe_pedido_en_restaurante(char* nombre_restaurante,int id_pedido){
	t_list* tabla_segmentos = dictionary_get(tablas_segmentos, nombre_restaurante);

	int existe_pedido(t_segmento* segmento){
		return segmento->id_pedido == id_pedido;
	}

	return list_any_satisfy(tabla_segmentos,(void*)existe_pedido);
}
int crear_tabla_de_paginas_de_pedido(t_segmento* segmento_pedido){
	segmento_pedido->tabla_paginas = list_create();
	return segmento_pedido->tabla_paginas != NULL;
}

uint32_t procesar_guardar_pedido(t_guardar_pedido* info_guardar_pedido){
	uint32_t ret = false;
	char* nombre_restaurante = &info_guardar_pedido->restaurante[0];
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(nombre_restaurante)){
		/*SI NO EXISTE, SE CREA TABLA DE SEGMENTOS */
		t_list* tabla_segmentos = inicializar_tabla_segmentos(nombre_restaurante);
		/*SE CREA SEGMENTO*/
		t_segmento* segmento_pedido = inicializar_segmento_pedido(tabla_segmentos,info_guardar_pedido->id_pedido);
		/*SE CREA TABLA DE PAGINAS*/
		ret = crear_tabla_de_paginas_de_pedido(segmento_pedido);
	}
	else{
		/*SI EXISTE, SE INFORMA */
		log_info(logger,"La tabla de segmentos del restaurante:%s ya existe por lo que no fue creada",info_guardar_pedido->restaurante);
		/*SE VALIDA SI EL PEDIDO YA TENIA UN SEGMENTO ASOCIADO*/
		if(!existe_pedido_en_restaurante(nombre_restaurante, info_guardar_pedido->id_pedido)){
			/*SE BUSCA LA TABLA DE SEGMENTO*/
			t_list* tabla_segmentos = dictionary_get(tablas_segmentos,nombre_restaurante);
			/*SE CREA SEGMENTO*/
			t_segmento* segmento = inicializar_segmento_pedido(tabla_segmentos,info_guardar_pedido->id_pedido);
			/*SE CREA TABLA DE PAGINAS*/
			ret = crear_tabla_de_paginas_de_pedido(segmento);
		}
		else
			log_info(logger,"El pedido:%d ya existia en la tabla de segmentos del restaurante:%s",info_guardar_pedido->id_pedido,nombre_restaurante);
	}
	return ret;
}
t_segmento* obtener_segmento_del_pedido(uint32_t id_pedido, char nombre_restaurante[L_ID]){
	t_list* tabla_segmentos = dictionary_get(tablas_segmentos, nombre_restaurante);

	int existe_pedido_en_segmento(t_segmento* segmento){
		return segmento->id_pedido == id_pedido;
	}
	return list_find(tabla_segmentos,(void*)existe_pedido_en_segmento);
}
t_entrada_pagina* buscar_plato_en_memoria(t_list* lista_paginas_mem,char* plato){
	t_entrada_pagina* pagina_encontrada;

	int plato_buscado(t_entrada_pagina* entrada_pagina){
		t_pagina* pagina = memoria_fisica + entrada_pagina->nro_frame_mp*sizeof(t_pagina);
		return strcmp(pagina->nombre_comida,plato) == 0;
	}
	pagina_encontrada = list_find(lista_paginas_mem,(void*)plato_buscado);
	return pagina_encontrada;
}
t_entrada_pagina* obtener_pagina_de_plato(t_list* lista_paginas,char* plato){
	t_entrada_pagina* entrada_pagina;

	/*SE RECORRE LAS PAGINAS QUE ESTAN EN MP*/
	int obtener_pag_en_mem(t_entrada_pagina* entrada_pag){
		return entrada_pag->presencia;
	}
	int obtener_pag_en_swap(t_entrada_pagina* entrada_pag){
		return !entrada_pag->presencia;
	}
	t_list* lista_pag_mem = list_filter(lista_paginas,(void*)obtener_pag_en_mem);
	entrada_pagina = buscar_plato_en_memoria(lista_pag_mem,plato);
	list_destroy(lista_pag_mem);
	/*SI NO ESTÁ EN MP, LUEGO SE RECORREN LAS PAGINAS QUE ESTAN EN SWAP*/
	if(entrada_pagina == NULL){
		t_list* lista_pag_swap = list_filter(lista_paginas,(void*)obtener_pag_en_swap);
		/*TODO: EN SWAP AGREGAR FUNCIONES DE ABAJO*/
//		u_int32_t nro_frame_ms = buscar_plato_en_SWAP(lista_paginas,plato);
//		entrada_pagina = cargar_pagina_en_memoria(nro_frame_ms);
		list_destroy(lista_pag_swap);
	}
	return entrada_pagina;
}
uint32_t procesar_guardar_plato(t_guardar_plato* info_guardar_plato){
	char* nombre_restaurante = &info_guardar_plato->restaurante[0];
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(nombre_restaurante)){
		log_info(logger,"La tabla de segmentos del restaurante:%s no existe.",info_guardar_plato->restaurante);
		return false;
	}
	else{
		t_segmento* segmento = obtener_segmento_del_pedido(info_guardar_plato->id_pedido,nombre_restaurante);
		/*SE VALIDA SI EXISTE EL SEGMENTO PARA EL PEDIDO DEL RESTAURANTE*/
		if(segmento!=NULL){
			t_entrada_pagina* entrada_pagina = obtener_pagina_de_plato(segmento->tabla_paginas, info_guardar_plato->plato);
			/*SE VALIDA SI EL PLATO YA TIENE ASIGNADO UN FRAME*/
			if(entrada_pagina != NULL){
				t_pagina* pagina = memoria_fisica + entrada_pagina->nro_frame_mp*sizeof(t_pagina);
				pagina->cant_total += info_guardar_plato->cantPlato;
			}
			else{
				/*TODO:Asignar nueva pagina, nro_frame y grabar en memoria_fisica*/
//				t_entrada_pagina* pagina = malloc(sizeof(t_entrada_pagina));
			}
		}
		else{
			log_info(logger,"El segmento del pedido:%d para el restaurante:%s no existe.",info_guardar_plato->id_pedido,info_guardar_plato->restaurante);
			return false;
		}
	}
	return true;
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
