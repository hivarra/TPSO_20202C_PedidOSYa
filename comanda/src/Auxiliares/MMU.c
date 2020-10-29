/*
 * MMU.c
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */


#include "MMU.h"

t_list* inicializar_tabla_segmentos(char* nombre_restaurante){
	t_list* tabla_segmentos = list_create();
	if (tabla_segmentos != NULL)
		dictionary_put(tablas_segmentos, nombre_restaurante, tabla_segmentos);
	return tabla_segmentos;
}

t_segmento* inicializar_segmento_pedido(t_list* tabla_segmentos,uint32_t id_pedido){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	if (segmento != NULL){
		segmento->id_pedido = id_pedido;
		segmento->estado_pedido = PENDIENTE;
		segmento->tabla_paginas = list_create();
		if (segmento->tabla_paginas != NULL)
			list_add(tabla_segmentos, segmento);
		else{
			free(segmento);
			return NULL;
		}
	}
	return segmento;
}

t_entrada_pagina* inicializar_entrada_pagina(){
	t_entrada_pagina* entrada_pagina = malloc(sizeof(t_entrada_pagina));
	entrada_pagina->ultimo_uso = 0;
	entrada_pagina->uso = 0;
	entrada_pagina->modificado = 0;
	entrada_pagina->presencia = 0;
	entrada_pagina->nro_frame_mp = 0;//NO TIENE IMPORTANCIA MIENTRAS PRESENCIA = 0
	entrada_pagina->nro_frame_ms = 0;//DEBE ACTUALIZARSE

	return entrada_pagina;
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

t_segmento* obtener_segmento_del_pedido(uint32_t id_pedido, char* nombre_restaurante){
	t_list* tabla_segmentos = dictionary_get(tablas_segmentos, nombre_restaurante);

	int existe_pedido_en_segmento(t_segmento* segmento){
		return segmento->id_pedido == id_pedido;
	}
	return list_find(tabla_segmentos,(void*)existe_pedido_en_segmento);
}

t_entrada_pagina* buscar_plato_en_memoria(t_list* lista_paginas_mem,char* plato){
	t_entrada_pagina* pagina_encontrada = NULL;

	int plato_buscado(t_entrada_pagina* entrada_pagina){
		t_pagina* pagina = memoria_fisica + entrada_pagina->nro_frame_mp*sizeof(t_pagina);
		actualizar_bits_de_uso(entrada_pagina);
		return strcmp(pagina->nombre_comida,plato) == 0;
	}
	pagina_encontrada = list_find(lista_paginas_mem,(void*)plato_buscado);
	return pagina_encontrada;
}

t_entrada_pagina* obtener_pagina_de_plato(t_list* lista_paginas,char* plato){
	t_entrada_pagina* entrada_pagina = NULL;

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
		entrada_pagina = buscar_plato_en_swap(lista_pag_swap,plato);
		list_destroy(lista_pag_swap);
	}
	return entrada_pagina;
}

