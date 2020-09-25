/*
 * protocolo.c
 *
 *  Created on: 17 sep. 2020
 *      Author: utnso
 */

#include "protocolo.h"


/************PRIVATE************/

void* empaquetar(t_tipoMensaje tipo_mensaje, t_buffer* buffer, int* bytes){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->tipoMensaje = tipo_mensaje;
	paquete->buffer = buffer;

	*bytes = sizeof(paquete->tipoMensaje)
			+ sizeof(buffer->size)
			+ buffer->size;

	void* fullSerializado = malloc(*bytes);
	int offset = 0;

	memcpy(fullSerializado+offset, &(paquete->tipoMensaje), sizeof(paquete->tipoMensaje));
	offset =+ sizeof(paquete->tipoMensaje);
	memcpy(fullSerializado+offset, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	offset =+ sizeof(paquete->buffer->size);
	memcpy(fullSerializado+offset, paquete->buffer->stream, paquete->buffer->size);
	offset =+ paquete->buffer->size;

	free(paquete);

	return fullSerializado;
}

void* serializar_rta_consultar_restaurantes(uint32_t* largo, void* content){

	t_rta_consultar_restaurantes* rta = content;

	*largo += sizeof(rta->cantRestaurantes)
			+rta->cantRestaurantes*L_STRING;

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, &rta->cantRestaurantes, sizeof(rta->cantRestaurantes));
	offset += sizeof(rta->cantRestaurantes);

	for (int i = 0; i < rta->cantRestaurantes; i++){
		memcpy(serializado+offset, list_get(rta->restaurantes, i), L_STRING);
		offset += L_STRING;
	}
	return serializado;
}

void* deserializar_rta_consultar_restaurantes(t_buffer* buffer){

	t_rta_consultar_restaurantes* rta = malloc(sizeof(t_rta_consultar_restaurantes));

	int offset = 0;

	memcpy(&rta->cantRestaurantes, buffer->stream+offset, sizeof(rta->cantRestaurantes));
	offset =+ sizeof(rta->cantRestaurantes);

	rta->restaurantes = list_create();

	for (int i = 0; i < rta->cantRestaurantes; i++){

		char restaurante_i[L_STRING];

		memcpy(restaurante_i, buffer->stream+offset, L_STRING);
		offset += L_STRING;

		list_add(rta->restaurantes, restaurante_i);//REVISAR SI NO SE PIERDE REFERENCIA AL CHAR[]. AL PARECER LA FUNCION LIST_ADD USA STATIC VAR POR LO QUE NO DEBERIA PERDERSE
	}

	free(buffer->stream);

	return rta;
}

void* serializar_rta_obtener_receta(uint32_t* largo, void* content){

	t_receta* rta = content;

	*largo += L_STRING
			+sizeof(rta->precio)
			+sizeof(rta->cantPasos)
			+rta->cantPasos*sizeof(t_paso_receta);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, rta->nombre, L_STRING);
	offset += L_STRING;
	memcpy(serializado+offset, &rta->precio, sizeof(rta->precio));
	offset += sizeof(rta->precio);
	memcpy(serializado+offset, &rta->cantPasos, sizeof(rta->cantPasos));
	offset += sizeof(rta->cantPasos);

	for (int i = 0; i < rta->cantPasos; i++){
		memcpy(serializado+offset, list_get(rta->pasos, i), sizeof(t_paso_receta));
		offset += sizeof(t_paso_receta);
	}
	return serializado;
}

void* deserializar_rta_obtener_receta(t_buffer* buffer){

	t_receta* rta = malloc(sizeof(t_receta));

	int offset = 0;

	memcpy(rta->nombre, buffer->stream+offset, L_STRING);
	offset =+ L_STRING;
	memcpy(&rta->precio, buffer->stream+offset, sizeof(rta->precio));
	offset =+ sizeof(rta->precio);
	memcpy(&rta->cantPasos, buffer->stream+offset, sizeof(rta->cantPasos));
	offset =+ sizeof(rta->cantPasos);

	rta->pasos = list_create();

	for (int i = 0; i < rta->cantPasos; i++){

		t_paso_receta* paso_i = malloc(sizeof(t_paso_receta));

		memcpy(paso_i, buffer->stream+offset, sizeof(t_paso_receta));
		offset =+ sizeof(t_paso_receta);

		list_add(rta->pasos, paso_i);
	}

	free(buffer->stream);

	return rta;
}

