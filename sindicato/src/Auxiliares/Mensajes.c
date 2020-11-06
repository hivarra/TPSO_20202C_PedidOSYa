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
	t_rta_obtener_restaurante* respuesta = malloc(sizeof(t_rta_obtener_restaurante));

	char* path_restaurante = string_from_format("%s%s", ruta_restaurantes, nombre_restaurante);
	/*Valido si existe el restaurante*/
	if (!existeDirectorio(path_restaurante)){
		respuesta->posX = 0;
		respuesta->posY = 0;
		respuesta->cantHornos = 0;
		respuesta->cantPedidos = 0;
		respuesta->cantCocineros = 0;
		respuesta->cantPlatos = 0;
		respuesta->cocineros = list_create();
		log_warning(logger, "El restaurante %s no existe en el FileSystem AFIP.", nombre_restaurante);
	}
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
		/*TODO: HASTA ACA LLEGUE HOY, FALTA SEPARAR POR "="*/

		liberar_lista(lineas_info);
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

