/*
 * Mensajes.c
 *
 *  Created on: 5 nov. 2020
 *      Author: utnso
 */
#include "Mensajes.h"

char* concatenarLineasModificadas(char** lineas){

	char* concatenado = string_new();
	int i = 0;
	while(lineas[i] != NULL){
		if(lineas[i+1] != NULL)
			string_append_with_format(&concatenado, "%s\n", lineas[i]);
		else
			string_append(&concatenado, lineas[i]);
		i++;
	}
	return concatenado;
}

t_rta_consultar_platos* procesar_consultar_platos(char* nombre_restaurante){
	t_rta_consultar_platos* respuesta = malloc(sizeof(t_rta_consultar_platos));
	/*Inicializo la respuesta*/
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
		free(path_info);
		/*Leo los bloques del restaurante*/
		t_file_leido* info_file = leer_bloques_file(mdata_restaurante);
		free(mdata_restaurante);
		/*Separo las distintas lineas*/
		char** lineas_info =  string_split(info_file->string_leido, "\n");
		free(info_file->string_leido);
		free(info_file->array_bloques);
		free(info_file);
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
		/*LIBERO LINEAS*/
		liberar_lista(lineas_info);
	}
	free(path_restaurante);

	return respuesta;
}

uint32_t procesar_guardar_pedido(t_guardar_pedido* msg_guardar_pedido){
	uint32_t result = 0;
	char* path_pedido = string_from_format("%s%s", ruta_restaurantes, msg_guardar_pedido->restaurante);//Es el path del restaurante, pero lo voy a reusar
	/*Valido si existe el restaurante*/
	if (!existeDirectorio(path_pedido))
		log_warning(logger, "El restaurante %s no existe en el FileSystem AFIP.", msg_guardar_pedido->restaurante);
	else {
		/*Valido si existe el pedido*/
		string_append_with_format(&path_pedido, "/Pedido%d.AFIP", msg_guardar_pedido->id_pedido);
		if (existeFile(path_pedido))
			log_warning(logger, "El pedido %d del restaurante %s ya existe.", msg_guardar_pedido->id_pedido, msg_guardar_pedido->restaurante);
		else{
			/*Creo el contenido del archivo*/
			char* contenido = string_from_format("ESTADO_PEDIDO=Pendiente\nLISTA_PLATOS=[]\nCANTIDAD_PLATOS=[]\nCANTIDAD_LISTA=[]\nPRECIO_TOTAL=0");
			/*Creo los datos de la metadata*/
			t_metadata* file_mdata = malloc(sizeof(t_metadata));
			file_mdata->size = strlen(contenido)+1;
			/*Solicito bloques nuevos*/
			int* bloques = listar_bloques_necesarios_file_nuevo(file_mdata->size);
			file_mdata->initial_block = bloques[0];
			/*Guardo contenido en los bloques*/
			persistirDatos(contenido, bloques);
			/*Creo el archivo metadata*/
			crearMetadataArchivo(path_pedido, file_mdata);

			free(contenido);
			free(bloques);
			free(file_mdata);
			result = 1;
		}
	}
	free(path_pedido);
	return result;
}

