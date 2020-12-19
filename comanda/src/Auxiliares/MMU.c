/*
 * MMU.c
 *
 *  Created on: 18 sep. 2020
 *      Author: utnso
 */


#include "MMU.h"

t_list* inicializar_tabla_segmentos(char* nombre_restaurante){
	t_list* tabla_segmentos = list_create();

	pthread_mutex_lock(&mutex_tablas);
	if (tabla_segmentos != NULL)
		dictionary_put(tablas_segmentos, nombre_restaurante, tabla_segmentos);
	pthread_mutex_unlock(&mutex_tablas);

	return tabla_segmentos;
}

t_segmento* inicializar_segmento_pedido(t_list* tabla_segmentos,uint32_t id_pedido){
	t_segmento* segmento = malloc(sizeof(t_segmento));
	if (segmento != NULL){
		segmento->id_pedido = id_pedido;
		segmento->estado_pedido = PENDIENTE;
		segmento->cant_platos_listos = 0;
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
	pthread_mutex_t* semRest = mutex_restaurante(nombre_restaurante);
	t_list* tabla_segmentos = dictionary_get(tablas_segmentos, nombre_restaurante);
	pthread_mutex_unlock(semRest);

	bool existe_pedido_en_segmento(t_segmento* segmento){
		return segmento->id_pedido == id_pedido;
	}
	return list_find(tabla_segmentos,(void*)existe_pedido_en_segmento);
}

void borrar_segmento_del_pedido(uint32_t id_pedido, char* nombre_restaurante){
	pthread_mutex_t* semRest = mutex_restaurante(nombre_restaurante);
	t_list* tabla_segmentos = dictionary_get(tablas_segmentos, nombre_restaurante);

	bool existe_pedido_en_segmento(t_segmento* segmento){
		return segmento->id_pedido == id_pedido;
	}

	list_remove_and_destroy_by_condition(tabla_segmentos, (void*)existe_pedido_en_segmento, free);
	pthread_mutex_unlock(semRest);

	pthread_mutex_lock(&mutex_tablas);
	if (list_size(tabla_segmentos) == 0)
		dictionary_remove_and_destroy(tablas_segmentos, nombre_restaurante, (void*)list_destroy);
	pthread_mutex_unlock(&mutex_tablas);
}

void borrar_entrada_en_lista_global(t_entrada_pagina* entrada_pag){
	bool _buscar_por_frame_ms(t_entrada_pagina* entrada){
		return entrada->nro_frame_ms == entrada_pag->nro_frame_ms;
	}

	pthread_mutex_lock(&mutex_lista_global);
	list_remove_by_condition(lista_entradas_paginas, (void*)_buscar_por_frame_ms);
	pthread_mutex_unlock(&mutex_lista_global);
}

//t_entrada_pagina* buscar_plato_en_memoria(t_list* lista_paginas_mem,char* plato){
//	t_entrada_pagina* pagina_encontrada = NULL;
//
//	bool plato_buscado(t_entrada_pagina* entrada_pagina){
//		t_pagina* pagina = memoria_fisica + entrada_pagina->nro_frame_mp*sizeof(t_pagina);
//		actualizar_bits_de_uso(entrada_pagina);
//		return (strcmp(pagina->nombre_comida,plato) == 0);
//	}
//	pagina_encontrada = list_find(lista_paginas_mem,(void*)plato_buscado);
//	return pagina_encontrada;
//}

//t_entrada_pagina* obtener_pagina_de_plato(t_list* lista_paginas,char* plato){//TODO:PREGUNTAR SI LA SECUENCIA ES CORRECTA O CORRESPONDE HACERLO SECUENCIAL
//	t_entrada_pagina* entrada_pagina = NULL;
//
//	/*SE RECORRE LAS PAGINAS QUE ESTAN EN MP*/
//	bool obtener_pag_en_mem(t_entrada_pagina* entrada_pag){
//		return entrada_pag->presencia;
//	}
//	bool obtener_pag_en_swap(t_entrada_pagina* entrada_pag){
//		return !entrada_pag->presencia;
//	}
//	t_list* lista_pag_mem = list_filter(lista_paginas,(void*)obtener_pag_en_mem);
//	entrada_pagina = buscar_plato_en_memoria(lista_pag_mem,plato);
//	list_destroy(lista_pag_mem);
//	/*SI NO ESTÁ EN MP, LUEGO SE RECORREN LAS PAGINAS QUE ESTAN EN SWAP*/
//	if(entrada_pagina == NULL){
//		t_list* lista_pag_swap = list_filter(lista_paginas,(void*)obtener_pag_en_swap);
//		entrada_pagina = buscar_plato_en_swap(lista_pag_swap,plato);
//		list_destroy(lista_pag_swap);
//	}
//	return entrada_pagina;
//}

t_entrada_pagina* obtener_pagina_de_plato(t_list* lista_paginas,char* plato){//MODO SECUENCIAL
	t_entrada_pagina* entrada_buscada = NULL;

	for (int i = 0; i < list_size(lista_paginas); i++){
		t_entrada_pagina* entrada_i = list_get(lista_paginas, i);
		if (!entrada_i->presencia)
			reemplazo_de_pagina(entrada_i);

		t_pagina* pagina = memoria_fisica + entrada_i->nro_frame_mp*sizeof(t_pagina);
		actualizar_bits_de_uso(entrada_i);
		if (string_equals_ignore_case(pagina->nombre_comida,plato)){
			entrada_buscada = entrada_i;
			break;
		}
	}
	return entrada_buscada;
}

uint32_t procesar_guardar_pedido(t_guardar_pedido* info_guardar_pedido){
	uint32_t ret = false;
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(info_guardar_pedido->restaurante)){
		/*SI NO EXISTE, SE CREA TABLA DE SEGMENTOS */
		t_list* tabla_segmentos = inicializar_tabla_segmentos(info_guardar_pedido->restaurante);
		if (tabla_segmentos == NULL){
			log_warning(logger, "No se pudo crear la tabla de segmentos para el restaurante %s.", info_guardar_pedido->restaurante);
			return false;
		}
		/*SE CREA SEGMENTO*/
		pthread_mutex_t* semRest = mutex_restaurante(info_guardar_pedido->restaurante);
		pthread_mutex_t* semPed = mutex_pedido(info_guardar_pedido->restaurante, info_guardar_pedido->id_pedido);
		t_segmento* segmento_pedido = inicializar_segmento_pedido(tabla_segmentos,info_guardar_pedido->id_pedido);
		pthread_mutex_unlock(semPed);
		pthread_mutex_unlock(semRest);
		if (segmento_pedido != NULL)
			ret = true;
		else
			log_warning(logger, "No se pudo crear el segmento para el pedido %d del restaurante %s.", info_guardar_pedido->id_pedido, info_guardar_pedido->restaurante);
	}
	else{
		/*SE VALIDA SI EL PEDIDO YA TENIA UN SEGMENTO ASOCIADO*/
		if(!existe_pedido_en_restaurante(info_guardar_pedido->restaurante, info_guardar_pedido->id_pedido)){
			/*SE BUSCA LA TABLA DE SEGMENTO*/
			t_list* tabla_segmentos = dictionary_get(tablas_segmentos,info_guardar_pedido->restaurante);
			/*SE CREA SEGMENTO*/
			pthread_mutex_t* semRest = mutex_restaurante(info_guardar_pedido->restaurante);
			pthread_mutex_t* semPed = mutex_pedido(info_guardar_pedido->restaurante, info_guardar_pedido->id_pedido);
			t_segmento* segmento_pedido = inicializar_segmento_pedido(tabla_segmentos,info_guardar_pedido->id_pedido);
			pthread_mutex_unlock(semPed);
			pthread_mutex_unlock(semRest);
			if (segmento_pedido != NULL)
				ret = true;
			else
				log_warning(logger, "No se pudo crear el segmento para el pedido %d del restaurante %s.", info_guardar_pedido->id_pedido, info_guardar_pedido->restaurante);
		}
		else
			log_warning(logger,"El pedido %d ya existia en la tabla de segmentos del restaurante %s.",info_guardar_pedido->id_pedido,info_guardar_pedido->restaurante);
	}
	return ret;
}

