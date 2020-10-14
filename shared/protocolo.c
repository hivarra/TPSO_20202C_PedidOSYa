/*
 * protocolo.c
 *
 *  Created on: 17 sep. 2020
 *      Author: utnso
 */

#include "protocolo.h"


/************PRIVATE************/

int empaquetar_enviar_free_buffer(t_tipoMensaje tipoMensaje, t_buffer* buffer, int socketReceptor, t_log* logger){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->tipoMensaje = tipoMensaje;
	paquete->buffer = buffer;

	int bytes = sizeof(paquete->tipoMensaje)
			+ sizeof(buffer->size)
			+ buffer->size;

	void* fullSerializado = malloc(bytes);
	int offset = 0;

	memcpy(fullSerializado+offset, &paquete->tipoMensaje, sizeof(paquete->tipoMensaje));
	offset += sizeof(paquete->tipoMensaje);
	memcpy(fullSerializado+offset, &paquete->buffer->size, sizeof(paquete->buffer->size));
	offset += sizeof(paquete->buffer->size);
	memcpy(fullSerializado+offset, paquete->buffer->stream, paquete->buffer->size);
	offset += paquete->buffer->size;

	free(paquete);

	if(buffer->stream != NULL)
		free(buffer->stream);

	free(buffer);

	if (send(socketReceptor, fullSerializado, bytes, 0) != bytes) {
		log_error(logger, "No se pudo enviar el mensaje %s al socket %d.", get_nombre_mensaje(tipoMensaje), socketReceptor);
		free(fullSerializado);
		return -1;
	}
	free(fullSerializado);
	return 0;
}

void* recibirMensaje(int socketEmisor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = NULL;

	int bytes_recibidos = 0;

	bytes_recibidos += recv(socketEmisor, &buffer->size, sizeof(buffer->size), MSG_WAITALL);

	if (buffer->size > 0){
		buffer->stream = malloc(buffer->size);
		bytes_recibidos += recv(socketEmisor, buffer->stream, buffer->size, MSG_WAITALL);
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

void* serializar_rta_consultar_restaurantes(uint32_t* largo, t_rta_consultar_restaurantes* rta){

	*largo = sizeof(rta->cantRestaurantes)
			+rta->cantRestaurantes*L_ID;

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, &rta->cantRestaurantes, sizeof(rta->cantRestaurantes));
	offset += sizeof(rta->cantRestaurantes);

	for (int i = 0; i < rta->cantRestaurantes; i++){
		memcpy(serializado+offset, list_get(rta->restaurantes, i), L_ID);
		offset += L_ID;
	}
	return serializado;
}

t_rta_consultar_restaurantes* deserializar_rta_consultar_restaurantes(void* serializado){

	t_rta_consultar_restaurantes* rta = malloc(sizeof(t_rta_consultar_restaurantes));

	int offset = 0;

	memcpy(&rta->cantRestaurantes, serializado+offset, sizeof(rta->cantRestaurantes));
	offset += sizeof(rta->cantRestaurantes);

	rta->restaurantes = list_create();

	for (int i = 0; i < rta->cantRestaurantes; i++){

		char* restaurante_i = malloc(L_ID);

		memcpy(restaurante_i, serializado+offset, L_ID);
		offset += L_ID;

		list_add(rta->restaurantes, restaurante_i);
	}

	free(serializado);

	return rta;
}

void* serializar_rta_obtener_restaurante(uint32_t* largo, t_rta_obtener_restaurante* rta){

	*largo = sizeof(rta->posX)
			+sizeof(rta->posY)
			+sizeof(rta->cantHornos)
			+sizeof(rta->cantPedidos)
			+sizeof(rta->cantCocineros)
			+rta->cantCocineros*L_PLATO
			+sizeof(rta->cantPlatos)
			+rta->cantPlatos*sizeof(t_plato);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, &rta->posX, sizeof(rta->posX));
	offset += sizeof(rta->posX);
	memcpy(serializado+offset, &rta->posY, sizeof(rta->posY));
	offset += sizeof(rta->posY);
	memcpy(serializado+offset, &rta->cantHornos, sizeof(rta->cantHornos));
	offset += sizeof(rta->cantHornos);
	memcpy(serializado+offset, &rta->cantPedidos, sizeof(rta->cantPedidos));
	offset += sizeof(rta->cantPedidos);
	memcpy(serializado+offset, &rta->cantCocineros, sizeof(rta->cantCocineros));
	offset += sizeof(rta->cantCocineros);

	for (int i = 0; i < rta->cantCocineros; i++){
		memcpy(serializado+offset, list_get(rta->cocineros, i), L_PLATO);
		offset += L_PLATO;
	}

	memcpy(serializado+offset, &rta->cantPlatos, sizeof(rta->cantPlatos));
	offset += sizeof(rta->cantPlatos);

	for (int i = 0; i < rta->cantPlatos; i++){
		memcpy(serializado+offset, list_get(rta->platos, i), sizeof(t_plato));
		offset += sizeof(t_plato);
	}
	return serializado;
}