uint32_t procesar_guardar_plato(t_guardar_plato* msg_guardar_plato){
	/*Funcion auxiliar para buscar el plato en el restaurante(para calcular el precio total)*/
	int precioPlato(){
		int precio = -1;
		bool _contienePlato(t_plato* platoIterado){
			return strcmp(platoIterado->nombre, msg_guardar_plato->plato) == 0;
		}
		t_rta_consultar_platos* consultar_platos = procesar_consultar_platos(msg_guardar_plato->restaurante);
		t_plato* platoYprecio = list_find(consultar_platos->platos,(void*)_contienePlato);
		if(platoYprecio)
			precio = platoYprecio->precio;
		list_destroy_and_destroy_elements(consultar_platos->platos, free);
		free(consultar_platos);
		return precio;
	}
	/****************************************************************************************/

	uint32_t result = 0;

	char* path_pedido = string_from_format("%s%s", ruta_restaurantes, msg_guardar_plato->restaurante);//Es el path del restaurante, pero lo voy a reusar
	/*Valido si existe el restaurante*/
	if (!existeDirectorio(path_pedido))
		log_warning(logger, "El restaurante %s no existe en el FileSystem AFIP.", msg_guardar_plato->restaurante);
	else {
		/*Valido si existe el pedido*/
		string_append_with_format(&path_pedido, "/Pedido%d.AFIP", msg_guardar_plato->id_pedido);
		if (!existeFile(path_pedido))
			log_warning(logger, "El pedido %d del restaurante %s no existe.", msg_guardar_plato->id_pedido, msg_guardar_plato->restaurante);
		else{
			/*Leo los datos de la metadata*/
			t_metadata* file_mdata = leerMetadataArchivo(path_pedido);
			/*Leo los bloques del restaurante*/
			t_file_leido* contenido_file = leer_bloques_file(file_mdata);
			/*Separo las distintas lineas*/
			char** lineas_info =  string_split(contenido_file->string_leido, "\n");
			free(contenido_file->string_leido);
			/*Valido el estado del pedido*/
			char* estadoPedido = string_substring_from(lineas_info[0], 14);
			if (strcmp(estadoPedido, "Pendiente") != 0)
				log_warning(logger, "El pedido %d del restaurante %s no se encuentra PENDIENTE.", msg_guardar_plato->id_pedido, msg_guardar_plato->restaurante);
			else{
				/*Verifico si el plato ya existe en el pedido*/
				if(!string_contains(lineas_info[1], msg_guardar_plato->plato)){//Si no existe, lo agrego, con su cantidad
					int tam_linea_platos = strlen(lineas_info[1]);
					char* linea_platos = string_substring_until(lineas_info[1], tam_linea_platos-1);
					char* linea_cant_platos = string_substring_until(lineas_info[2], strlen(lineas_info[2])-1);
					char* linea_cant_lista = string_substring_until(lineas_info[3], strlen(lineas_info[3])-1);
					if (tam_linea_platos > 15){
						string_append_with_format(&linea_platos, ",%s]", msg_guardar_plato->plato);
						string_append_with_format(&linea_cant_platos, ",%d]", msg_guardar_plato->cantPlato);
						string_append(&linea_cant_lista, ",0]");
					}
					else{
						string_append_with_format(&linea_platos, "%s]", msg_guardar_plato->plato);
						string_append_with_format(&linea_cant_platos, "%d]", msg_guardar_plato->cantPlato);
						string_append(&linea_cant_lista, "0]");
					}
					void* aux_delete1 = lineas_info[1];
					void* aux_delete2 = lineas_info[2];
					void* aux_delete3 = lineas_info[3];
					lineas_info[1] = linea_platos;
					lineas_info[2] = linea_cant_platos;
					lineas_info[3] = linea_cant_lista;
					free(aux_delete1);
					free(aux_delete2);
					free(aux_delete3);
				}
				else{//Si existe busco en que parte del string esta el plato, y aumento su cantidad en el string de abajo
					char* listaPlatos = string_substring_from(lineas_info[1], 13);
					char** arrayPlatos = string_get_string_as_array(listaPlatos);
					free(listaPlatos);
					char* listaCantidades = string_substring_from(lineas_info[2], 16);
					char** arrayCantidades = string_get_string_as_array(listaCantidades);
					free(listaCantidades);
					char* newLineaCantidades = string_from_format("CANTIDAD_PLATOS=[");
					int i = 0;
					while(arrayPlatos[i]!=NULL && arrayCantidades[i]!=NULL){
						if (strcmp(arrayPlatos[i], msg_guardar_plato->plato) == 0){
							int nuevaCantidad = atoi(arrayCantidades[i]) + msg_guardar_plato->cantPlato;
							if(arrayCantidades[i+1]!=NULL)
								string_append_with_format(&newLineaCantidades, "%d,", nuevaCantidad);
							else
								string_append_with_format(&newLineaCantidades, "%d", nuevaCantidad);
						}
						else{
							if(arrayCantidades[i+1]!=NULL)
								string_append_with_format(&newLineaCantidades, "%s,", arrayCantidades[i]);
							else
								string_append(&newLineaCantidades, arrayCantidades[i]);
						}
						i++;
					}
					string_append(&newLineaCantidades, "]");
					liberar_lista(arrayPlatos);
					liberar_lista(arrayCantidades);
					void* aux_delete = lineas_info[2];
					lineas_info[2] = newLineaCantidades;
					free(aux_delete);
				}
				//TODO:FALTA CREAR LAS CANTIDADES LISTAS EN 0
				//Actualizo el precio total del pedido
				int precio_unitario = precioPlato();
				char** lineasPrecioTotal =  string_split(lineas_info[4], "=");
				int precio_total = atoi(lineasPrecioTotal[1]) + precio_unitario*msg_guardar_plato->cantPlato;
				char* nuevaLineaPrecioTotal = string_from_format("%s=%d", lineasPrecioTotal[0], precio_total);
				liberar_lista(lineasPrecioTotal);
				void* aux_delete = lineas_info[4];
				lineas_info[4] = nuevaLineaPrecioTotal;
				free(aux_delete);
				//Creo un nuevo super string concatenando todas las lineas modificadas
				char* nuevo_contenido = concatenarLineasModificadas(lineas_info);
				/*Actualizo los files*/
				contenido_file->array_bloques = listar_bloques_necesarios_file_existente(strlen(nuevo_contenido)+1, file_mdata->size, contenido_file->array_bloques);
				persistirDatos(nuevo_contenido, contenido_file->array_bloques);
				actualizarSizeMetadataArchivo(path_pedido, strlen(nuevo_contenido)+1);
				free(nuevo_contenido);
				/*Lo que todos estabamos esperando*/
				result = 1;
			}
			liberar_lista(lineas_info);
			free(contenido_file->array_bloques);
			free(contenido_file);
			free(estadoPedido);
			free(file_mdata);
		}
	}
	free(path_pedido);
	return result;
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
		free(path_info);
		/*Leo los bloques del restaurante*/
		t_file_leido* info_file = leer_bloques_file(mdata_restaurante);
		free(mdata_restaurante);
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
			char* afinidad_i = calloc(1,L_PLATO);
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
		/*CANTIDAD PEDIDOS*/
		int j = 1;
		int existenPedidos = 1;
		while(existenPedidos){
			char* pathPedido_j = string_from_format("%s/Pedido%d.AFIP", path_restaurante, j);
			if (existeFile(pathPedido_j))
				j++;
			else
				existenPedidos = 0;
			free(pathPedido_j);
		}
		respuesta->cantPedidos = j-1;
		/*LIBERO LINEAS*/
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
	/*Inicializo la respuesta*/
	t_rta_obtener_receta* respuesta = calloc(1,sizeof(t_rta_obtener_receta));
	strcpy(respuesta->nombre, nombre_receta);
	respuesta->cantPasos = 0;
	respuesta->pasos = list_create();

	char* path_receta = string_from_format("%s%s.AFIP", ruta_recetas, nombre_receta);
	/*Valido si existe la receta*/
	if (!existeFile(path_receta))
		log_warning(logger, "La receta %s no existe en el FileSystem AFIP.", nombre_receta);
	else{
		/*Leo la metadata de la receta*/
		t_metadata* mdata_receta = leerMetadataArchivo(path_receta);
		/*Leo los bloques de la receta*/
		t_file_leido* info_file = leer_bloques_file(mdata_receta);
		free(mdata_receta);
		/*Separo las distintas lineas*/
		char** lineas_info =  string_split(info_file->string_leido, "\n");
		free(info_file->string_leido);
		free(info_file->array_bloques);
		free(info_file);
		/*Pasos y tiempos*/
		char* strPasos = string_substring_from(lineas_info[0], 6);
		char** arrayPasos = string_get_string_as_array(strPasos);
		char* strTiempos = string_substring_from(lineas_info[1], 13);
		char** arrayTiempos = string_get_string_as_array(strTiempos);
		int i = 0;
		while(arrayPasos[i] != NULL && arrayTiempos[i] != NULL){
			t_paso_receta* paso_i = calloc(1,sizeof(t_paso_receta));
			strcpy(paso_i->accion, arrayPasos[i]);
			paso_i->tiempo = atoi(arrayTiempos[i]);
			list_add(respuesta->pasos, paso_i);
			i++;
		}
		respuesta->cantPasos = i;
		liberar_lista(arrayPasos);
		liberar_lista(arrayTiempos);
		free(strPasos);
		free(strTiempos);
		/*LIBERO LINEAS*/
		liberar_lista(lineas_info);
	}
	free(path_receta);

	return respuesta;
}