uint32_t procesar_guardar_plato(t_guardar_plato* info_guardar_plato){
	uint32_t ret = false;
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(info_guardar_plato->restaurante))
		log_warning(logger,"La tabla de segmentos del restaurante %s no existe.",info_guardar_plato->restaurante);
	else{
		pthread_mutex_t* semPed = mutex_pedido(info_guardar_plato->restaurante, info_guardar_plato->id_pedido);
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

					pthread_mutex_lock(&mutex_lista_global);
					list_add(lista_entradas_paginas, entrada_nueva);
					pthread_mutex_unlock(&mutex_lista_global);

					entrada_nueva->nro_frame_ms = free_frame_swap;
					entrada_nueva->modificado = 1;

					/*ESCRIBO LA PAGINA EN SWAP*/
					t_pagina* pagina_nueva = calloc(1,sizeof(t_pagina));
					strcpy(pagina_nueva->nombre_comida, info_guardar_plato->plato);
					pagina_nueva->cant_total = info_guardar_plato->cantPlato;
					pagina_nueva->cant_lista = 0;
					volcar_pagina_a_swap(entrada_nueva, pagina_nueva);

					int free_frame_mp = get_free_frame_mp();
					if (free_frame_mp != -1){
						entrada_nueva->nro_frame_mp = free_frame_mp;
						entrada_nueva->presencia = 1;
						actualizar_bits_de_uso(entrada_nueva);
						memcpy(memoria_fisica+entrada_nueva->nro_frame_mp*sizeof(t_pagina),pagina_nueva,sizeof(t_pagina));
					}
					else
						reemplazo_de_pagina(entrada_nueva);

					log_info(logger, "[PLATO ALMACENADO] Se almaceno el plato %s en la posicion %d del area de Swap.", pagina_nueva->nombre_comida, free_frame_swap);
					log_info(logger, "[PLATO ALMACENADO] Se almaceno el plato %s en la posicion %d de MP.", pagina_nueva->nombre_comida, entrada_nueva->nro_frame_mp);

					free(pagina_nueva);
					ret = true;
				}
				else
					log_warning(logger,"No hay lugar en SWAP para almacenar el plato %s del pedido %d del restaurante %s.", info_guardar_plato->plato,info_guardar_plato->id_pedido,info_guardar_plato->restaurante);
			}
		}
		else
			log_warning(logger,"El segmento del pedido %d para el restaurante %s no existe.",info_guardar_plato->id_pedido,info_guardar_plato->restaurante);
		pthread_mutex_unlock(semPed);
	}
	return ret;
}

