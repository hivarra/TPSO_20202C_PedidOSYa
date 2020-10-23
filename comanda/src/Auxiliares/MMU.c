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

void inicializar_tabla_segmentos(char nombre_restaurante[L_ID]){
	t_list* tabla_segmentos = list_create();
	dictionary_put(tablas_segmentos, nombre_restaurante, tabla_segmentos);
}

void inicializar_segmento_pedido(char nombre_restaurante[L_ID], int id_pedido){
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

int existe_tabla_de_segmentos_de_restaurante(char restaurante[L_ID]){
	return dictionary_has_key(tablas_segmentos,restaurante);
}
uint32_t procesar_guardar_pedido(t_guardar_pedido* info_guardar_pedido){
	uint32_t ret = false;
	char nombre_restaurante[L_ID];
	strcpy(nombre_restaurante,info_guardar_pedido->restaurante);
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	/*SI NO EXISTE, SE CREA Y GUARDO EL PEDIDO */
	/*SI EXISTE, SE INFORMA */
	if(!existe_tabla_de_segmentos_de_restaurante(nombre_restaurante)){
		inicializar_tabla_segmentos(nombre_restaurante);
		inicializar_segmento_pedido(nombre_restaurante,info_guardar_pedido->id_pedido);
		ret = true;
	}
	else{
		log_info(logger,"La tabla de segmentos del restaurante:% ya existe.",info_guardar_pedido->restaurante);
		ret = false;
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
t_entrada_pagina* obtener_entrada_pagina(t_segmento* segmento, char plato[L_PLATO_MEM]){
	/*TODO: RECORRER LAS PAGINA DEL SEGMENTO Y LEER DE memoria_fisica CON EL nro_frame de la entrada_pagina*/
}
uint32_t procesar_guardar_plato(t_guardar_plato* info_guardar_plato){
	char nombre_restaurante[L_ID];
	strcpy(nombre_restaurante,info_guardar_plato->restaurante);
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(nombre_restaurante)){
		log_info(logger,"La tabla de segmentos del restaurante:%s no existe.",info_guardar_plato->restaurante);
		return false;
	}
	else{
		t_segmento* segmento = obtener_segmento_del_pedido(info_guardar_plato->id_pedido,nombre_restaurante);
		/*SE VALIDA SI EXISTE EL SEGMENTO PARA EL PEDIDO DEL RESTAURANTE*/
		if(segmento!=NULL){
			t_entrada_pagina* entrada_pagina = obtener_entrada_pagina(segmento,info_guardar_plato->plato);
			/*SE VALIDA SI EXISTE EL PLATO PARA EL PEDIDO DEL RESTAURANTE*/
			if(entrada_pagina != NULL){
				/*TODO:Agregar cantidades al plato*/
			}
			else{
				/*TODO:Asignar nueva pagina, nro_frame y grabar en memoria_fisica*/
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
	entrada_pagina->nro_pagina = 0;
	entrada_pagina->nro_frame = 0;
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
