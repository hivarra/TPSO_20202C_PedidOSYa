/*
 * shared.h
 *
 *  Created on: 31 ago. 2020
 *      Author: utnso
 */
#ifndef SHARED_H_
#define SHARED_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <libgen.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/error.h>

/* ---------- Definición de tipos ---------- */
typedef enum tipoProceso {
	APP, CLIENTE, COMANDA, RESTAURANTE, SINDICATO
} t_tipoProceso;

typedef enum tipoMensaje {
	HANDSHAKE,
	CONSULTAR_RESTAURANTES,
	SELECCIONAR_RESTAURANTE,
	OBTENER_RESTAURANTE,
	CONSULTAR_PLATOS,
	CREAR_PEDIDO,
	GUARDAR_PEDIDO,
	ANADIR_PLATO,
	GUARDAR_PLATO,
	CONFIRMAR_PEDIDO,
	PLATO_LISTO,
	CONSULTAR_PEDIDO,
	OBTENER_PEDIDO,
	FINALIZAR_PEDIDO,
	TERMINAR_PEDIDO,
	OBTENER_RECETA,
	RESULTADO_GUARDAR,
	RESPUESTA_OK_FAIL,
	CLIENTE_RECIBE_INFO,
	SALIR
	// Agregar los que falten
	// Para la consola
}t_tipoMensaje;

typedef struct {
	t_tipoProceso 	tipoProceso;
	uint32_t			idProceso;
	t_tipoMensaje 	tipoMensaje;
	uint32_t 		longitud;
}__attribute__((packed)) t_header;


typedef struct {
	t_header 		header;
	void* 			content;
}__attribute__((packed)) t_mensaje;

/*Protocolos*/

typedef struct {
	char nombre_restaurante[32];
	uint32_t id_pedido;
}__attribute__((packed)) t_guardar_pedido;

typedef struct {
	char nombre_restaurante[32];
	uint32_t id_pedido;
	char comida[32];
	uint32_t cantidad_comida;
}__attribute__((packed)) t_guardar_plato;

typedef struct {
	uint32_t id_pedido;
}__attribute__((packed)) t_confirmar_pedido;

typedef struct {
	char nombre_restaurante[32];
	uint32_t id_pedido;
	char* comida[32];
}__attribute__((packed)) t_plato_listo;

typedef struct {
	char nombre_restaurante[32];
	uint32_t id_pedido;
}__attribute__((packed)) t_finalizar_pedido;

typedef struct {
	char nombre_restaurante[32];
	uint32_t id_pedido;
}__attribute__((packed)) t_obtener_pedido;

typedef struct {
	uint32_t respuesta_ok_fail;
}__attribute__((packed)) t_respuesta_ok_fail;

char* get_nombre_proceso(int enum_proceso);
char* get_nombre_mensaje(int enum_mensaje);

/* ---------- Logger ---------- */
t_log* configurar_logger(char* nombreLog, char* nombreProceso);
void loggear(t_log* logger, t_log_level level, const char* message_format, ...) ;
void destruir_logger(t_log* logger);
/* ---------- Configuración ---------- */
t_config* cargarConfiguracion(char *nombreArchivo, t_log* logger);
void destruir_config(t_config* config);

/* ---------- Conexión ---------- */
int definirSocket(t_log* logger);
int bindearSocketYEscuchar(int socket, char *ip, int puerto, t_log* logger);
int aceptarConexiones(int socket, t_log* logger);
int conectar_a_servidor(char* ip, int puerto, int tipoProcesoEmisor, int tipoProcesoReceptor, t_log* logger);
int conectarseAServidor(int socket, char* ip, int puerto, t_log* logger);
int enviarMensaje(int tipoProcesoEmisor, int tipoMensaje, int len, void* content,
		int socketReceptor, int tipoProcesoReceptor, t_log* logger);
t_mensaje* recibirMensaje(int socketEmisor, t_log* logger);
void* serializar(int tipoProceso, int tipoMensaje, int len, void* content);
t_mensaje* deserializar(void* buffer);
void destruirMensaje(t_mensaje* msg);

/* ---------- Error ---------- */
void exit_gracefully(int return_nr, t_log* logger);
void _exit_with_error(char* error_msg, void * buffer, t_log* logger);

t_tipoProceso tipo_proceso_string_to_enum(char *sval);
t_tipoMensaje tipo_mensaje_string_to_enum(char *sval);
void eliminar_blancos(char* text);
void imprimir_lista_strings(t_list* lista, char* nombre_lista);

char* getCurrentPath();
char* getParentPath();
char* getConfigPath(char* nombre_archivo);
char* getLogPath(char* nombre_archivo);

#endif /* SHARED_H_ */