void* serializar_rta_obtener_restaurante(uint32_t* largo, void* content){
	/*****Inner Function*****/
	int tamanio_total_recetas(uint32_t cant_recetas, t_list* recetas){
		int tamanio = 0;
		for (int i = 0; i < cant_recetas; i++){
			t_receta* receta_i = list_get(recetas, i);
			tamanio += L_STRING
					+sizeof(receta_i->precio)
					+sizeof(receta_i->cantPasos)
					+receta_i->cantPasos*sizeof(t_paso_receta);
		}
		return tamanio;
	}
	/**********/

	t_rta_obtener_restaurante* rta = content;

	*largo += sizeof(rta->posX)
			+sizeof(rta->posY)
			+sizeof(rta->cantHornos)
			+sizeof(rta->cantCocineros)
			+sizeof(rta->cantAfinidad)
			+rta->cantAfinidad*L_STRING
			+sizeof(rta->cantRecetas)
			+tamanio_total_recetas(rta->cantRecetas, rta->recetas);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, &rta->posX, sizeof(rta->posX));
	offset += sizeof(rta->posX);
	memcpy(serializado+offset, &rta->posY, sizeof(rta->posY));
	offset += sizeof(rta->posY);
	memcpy(serializado+offset, &rta->cantHornos, sizeof(rta->cantHornos));
	offset += sizeof(rta->cantHornos);
	memcpy(serializado+offset, &rta->cantCocineros, sizeof(rta->cantCocineros));
	offset += sizeof(rta->cantCocineros);
	memcpy(serializado+offset, &rta->cantAfinidad, sizeof(rta->cantAfinidad));
	offset += sizeof(rta->cantAfinidad);

	for (int i = 0; i < rta->cantAfinidad; i++){
		memcpy(serializado+offset, list_get(rta->afinidad_cocineros, i), L_STRING);
		offset += L_STRING;
	}

	memcpy(serializado+offset, &rta->cantRecetas, sizeof(rta->cantRecetas));
	offset += sizeof(rta->cantRecetas);

	for (int i = 0; i < rta->cantRecetas; i++){
		uint32_t largo_receta = 0;
		void* receta_serializada = serializar_obtener_receta(&largo_receta, list_get(rta->recetas, i));
		memcpy(serializado+offset, receta_serializada, largo_receta);
		free(receta_serializada);
		offset += largo_receta;
	}
	return serializado;
}

//TODO: FALTA DESERIALIZAR OBTENER RESTAURANTE. CONSULTAR ESTRUCTURAS CON DESIGNADO A RESTAURANTE

void* serializar_rta_consultar_platos(uint32_t* largo, void* content){

	t_rta_consultar_platos* rta = content;

	*largo += sizeof(rta->cantComidas)
			+rta->cantComidas*sizeof(t_comida);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, &rta->cantComidas, sizeof(rta->cantComidas));
	offset += sizeof(rta->cantComidas);

	for (int i = 0; i < rta->cantComidas; i++){
		memcpy(serializado+offset, list_get(rta->comidas, i), sizeof(t_comida));
		offset += sizeof(t_comida);
	}
	return serializado;
}

void* deserializar_rta_consultar_platos(t_buffer* buffer){

	t_rta_consultar_platos* rta = malloc(sizeof(t_rta_consultar_platos));

	int offset = 0;

	memcpy(&rta->cantComidas, buffer->stream+offset, sizeof(rta->cantComidas));
	offset =+ sizeof(rta->cantComidas);

	rta->comidas = list_create();

	for (int i = 0; i < rta->cantComidas; i++){

		t_comida* comida_i = malloc(sizeof(t_comida));

		memcpy(comida_i, buffer->stream+offset, sizeof(t_comida));
		offset =+ sizeof(t_comida);

		list_add(rta->comidas, comida_i);
	}

	free(buffer->stream);

	return rta;
}

void* serializar_rta_consultar_pedido(uint32_t* largo, void* content){

	t_rta_consultar_pedido* rta = content;

	*largo += L_STRING
			+sizeof(rta->estado)
			+sizeof(rta->cantComidas)
			+rta->cantComidas*sizeof(t_comida);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, rta->nombre_restaurante, L_STRING);
	offset += L_STRING;
	memcpy(serializado+offset, &rta->estado, sizeof(rta->estado));
	offset += sizeof(rta->estado);
	memcpy(serializado+offset, &rta->cantComidas, sizeof(rta->cantComidas));
	offset += sizeof(rta->cantComidas);

	for (int i = 0; i < rta->cantComidas; i++){
		memcpy(serializado+offset, list_get(rta->comidas, i), sizeof(t_comida));
		offset += sizeof(t_comida);
	}
	return serializado;
}