t_rta_obtener_restaurante* deserializar_rta_obtener_restaurante(void* serializado){

	t_rta_obtener_restaurante* rta = malloc(sizeof(t_rta_obtener_restaurante));

	int offset = 0;

	memcpy(&rta->posX, serializado+offset, sizeof(rta->posX));
	offset += sizeof(rta->posX);
	memcpy(&rta->posY, serializado+offset, sizeof(rta->posY));
	offset += sizeof(rta->posY);
	memcpy(&rta->cantHornos, serializado+offset, sizeof(rta->cantHornos));
	offset += sizeof(rta->cantHornos);
	memcpy(&rta->cantPedidos, serializado+offset, sizeof(rta->cantPedidos));
	offset += sizeof(rta->cantPedidos);
	memcpy(&rta->cantCocineros, serializado+offset, sizeof(rta->cantCocineros));
	offset += sizeof(rta->cantCocineros);

	rta->cocineros = list_create();

	for (int i = 0; i < rta->cantCocineros; i++){

		char* afinidad_cocinero_i = malloc(L_PLATO);

		memcpy(afinidad_cocinero_i, serializado+offset, L_PLATO);
		offset += L_PLATO;

		list_add(rta->cocineros, afinidad_cocinero_i);
	}

	memcpy(&rta->cantPlatos, serializado+offset, sizeof(rta->cantPlatos));
	offset += sizeof(rta->cantPlatos);

	rta->platos = list_create();

	for (int i = 0; i < rta->cantPlatos; i++){

		t_plato* plato_i = malloc(sizeof(t_plato));

		memcpy(plato_i, serializado+offset, sizeof(t_plato));
		offset += sizeof(t_plato);

		list_add(rta->platos, plato_i);
	}

	free(serializado);

	return rta;
}

void* serializar_rta_consultar_platos(uint32_t* largo, t_rta_consultar_platos* rta){

	*largo = sizeof(rta->cantPlatos)
			+rta->cantPlatos*sizeof(t_plato);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, &rta->cantPlatos, sizeof(rta->cantPlatos));
	offset += sizeof(rta->cantPlatos);

	for (int i = 0; i < rta->cantPlatos; i++){
		memcpy(serializado+offset, list_get(rta->platos, i), sizeof(t_plato));
		offset += sizeof(t_plato);
	}
	return serializado;
}

t_rta_consultar_platos* deserializar_rta_consultar_platos(void* serializado){

	t_rta_consultar_platos* rta = malloc(sizeof(t_rta_consultar_platos));

	int offset = 0;

	memcpy(&rta->cantPlatos, serializado+offset, sizeof(rta->cantPlatos));
	offset += sizeof(rta->cantPlatos);

	rta->platos = list_create();

	for (int i = 0; i < rta->cantPlatos; i++){

		t_plato* plato_i = malloc(sizeof(t_plato));

		memcpy(plato_i, serializado+offset, sizeof(t_plato));
		offset += sizeof(t_plato);

		list_add(rta->platos, plato_i);
	}

	free(serializado);

	return rta;
}

