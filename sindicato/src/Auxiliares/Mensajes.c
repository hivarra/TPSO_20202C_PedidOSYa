/*
 * Mensajes.c
 *
 *  Created on: 5 nov. 2020
 *      Author: utnso
 */
#include "Mensajes.h"

t_rta_consultar_platos* procesar_consultar_platos(char* nombre_restaurante){
	t_rta_consultar_platos* respuesta = NULL;
	return respuesta;
}

uint32_t procesar_guardar_pedido(t_guardar_pedido* msg_guardar_pedido){
	return 0;
}

uint32_t procesar_guardar_plato(t_guardar_plato* msg_guardar_plato){
	return 0;
}

uint32_t procesar_confirmar_pedido(t_confirmar_pedido* msg_confirmar_pedido){
	return 0;
}

t_rta_obtener_pedido* procesar_obtener_pedido(t_obtener_pedido* msg_obtener_pedido){
	t_rta_obtener_pedido* respuesta = NULL;
	return respuesta;
}

t_rta_obtener_restaurante* procesar_obtener_restaurante(char* nombre_restaurante){
	/*Inicializo la respuesta*/
	t_rta_obtener_restaurante* respuesta = malloc(sizeof(t_rta_obtener_restaurante));
	respuesta->posX = 0;
	respuesta->posY = 0;
	respuesta->cantHornos = 0;
	respuesta->cantPedidos = 0;
	respuesta->cantCocineros = 0;
	respuesta->cocineros = list_create();
	respuesta->cantPlatos = 0;
	respuesta->platos = list_create();

	char* path_restaurante = string_from_format("%s%s", ruta_restaurantes, nombre_restaurante);
	/*Valido si existe el restaurante*/
	if (!existeDirectorio(path_restaurante))
		log_warning(logger, "El restaurante %s no existe en el FileSystem AFIP.", nombre_restaurante);
	else{
		/*Leo la metadata del restaurante*/
		char* path_info = string_from_format("%s/Info.AFIP", path_restaurante);
		t_metadata* mdata_restaurante = leerMetadataArchivo(path_info);
		/*Leo los bloques del restaurante*/
		t_file_leido* info_file = leer_bloques_file(mdata_restaurante);
		/*Separo las distintas lineas*/
		char** lineas_info =  string_split(info_file->string_leido, "\n");
		free(info_file->string_leido);
		free(info_file->array_bloques);
		free(info_file);
		/*CANTIDAD COCINEROS*/
		char* strCantCocineros = string_substring_from(lineas_info[0], 19);
		respuesta->cantCocineros = atoi(strCantCocineros);
		free(strCantCocineros);
		/*POSICION*/
		char* strPosicion = string_substring_from(lineas_info[1], 9);
		char** arrayPosicion = string_get_string_as_array(strPosicion);
		respuesta->posX = atoi(arrayPosicion[0]);
		respuesta->posY = atoi(arrayPosicion[1]);
		liberar_lista(arrayPosicion);
		free(strPosicion);
		/*AFINIDAD COCINEROS*/
		char* strAfinidades = string_substring_from(lineas_info[2], 19);
		char** arrayAfinidades = string_get_string_as_array(strAfinidades);
		int hayAfinidades = 1;
		for(int i = 0; i < respuesta->cantCocineros; i++){
			char* afinidad_i = malloc(L_PLATO);
			if(hayAfinidades){
				if (arrayAfinidades[i] != NULL)
					strcpy(afinidad_i, arrayAfinidades[i]);
				else{
					strcpy(afinidad_i, "N");
					hayAfinidades = 0;
				}
			}
			else
				strcpy(afinidad_i, "N");
			list_add(respuesta->cocineros, afinidad_i);
		}
		liberar_lista(arrayAfinidades);
		free(strAfinidades);
		/*PLATOS Y SUS PRECIOS*/
		char* strPlatos = string_substring_from(lineas_info[3], 7);
		char** arrayPlatos = string_get_string_as_array(strPlatos);
		char* strPrecioPlatos = string_substring_from(lineas_info[4], 14);
		char** arrayPrecioPlatos = string_get_string_as_array(strPrecioPlatos);
		int i = 0;
		while(arrayPlatos[i] != NULL && arrayPrecioPlatos[i] != NULL){
			t_plato* plato_i = calloc(1,sizeof(t_plato));
			strcpy(plato_i->nombre, arrayPlatos[i]);
			plato_i->precio = atoi(arrayPrecioPlatos[i]);
			list_add(respuesta->platos, plato_i);
			i++;
		}
		respuesta->cantPlatos = i;
		liberar_lista(arrayPlatos);
		liberar_lista(arrayPrecioPlatos);
		free(strPlatos);
		free(strPrecioPlatos);
		/*CANTIDAD HORNOS*/
		char* strCantHornos = string_substring_from(lineas_info[5], 16);
		respuesta->cantHornos = atoi(strCantHornos);
		free(strCantHornos);
		liberar_lista(lineas_info);
		//TODO: FALTA IMPLEMENTAR CANTIDAD DE PEDIDOS
	}
	free(path_restaurante);

	return respuesta;
}

uint32_t procesar_plato_listo(t_plato_listo* msg_plato_listo){
	return 0;
}

uint32_t procesar_terminar_pedido(t_terminar_pedido* msg_terminar_pedido){
	return 0;
}

t_rta_obtener_receta* procesar_obtener_receta(char* nombre_receta){
	t_rta_obtener_receta* respuesta = NULL;
	return respuesta;
}