/***FUNCION PARA OBTENER UN T_COMIDA DESDE LA MEMORIA***/
t_comida* _obtener_plato_de_memoria(t_entrada_pagina* entrada_pag){
	t_comida* plato = calloc(1,sizeof(t_comida));

	if (!entrada_pag->presencia)
		reemplazo_de_pagina(entrada_pag);

	t_pagina* pag = memoria_fisica+entrada_pag->nro_frame_mp*sizeof(t_pagina);
	strcpy(plato->nombre,pag->nombre_comida);
	plato->cantTotal = pag->cant_total;
	plato->cantLista = pag->cant_lista;
	actualizar_bits_de_uso(entrada_pag);
	return plato;
}

t_rta_obtener_pedido* procesar_obtener_pedido(t_obtener_pedido* info_obtener_pedido){
	t_rta_obtener_pedido* respuesta = malloc(sizeof(t_rta_obtener_pedido));
	respuesta->estado = FAILED;
	respuesta->cantComidas = 0;
	respuesta->comidas = list_create();

	/***FUNCION PARA GUARDAR UN PLATO EN LA RESPUESTA***/
	void _guardar_plato_en_lista(t_entrada_pagina* entrada_pag){
		t_comida* plato = calloc(1,sizeof(t_comida));
		if (!entrada_pag->presencia)
			reemplazo_de_pagina(entrada_pag);
		t_pagina* pag = memoria_fisica+entrada_pag->nro_frame_mp*sizeof(t_pagina);
		strcpy(plato->nombre,pag->nombre_comida);
		plato->cantTotal = pag->cant_total;
		plato->cantLista = pag->cant_lista;
		actualizar_bits_de_uso(entrada_pag);
		list_add(respuesta->comidas,plato);
	}

	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(info_obtener_pedido->restaurante))
		log_warning(logger,"La tabla de segmentos del restaurante %s no existe.",info_obtener_pedido->restaurante);
	else{
		pthread_mutex_t* semPed = mutex_pedido(info_obtener_pedido->restaurante, info_obtener_pedido->id_pedido);
		t_segmento* segmento = obtener_segmento_del_pedido(info_obtener_pedido->id_pedido,info_obtener_pedido->restaurante);
		/*SE VALIDA SI EXISTE EL SEGMENTO PARA EL PEDIDO DEL RESTAURANTE*/
		if(segmento!=NULL){
			respuesta->estado = segmento->estado_pedido;
			respuesta->cantComidas = list_size(segmento->tabla_paginas);
			list_iterate(segmento->tabla_paginas, (void*)_guardar_plato_en_lista);
		}
		else
			log_warning(logger,"El segmento del pedido %d para el restaurante %s no existe.",info_obtener_pedido->id_pedido,info_obtener_pedido->restaurante);
		pthread_mutex_unlock(semPed);
	}
	return respuesta;
}