uint32_t procesar_guardar_pedido(t_guardar_pedido* info_guardar_pedido){
	uint32_t ret = false;
	//char* nombre_restaurante = &info_guardar_pedido->restaurante[0];
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(info_guardar_pedido->restaurante)){
		/*SI NO EXISTE, SE CREA TABLA DE SEGMENTOS */
		t_list* tabla_segmentos = inicializar_tabla_segmentos(info_guardar_pedido->restaurante);
		if (tabla_segmentos == NULL){
			log_warning(logger, "No se pudo crear la tabla de segmentos para el restaurante %s", info_guardar_pedido->restaurante);
			return false;
		}
		/*SE CREA SEGMENTO*/
		t_segmento* segmento_pedido = inicializar_segmento_pedido(tabla_segmentos,info_guardar_pedido->id_pedido);
		if (segmento_pedido != NULL)
			ret = true;
		else
			log_warning(logger, "No se pudo crear el segmento para el pedido %d del restaurante %s", info_guardar_pedido->id_pedido, info_guardar_pedido->restaurante);
	}
	else{
		/*SE VALIDA SI EL PEDIDO YA TENIA UN SEGMENTO ASOCIADO*/
		if(!existe_pedido_en_restaurante(info_guardar_pedido->restaurante, info_guardar_pedido->id_pedido)){
			/*SE BUSCA LA TABLA DE SEGMENTO*/
			t_list* tabla_segmentos = dictionary_get(tablas_segmentos,info_guardar_pedido->restaurante);
			/*SE CREA SEGMENTO*/
			t_segmento* segmento_pedido = inicializar_segmento_pedido(tabla_segmentos,info_guardar_pedido->id_pedido);
			if (segmento_pedido != NULL)
				ret = true;
			else
				log_warning(logger, "No se pudo crear el segmento para el pedido %d del restaurante %s", info_guardar_pedido->id_pedido, info_guardar_pedido->restaurante);
		}
		else
			log_warning(logger,"El pedido %d ya existia en la tabla de segmentos del restaurante %s",info_guardar_pedido->id_pedido,info_guardar_pedido->restaurante);
	}
	return ret;
}

uint32_t procesar_guardar_plato(t_guardar_plato* info_guardar_plato){
	uint32_t ret = false;
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(info_guardar_plato->restaurante))
		log_warning(logger,"La tabla de segmentos del restaurante %s no existe.",info_guardar_plato->restaurante);
	else{
		t_segmento* segmento = obtener_segmento_del_pedido(info_guardar_plato->id_pedido,info_guardar_plato->restaurante);
		/*SE VALIDA SI EXISTE EL SEGMENTO PARA EL PEDIDO DEL RESTAURANTE*/
		if(segmento!=NULL){
			t_entrada_pagina* entrada_pagina = obtener_pagina_de_plato(segmento->tabla_paginas, info_guardar_plato->plato);
			/*SE VALIDA SI EL PLATO YA TIENE ASIGNADO UN FRAME*/
			if(entrada_pagina != NULL){
				t_pagina* pagina = memoria_fisica + entrada_pagina->nro_frame_mp*sizeof(t_pagina);
				pagina->cant_total += info_guardar_plato->cantPlato;
				actualizar_bits_de_uso(entrada_pagina);
				entrada_pagina->modificado = 1;
				ret = true;
			}
			else{
				int free_frame_swap = get_free_frame_ms();
				if (free_frame_swap != -1){
					/*SI HAY LUGAR EN SWAP, CREO LA ENTRADA DE PAGINA*/
					t_entrada_pagina* entrada_nueva = inicializar_entrada_pagina();
					list_add(segmento->tabla_paginas, entrada_nueva);
					entrada_nueva->nro_frame_ms = free_frame_swap;
					int free_frame_mp = get_free_frame_mp();
					if (free_frame_mp != -1){
						entrada_nueva->nro_frame_mp = free_frame_mp;
						entrada_nueva->presencia = 1;
					}
					else
						reemplazo_de_pagina(entrada_nueva);//me actualiza el frame_mp y la presencia
					/*ESCRIBO LA PAGINA Y ACTUALIZO BITS*/
					t_pagina* pagina = memoria_fisica + entrada_nueva->nro_frame_mp*sizeof(t_pagina);
					strcpy(pagina->nombre_comida, info_guardar_plato->plato);
					pagina->cant_total = info_guardar_plato->cantPlato;
					pagina->cant_lista = 0;
					actualizar_bits_de_uso(entrada_nueva);
					entrada_nueva->modificado = 1;
					ret = true;
				}
				else
					log_warning(logger,"No hay lugar en SWAP para almacenar el plato %s del pedido %d del restaurante %s.", info_guardar_plato->plato,info_guardar_plato->id_pedido,info_guardar_plato->restaurante);
			}
		}
		else
			log_warning(logger,"El segmento del pedido %d para el restaurante %s no existe.",info_guardar_plato->id_pedido,info_guardar_plato->restaurante);
	}
	return ret;
}