void* deserializar_rta_consultar_pedido(t_buffer* buffer){

	t_rta_consultar_pedido* rta = malloc(sizeof(t_rta_consultar_pedido));

	int offset = 0;

	memcpy(rta->nombre_restaurante, buffer->stream+offset, L_STRING);
	offset =+ L_STRING;
	memcpy(&rta->estado, buffer->stream+offset, sizeof(rta->estado));
	offset =+ sizeof(rta->estado);
	memcpy(&rta->cantComidas, buffer->stream+offset, sizeof(rta->cantComidas));
	offset =+ sizeof(rta->cantComidas);

	rta->comidas = list_create();

	for (int i = 0; i < rta->cantComidas; i++){

		t_comida* comida_i = malloc(sizeof(t_comida));

		memcpy(comida_i, buffer->stream+offset, sizeof(t_comida));
		offset =+ sizeof(t_comida);

		list_add(rta->comidas, comida_i);
	}

	free(buffer->stream);

	return rta;
}

void* serializar_rta_obtener_pedido(uint32_t* largo, void* content){

	t_rta_obtener_pedido* rta = content;

	*largo += sizeof(rta->cantComidas)
			+rta->cantComidas*sizeof(t_comida);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, &rta->cantComidas, sizeof(rta->cantComidas));
	offset += sizeof(rta->cantComidas);

	for (int i = 0; i < rta->cantComidas; i++){
		memcpy(serializado+offset, list_get(rta->comidas, i), sizeof(t_comida));
		offset += sizeof(t_comida);
	}
	return serializado;
}

void* deserializar_rta_obtener_pedido(t_buffer* buffer){

	t_rta_obtener_pedido* rta = malloc(sizeof(t_rta_obtener_pedido));

	int offset = 0;

	memcpy(&rta->cantComidas, buffer->stream+offset, sizeof(rta->cantComidas));
	offset =+ sizeof(rta->cantComidas);

	rta->comidas = list_create();

	for (int i = 0; i < rta->cantComidas; i++){

		t_comida* comida_i = malloc(sizeof(t_comida));

		memcpy(comida_i, buffer->stream+offset, sizeof(t_comida));
		offset =+ sizeof(t_comida);

		list_add(rta->comidas, comida_i);
	}

	free(buffer->stream);

	return rta;
}


/************PUBLIC************/

int enviarMensaje(t_tipoMensaje tipoMensaje, void* content, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = 0;
	buffer->stream = NULL;

	switch(tipoMensaje){

	case SELECCIONAR_RESTAURANTE:

		buffer->size = sizeof(t_seleccionar_restaurante);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;

	case OBTENER_RESTAURANTE:

		buffer->size = L_STRING;
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case CONSULTAR_PLATOS:

		buffer->size = L_STRING;
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case GUARDAR_PEDIDO:

		buffer->size = sizeof(t_guardar_pedido);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case ANADIR_PLATO:

		buffer->size = sizeof(t_anadir_plato);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case GUARDAR_PLATO:

		buffer->size = sizeof(t_guardar_plato);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case CONFIRMAR_PEDIDO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case PLATO_LISTO:

		buffer->size = sizeof(t_plato_listo);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case CONSULTAR_PEDIDO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case OBTENER_PEDIDO:

		buffer->size = sizeof(t_obtener_pedido);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case FINALIZAR_PEDIDO:

		buffer->size = sizeof(t_finalizar_pedido);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case TERMINAR_PEDIDO:

		buffer->size = sizeof(t_terminar_pedido);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	case OBTENER_RECETA:

		buffer->size = L_STRING;
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, buffer->size);

		break;
	default:;

		break;
	}//LOS MENSAJES QUE NO ESTEN EN ESA LISTA SE ENVIARAN SIN PARAMETROS (CONSULTAR_RESTAURANTES, CREAR_PEDIDO)

	int cant_bytes;
	void* paquete = empaquetar(tipoMensaje, buffer, &cant_bytes);

	if (buffer->stream != NULL)
		free(buffer->stream);

	free(buffer);

	if (send(socketReceptor, paquete, cant_bytes, 0) <= 0) {
		free(paquete);
		log_error(logger, "No se pudo enviar mensaje %s al socket %d.", get_nombre_mensaje(tipoMensaje), socketReceptor);
		return -1;
	}

	free(paquete);
	return 0;
}