uint32_t procesar_confirmar_pedido(t_confirmar_pedido* info_confirmar_pedido){
	uint32_t ret = false;
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(info_confirmar_pedido->restaurante))
		log_warning(logger,"La tabla de segmentos del restaurante %s no existe.",info_confirmar_pedido->restaurante);
	else{
		pthread_mutex_t* semPed = mutex_pedido(info_confirmar_pedido->restaurante, info_confirmar_pedido->id_pedido);
		t_segmento* segmento = obtener_segmento_del_pedido(info_confirmar_pedido->id_pedido,info_confirmar_pedido->restaurante);
		/*SE VALIDA SI EXISTE EL SEGMENTO PARA EL PEDIDO DEL RESTAURANTE*/
		if(segmento!=NULL){
			if(segmento->estado_pedido != PENDIENTE)/*VALIDO SI EL ESTADO ES PENDIENTE*/
				log_warning(logger,"El pedido %d para el restaurante %s no se encuentra pendiente.",info_confirmar_pedido->id_pedido,info_confirmar_pedido->restaurante);
			else if(list_size(segmento->tabla_paginas) < 1)/*VALIDO SI TIENE PLATOS PARA CONFIRMAR*/
				log_warning(logger,"El pedido %d para el restaurante %s no tiene platos para confirmar.",info_confirmar_pedido->id_pedido,info_confirmar_pedido->restaurante);
			else{/*CONFIRMO EL PEDIDO*/
				segmento->estado_pedido = CONFIRMADO;
				ret = true;
			}
		}
		else
			log_warning(logger,"El segmento del pedido %d para el restaurante %s no existe.",info_confirmar_pedido->id_pedido,info_confirmar_pedido->restaurante);
		pthread_mutex_unlock(semPed);
	}
	return ret;
}