void* serializar_rta_consultar_pedido(uint32_t* largo, t_rta_consultar_pedido* rta){

	*largo = L_ID
			+sizeof(rta->estado)
			+sizeof(rta->cantComidas)
			+rta->cantComidas*sizeof(t_comida);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, rta->restaurante, L_ID);
	offset += L_ID;
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

t_rta_consultar_pedido* deserializar_rta_consultar_pedido(void* serializado){

	t_rta_consultar_pedido* rta = malloc(sizeof(t_rta_consultar_pedido));

	int offset = 0;

	memcpy(rta->restaurante, serializado+offset, L_ID);
	offset += L_ID;
	memcpy(&rta->estado, serializado+offset, sizeof(rta->estado));
	offset += sizeof(rta->estado);
	memcpy(&rta->cantComidas, serializado+offset, sizeof(rta->cantComidas));
	offset += sizeof(rta->cantComidas);

	rta->comidas = list_create();

	for (int i = 0; i < rta->cantComidas; i++){

		t_comida* comida_i = malloc(sizeof(t_comida));

		memcpy(comida_i, serializado+offset, sizeof(t_comida));
		offset += sizeof(t_comida);

		list_add(rta->comidas, comida_i);
	}

	free(serializado);

	return rta;
}

void* serializar_rta_obtener_pedido(uint32_t* largo, t_rta_obtener_pedido* rta){

	*largo = sizeof(rta->estado)
			+sizeof(rta->cantComidas)
			+rta->cantComidas*sizeof(t_comida);

	void* serializado = malloc(*largo);
	int offset = 0;

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

t_rta_obtener_pedido* deserializar_rta_obtener_pedido(void* serializado){

	t_rta_obtener_pedido* rta = malloc(sizeof(t_rta_obtener_pedido));

	int offset = 0;

	memcpy(&rta->estado, serializado+offset, sizeof(rta->estado));
	offset += sizeof(rta->estado);
	memcpy(&rta->cantComidas, serializado+offset, sizeof(rta->cantComidas));
	offset += sizeof(rta->cantComidas);

	rta->comidas = list_create();

	for (int i = 0; i < rta->cantComidas; i++){

		t_comida* comida_i = malloc(sizeof(t_comida));

		memcpy(comida_i, serializado+offset, sizeof(t_comida));
		offset += sizeof(t_comida);

		list_add(rta->comidas, comida_i);
	}

	free(serializado);

	return rta;
}

void* serializar_rta_obtener_receta(uint32_t* largo, t_rta_obtener_receta* rta){

	*largo = L_PLATO
			+sizeof(rta->cantPasos)
			+rta->cantPasos*sizeof(t_paso_receta);

	void* serializado = malloc(*largo);
	int offset = 0;

	memcpy(serializado+offset, rta->nombre, L_PLATO);
	offset += L_PLATO;
	memcpy(serializado+offset, &rta->cantPasos, sizeof(rta->cantPasos));
	offset += sizeof(rta->cantPasos);

	for (int i = 0; i < rta->cantPasos; i++){
		memcpy(serializado+offset, list_get(rta->pasos, i), sizeof(t_paso_receta));
		offset += sizeof(t_paso_receta);
	}
	return serializado;
}

t_rta_obtener_receta* deserializar_rta_obtener_receta(void* serializado){

	t_rta_obtener_receta* rta = malloc(sizeof(t_rta_obtener_receta));

	int offset = 0;

	memcpy(rta->nombre, serializado+offset, L_PLATO);
	offset += L_PLATO;
	memcpy(&rta->cantPasos, serializado+offset, sizeof(rta->cantPasos));
	offset += sizeof(rta->cantPasos);

	rta->pasos = list_create();

	for (int i = 0; i < rta->cantPasos; i++){

		t_paso_receta* paso_i = malloc(sizeof(t_paso_receta));

		memcpy(paso_i, serializado+offset, sizeof(t_paso_receta));
		offset += sizeof(t_paso_receta);

		list_add(rta->pasos, paso_i);
	}

	free(serializado);

	return rta;
}

/************PUBLIC************/

/************ENVIAR MENSAJES************/

int enviar_handshake(int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = 0;
	buffer->stream = NULL;

	int resultado_envio = empaquetar_enviar_free_buffer(HANDSHAKE, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_mensaje_vacio(t_tipoMensaje tipoMensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = 0;
	buffer->stream = NULL;

	int resultado_envio = empaquetar_enviar_free_buffer(tipoMensaje, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_entero(t_tipoMensaje tipoMensaje, uint32_t numero, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(uint32_t);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, &numero, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(tipoMensaje, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_socket_envio(t_socket_envio* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_socket_envio);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(SOCKET_ENVIO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_socket_escucha(t_socket_escucha* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_socket_escucha);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(SOCKET_ESCUCHA, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_seleccionar_restaurante(char* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = L_ID;
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(SELECCIONAR_RESTAURANTE, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_obtener_restaurante(char* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = L_ID;
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(OBTENER_RESTAURANTE, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_consultar_platos(char* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = L_ID;
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(CONSULTAR_PLATOS, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_guardar_pedido(t_guardar_pedido* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_guardar_pedido);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(GUARDAR_PEDIDO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_anadir_plato(t_anadir_plato* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_anadir_plato);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(ANADIR_PLATO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_guardar_plato(t_guardar_plato* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_guardar_plato);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(GUARDAR_PLATO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_confirmar_pedido(t_confirmar_pedido* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_confirmar_pedido);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(CONFIRMAR_PEDIDO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_plato_listo(t_plato_listo* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_plato_listo);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(PLATO_LISTO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_obtener_pedido(t_obtener_pedido* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_obtener_pedido);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(OBTENER_PEDIDO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_finalizar_pedido(t_finalizar_pedido* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_finalizar_pedido);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(FINALIZAR_PEDIDO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_terminar_pedido(t_terminar_pedido* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(t_terminar_pedido);
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(TERMINAR_PEDIDO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_obtener_receta(char* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = L_PLATO;
	buffer->stream = malloc(buffer->size);
	memcpy(buffer->stream, mensaje, buffer->size);

	int resultado_envio = empaquetar_enviar_free_buffer(OBTENER_RECETA, buffer, socketReceptor, logger);

	return resultado_envio;
}

/************ENVIAR RESPUESTAS************/

int enviar_rta_consultar_restaurantes(t_rta_consultar_restaurantes* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = serializar_rta_consultar_restaurantes(&buffer->size, mensaje);

	int resultado_envio = empaquetar_enviar_free_buffer(RTA_CONSULTAR_RESTAURANTES, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_rta_obtener_restaurante(t_rta_obtener_restaurante* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = serializar_rta_obtener_restaurante(&buffer->size, mensaje);

	int resultado_envio = empaquetar_enviar_free_buffer(RTA_OBTENER_RESTAURANTE, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_rta_consultar_platos(t_rta_consultar_platos* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = serializar_rta_consultar_platos(&buffer->size, mensaje);

	int resultado_envio = empaquetar_enviar_free_buffer(RTA_CONSULTAR_PLATOS, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_rta_consultar_pedido(t_rta_consultar_pedido* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = serializar_rta_consultar_pedido(&buffer->size, mensaje);

	int resultado_envio = empaquetar_enviar_free_buffer(RTA_CONSULTAR_PEDIDO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_rta_obtener_pedido(t_rta_obtener_pedido* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = serializar_rta_obtener_pedido(&buffer->size, mensaje);

	int resultado_envio = empaquetar_enviar_free_buffer(RTA_OBTENER_PEDIDO, buffer, socketReceptor, logger);

	return resultado_envio;
}

int enviar_rta_obtener_receta(t_rta_obtener_receta* mensaje, int socketReceptor, t_log* logger) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream = serializar_rta_obtener_receta(&buffer->size, mensaje);

	int resultado_envio = empaquetar_enviar_free_buffer(RTA_OBTENER_RECETA, buffer, socketReceptor, logger);

	return resultado_envio;
}

/************RECIBIR MENSAJES************/

t_tipoMensaje recibir_tipo_mensaje(int socket_cliente, t_log* logger) {
	uint32_t tipo_mensaje_u;

	if (recv(socket_cliente, &tipo_mensaje_u, sizeof(uint32_t), MSG_WAITALL) < sizeof(uint32_t)) {
		return -1;
	}

	t_tipoMensaje tipo_mensaje = tipo_mensaje_u;
	return tipo_mensaje;
}

void* recibir_mensaje_vacio(int socketEmisor, t_log* logger) {

	return recibirMensaje(socketEmisor, logger);//==return NULL
}

uint32_t recibir_entero(int socketEmisor, t_log* logger) {

	uint32_t* recibido = recibirMensaje(socketEmisor, logger);

	uint32_t entero = *recibido;
	free(recibido);

	return entero;
}

t_socket_envio* recibir_socket_envio(int socketEmisor, t_log* logger) {

	t_socket_envio* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_socket_escucha* recibir_socket_escucha(int socketEmisor, t_log* logger) {

	t_socket_escucha* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

char* recibir_seleccionar_restaurante(int socketEmisor, t_log* logger) {

	char* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

char* recibir_obtener_restaurante(int socketEmisor, t_log* logger) {

	char* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

char* recibir_consultar_platos(int socketEmisor, t_log* logger) {

	char* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_guardar_pedido* recibir_guardar_pedido(int socketEmisor, t_log* logger) {

	t_guardar_pedido* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_anadir_plato* recibir_anadir_plato(int socketEmisor, t_log* logger) {

	t_anadir_plato* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_guardar_plato* recibir_guardar_plato(int socketEmisor, t_log* logger) {

	t_guardar_plato* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_confirmar_pedido* recibir_confirmar_pedido(int socketEmisor, t_log* logger) {

	t_confirmar_pedido* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_plato_listo* recibir_plato_listo(int socketEmisor, t_log* logger) {

	t_plato_listo* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_obtener_pedido* recibir_obtener_pedido(int socketEmisor, t_log* logger) {

	t_obtener_pedido* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_finalizar_pedido* recibir_finalizar_pedido(int socketEmisor, t_log* logger) {

	t_finalizar_pedido* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

t_terminar_pedido* recibir_terminar_pedido(int socketEmisor, t_log* logger) {

	t_terminar_pedido* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

char* recibir_obtener_receta(int socketEmisor, t_log* logger) {

	char* recibido = recibirMensaje(socketEmisor, logger);
	return recibido;
}

/************RECIBIR RESPUESTAS************/

t_rta_consultar_restaurantes* recibir_rta_consultar_restaurantes(int socketEmisor, t_log* logger) {

	void* serializado = recibirMensaje(socketEmisor, logger);

	return deserializar_rta_consultar_restaurantes(serializado);
}

t_rta_obtener_restaurante* recibir_rta_obtener_restaurante(int socketEmisor, t_log* logger) {

	void* serializado = recibirMensaje(socketEmisor, logger);

	return deserializar_rta_obtener_restaurante(serializado);
}

t_rta_consultar_platos* recibir_rta_consultar_platos(int socketEmisor, t_log* logger) {

	void* serializado = recibirMensaje(socketEmisor, logger);

	return deserializar_rta_consultar_platos(serializado);
}

t_rta_consultar_pedido* recibir_rta_consultar_pedido(int socketEmisor, t_log* logger) {

	void* serializado = recibirMensaje(socketEmisor, logger);

	return deserializar_rta_consultar_pedido(serializado);
}

t_rta_obtener_pedido* recibir_rta_obtener_pedido(int socketEmisor, t_log* logger) {

	void* serializado = recibirMensaje(socketEmisor, logger);

	return deserializar_rta_obtener_pedido(serializado);
}

t_rta_obtener_receta* recibir_rta_obtener_receta(int socketEmisor, t_log* logger) {

	void* serializado = recibirMensaje(socketEmisor, logger);

	return deserializar_rta_obtener_receta(serializado);
}