void* recibirMensaje(int socketEmisor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = 0;
	buffer->stream = NULL;

	int bytes_recibidos = 0;

	bytes_recibidos =+ recv(socketEmisor, &buffer->size, sizeof(buffer->size), MSG_WAITALL);

	if (buffer->size > 0){
		buffer->stream = malloc(buffer->size);
		bytes_recibidos =+ recv(socketEmisor, buffer->stream, buffer->size, MSG_WAITALL);
	}

	if (bytes_recibidos < sizeof(buffer->size)+buffer->size) {
		if (buffer->stream != NULL)
			free(buffer->stream);
		free(buffer);
		log_error(logger, "Se desconecto o ocurrio un error con el socket (sock_id: %d).", socketEmisor);
		return NULL;
	}

	void* stream = buffer->stream;
	free(buffer);

	return stream;
}


int enviarRespuesta(t_tipoRespuesta tipoRespuesta, void* content, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = 0;
	buffer->stream = NULL;

	switch(tipoRespuesta){

	case RTA_CONSULTAR_RESTAURANTES:

		buffer->stream = serializar_rta_consultar_restaurantes(&buffer->size, content);
		break;

	case RTA_SELECCIONAR_RESTAURANTE:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_OBTENER_RESTAURANTE:

		buffer->stream = serializar_rta_obtener_restaurante(&buffer->size, content);
		break;

	case RTA_CONSULTAR_PLATOS:

		buffer->stream = serializar_rta_consultar_platos(&buffer->size, content);
		break;

	case RTA_CREAR_PEDIDO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_GUARDAR_PEDIDO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_ANADIR_PLATO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_GUARDAR_PLATO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_CONFIRMAR_PEDIDO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_PLATO_LISTO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_CONSULTAR_PEDIDO:

		buffer->stream = serializar_rta_consultar_pedido(&buffer->size, content);
		break;

	case RTA_OBTENER_PEDIDO:

		buffer->stream = serializar_rta_obtener_pedido(&buffer->size, content);
		break;

	case RTA_FINALIZAR_PEDIDO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_TERMINAR_PEDIDO:

		buffer->size = sizeof(uint32_t);
		buffer->stream = malloc(buffer->size);
		memcpy(buffer->stream, content, sizeof(uint32_t));
		break;

	case RTA_OBTENER_RECETA:

		buffer->stream = serializar_rta_obtener_receta(&buffer->size, content);
		break;
	}

	int cant_bytes;
	void* paquete = empaquetar(tipoRespuesta, buffer, &cant_bytes);

	if (buffer->stream != NULL)
		free(buffer->stream);

	free(buffer);

	if (send(socketReceptor, paquete, cant_bytes, 0) <= 0) {
		free(paquete);
		//FALTA IMPLEMTENTAR TIPO RESPUESTAS log_error(logger, "No se pudo enviar la respuesta %s al socket %d.", get_nombre_mensaje(tipoRespuesta), socketReceptor);
		return -1;
	}

	free(paquete);
	return 0;
}

void* recibirRespuesta(int socketEmisor, t_tipoRespuesta tipoRespuesta, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = 0;
	buffer->stream = NULL;

	int bytes_recibidos = 0;

	bytes_recibidos =+ recv(socketEmisor, &buffer->size, sizeof(buffer->size), MSG_WAITALL);

	if (buffer->size > 0){
		buffer->stream = malloc(buffer->size);
		bytes_recibidos =+ recv(socketEmisor, buffer->stream, buffer->size, MSG_WAITALL);
	}

	if (bytes_recibidos < sizeof(buffer->size)+buffer->size) {
		if (buffer->stream != NULL)
			free(buffer->stream);
		free(buffer);
		log_error(logger, "Se desconecto o ocurrio un error con el socket (sock_id: %d).", socketEmisor);
		return NULL;
	}

	void* stream = NULL;

	switch(tipoRespuesta){

	case RTA_CONSULTAR_RESTAURANTES:

		stream = deserializar_rta_consultar_restaurantes(buffer);
		break;

	case RTA_OBTENER_RESTAURANTE:

//		stream = deserializar_rta_obtener_restaurante(buffer);
		break;

	case RTA_CONSULTAR_PLATOS:

		stream = deserializar_rta_consultar_platos(buffer);
		break;

	case RTA_CONSULTAR_PEDIDO:

		stream = deserializar_rta_consultar_pedido(buffer);
		break;

	case RTA_OBTENER_PEDIDO:

		stream = deserializar_rta_obtener_pedido(buffer);
		break;

	case RTA_OBTENER_RECETA:

		stream = deserializar_rta_obtener_receta(buffer);
		break;

	default://LAS RESPUESTAS QUE NO NECESITAN SER DESERIALIZADAS SE DEVUELVEN DIRECTAMENTE, LUEGO CASTEANDO FUERA DE ESTA FUNCION GRACIAS A ATRIBUTTED PACKED

			stream = buffer->stream;

		break;
	}

	free(buffer);

	return stream;
}