uint32_t procesar_plato_listo(t_plato_listo* info_plato_listo){
	uint32_t ret = false;
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(info_plato_listo->restaurante))
		log_warning(logger,"La tabla de segmentos del restaurante %s no existe.",info_plato_listo->restaurante);
	else{
		pthread_mutex_t* semPed = mutex_pedido(info_plato_listo->restaurante, info_plato_listo->id_pedido);
		t_segmento* segmento = obtener_segmento_del_pedido(info_plato_listo->id_pedido,info_plato_listo->restaurante);
		/*SE VALIDA SI EXISTE EL SEGMENTO PARA EL PEDIDO DEL RESTAURANTE*/
		if(segmento!=NULL){
			t_entrada_pagina* entrada_pagina = obtener_pagina_de_plato(segmento->tabla_paginas, info_plato_listo->plato);
			/*SE VALIDA SI EL PLATO ESTA EN MEMORIA*/
			if(entrada_pagina != NULL){
				t_pagina* pagina = memoria_fisica + entrada_pagina->nro_frame_mp*sizeof(t_pagina);
				if(pagina->cant_lista < 999999){//if(pagina->cant_lista < pagina->cant_total){
					pagina->cant_lista++;
					actualizar_bits_de_uso(entrada_pagina);
					entrada_pagina->modificado = 1;
					if (pagina->cant_lista == pagina->cant_total)
						segmento->cant_platos_listos++;
					if (segmento->cant_platos_listos == list_size(segmento->tabla_paginas))
						segmento->estado_pedido = TERMINADO;
					ret = true;
				}
				else;
					//log_warning(logger,"El plato %s del pedido %d para el restaurante %s ya se encuantra terminado.",info_plato_listo->plato,info_plato_listo->id_pedido,info_plato_listo->restaurante);
			}
			else
				log_warning(logger,"El plato %s del pedido %d para el restaurante %s no se encuentra en memoria.",info_plato_listo->plato,info_plato_listo->id_pedido,info_plato_listo->restaurante);

			if(segmento->estado_pedido == CONFIRMADO){/*SE VALIDA QUE EL PEDIDO ESTE CONFIRMADO*/
//				t_entrada_pagina* entrada_pagina = obtener_pagina_de_plato(segmento->tabla_paginas, info_plato_listo->plato);
//				/*SE VALIDA SI EL PLATO ESTA EN MEMORIA*/
//				if(entrada_pagina != NULL){
//					t_pagina* pagina = memoria_fisica + entrada_pagina->nro_frame_mp*sizeof(t_pagina);
//					if(pagina->cant_lista < pagina->cant_total){
//						pagina->cant_lista++;
//						actualizar_bits_de_uso(entrada_pagina);
//						entrada_pagina->modificado = 1;
//						if (pagina->cant_lista == pagina->cant_total)
//							segmento->cant_platos_listos++;
//						if (segmento->cant_platos_listos == list_size(segmento->tabla_paginas))
//							segmento->estado_pedido = TERMINADO;
//						ret = true;
//					}
//					else
//						log_warning(logger,"El plato %s del pedido %d para el restaurante %s ya se encuantra terminado.",info_plato_listo->plato,info_plato_listo->id_pedido,info_plato_listo->restaurante);
//				}
//				else{
//					log_warning(logger,"El plato %s del pedido %d para el restaurante %s no se encuentra en memoria.",info_plato_listo->plato,info_plato_listo->id_pedido,info_plato_listo->restaurante);
//				}
				log_trace(logger,"El pedido %d para el restaurante %s se encuentra confirmado.",info_plato_listo->id_pedido,info_plato_listo->restaurante);
			}
			else
				log_warning(logger,"El pedido %d para el restaurante %s no se encuentra confirmado.",info_plato_listo->id_pedido,info_plato_listo->restaurante);
		}
		else
			log_warning(logger,"El segmento del pedido %d para el restaurante %s no existe.",info_plato_listo->id_pedido,info_plato_listo->restaurante);
		pthread_mutex_unlock(semPed);
	}
	return ret;
}

uint32_t procesar_finalizar_pedido(t_finalizar_pedido* info_finalizar_pedido){
	uint32_t ret = false;
	/*SE VALIDA SI EXISTE LA TABLA DE SEGMENTOS DEL RESTAURANTE*/
	if(!existe_tabla_de_segmentos_de_restaurante(info_finalizar_pedido->restaurante))
		log_warning(logger,"La tabla de segmentos del restaurante %s no existe.",info_finalizar_pedido->restaurante);
	else{
		pthread_mutex_t* semPed = mutex_pedido(info_finalizar_pedido->restaurante, info_finalizar_pedido->id_pedido);
		t_segmento* segmento = obtener_segmento_del_pedido(info_finalizar_pedido->id_pedido,info_finalizar_pedido->restaurante);
		/*SE VALIDA SI EXISTE EL SEGMENTO PARA EL PEDIDO DEL RESTAURANTE*/
		if(segmento!=NULL){
			for(int i = 0; i < list_size(segmento->tabla_paginas); i++){
				t_entrada_pagina* entrada_i = list_get(segmento->tabla_paginas, i);
				if (entrada_i->presencia){
					free_frame_mp(entrada_i->nro_frame_mp);
					log_info(logger, "[PARTICION MP ELIMINADA] Se elimino el plato que se encontraba en el frame nro. %d de MP.", entrada_i->nro_frame_mp);
				}
				free_frame_ms(entrada_i->nro_frame_ms);
				log_info(logger, "[PARTICION SWAP ELIMINADA] Se elimino el plato que se encontraba en la posicion %d del area de Swap.", entrada_i->nro_frame_ms);
				borrar_entrada_en_lista_global(entrada_i);
			}
			list_destroy_and_destroy_elements(segmento->tabla_paginas, free);
			borrar_segmento_del_pedido(info_finalizar_pedido->id_pedido,info_finalizar_pedido->restaurante);
			ret = true;
		}
		else
			log_warning(logger,"El segmento del pedido %d para el restaurante %s no existe.",info_finalizar_pedido->id_pedido,info_finalizar_pedido->restaurante);
		pthread_mutex_unlock(semPed);
	}
	return ret;
}
