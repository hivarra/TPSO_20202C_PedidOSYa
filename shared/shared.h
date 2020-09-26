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
#include <sys/time.h>
#include <netdb.h>
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
	APP,
	CLIENTE,
	COMANDA,
	RESTAURANTE,
	SINDICATO
} t_tipoProceso;

typedef enum{
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
	RESPUESTA_OK_FAIL,
	SOCKET_ESCUCHA,//FALTA IMPLEMENTAR
	SOCKET_ENVIO//FALTA IMPLEMENTAR
}t_tipoMensaje;

typedef enum{
	RTA_CONSULTAR_RESTAURANTES,
	RTA_SELECCIONAR_RESTAURANTE,
	RTA_OBTENER_RESTAURANTE,
	RTA_CONSULTAR_PLATOS,
	RTA_CREAR_PEDIDO,
	RTA_GUARDAR_PEDIDO,
	RTA_ANADIR_PLATO,
	RTA_GUARDAR_PLATO,
	RTA_CONFIRMAR_PEDIDO,
	RTA_PLATO_LISTO,
	RTA_CONSULTAR_PEDIDO,
	RTA_OBTENER_PEDIDO,
	RTA_FINALIZAR_PEDIDO,
	RTA_TERMINAR_PEDIDO,
	RTA_OBTENER_RECETA
}t_tipoRespuesta;


//typedef enum tipoMensaje {
//	HANDSHAKE,
//	CONSULTAR_RESTAURANTES,
//	SELECCIONAR_RESTAURANTE,
//	OBTENER_RESTAURANTE,
//	CONSULTAR_PLATOS,
//	CREAR_PEDIDO,
//	GUARDAR_PEDIDO,
//	ANADIR_PLATO,
//	GUARDAR_PLATO,
//	CONFIRMAR_PEDIDO,
//	PLATO_LISTO,
//	CONSULTAR_PEDIDO,
//	OBTENER_PEDIDO,
//	FINALIZAR_PEDIDO,
//	TERMINAR_PEDIDO,
//	OBTENER_RECETA,
//	RESULTADO_GUARDAR,
//	RESPUESTA_OK_FAIL,
//	CLIENTE_RECIBE_INFO,
//	SALIR
//	// Agregar los que falten
//	// Para la consola
//}t_tipoMensaje;

//typedef struct {
//	t_tipoProceso 	tipoProceso;
//	uint32_t			idProceso;
//	t_tipoMensaje 	tipoMensaje;
//	uint32_t 		longitud;
//}__attribute__((packed)) t_header;
//
//
//typedef struct {
//	t_header 		header;
//	void* 			content;
//}__attribute__((packed)) t_mensaje;

/*Protocolos*/

//typedef struct {
//	char nombre_restaurante[32];
//	uint32_t id_pedido;
//}__attribute__((packed)) t_guardar_pedido;
//
//typedef struct {
//	char nombre_restaurante[32];
//	uint32_t id_pedido;
//	char comida[32];
//	uint32_t cantidad_comida;
//}__attribute__((packed)) t_guardar_plato;
//
//typedef struct {
//	uint32_t id_pedido;
//}__attribute__((packed)) t_confirmar_pedido;
//
//typedef struct {
//	char nombre_restaurante[32];
//	uint32_t id_pedido;
//	char* comida[32];
//}__attribute__((packed)) t_plato_listo;
//
//typedef struct {
//	char nombre_restaurante[32];
//	uint32_t id_pedido;
//}__attribute__((packed)) t_finalizar_pedido;
//
//typedef struct {
//	char nombre_restaurante[32];
//	uint32_t id_pedido;
//}__attribute__((packed)) t_obtener_pedido;
//
//typedef struct {
//	uint32_t respuesta_ok_fail;
//}__attribute__((packed)) t_respuesta_ok_fail;
//
//typedef struct {
//	char nombre[100];
//	uint32_t posX;
//	uint32_t posY;
//}__attribute__((packed)) t_restaurante;

/*----Tipos Restaurante -- */

typedef struct {
	uint32_t id;
	char  afinidad[100];//Milanesas
}__attribute__((packed)) t_cocinero;

typedef struct {
	uint32_t id;
	uint32_t id_Cliente;
	char* platos[32];//[Milanesas, Pastas, Ensalada]
	uint32_t cant_Platos[32];//[3, 5, 4]
	uint32_t precioTotal;// 300
}__attribute__((packed)) t_pedido;

typedef struct {
	uint32_t id;
	char* nombre[32];//Milanesas
	uint32_t precio;// 30
}__attribute__((packed)) t_plato;

//typedef struct {
//	uint32_t id;
//	char nombre[32];//Milanesas
//	char* pasos[32];//[Trocear, Empanar, Reposar, Hornear]
//	int  tiempos_pasos[32];//[4, 5, 3, 10]
//}__attribute__((packed)) t_receta;

/* ---------- Logger ---------- */
t_log* configurar_logger(char* nombreLog, char* nombreProceso);
void destruir_logger(t_log* logger);

/* ---------- Configuración ---------- */
void destruir_config(t_config* config);

/* ---------- Conexiónes ---------- */
int crear_conexion(char*, char*);//Recibe char* ip, char* puerto, y se conecta al servidor devolviendo el socket creado, si no se conecta devuelve -1

int definirSocket(t_log* logger);
int bindearSocketYEscuchar(int socket, char *ip, int puerto, t_log* logger);
int aceptarConexiones(int socket, t_log* logger);
int conectar_a_servidor(char* ip, int puerto, int id_proceso, int tipoProcesoEmisor, int tipoProcesoReceptor, t_log* logger);
int conectarseAServidor(int socket, char* ip, int puerto, t_log* logger);

/* ---------- Exit ---------- */
void exit_gracefully(int return_nr, t_log* logger);
void _exit_with_error(char* error_msg, void * buffer, t_log* logger);

/* ---------- Tipos mensajes y procesos ---------- */
t_tipoProceso tipo_proceso_string_to_enum(char*);//Recibe en string el tipo de proceso y lo devuelve en t_tipoProceso(Int)
t_tipoMensaje tipo_mensaje_string_to_enum(char*);//Recibe en string el tipo de msj y lo devuelve en t_tipoMensaje(Int)

char* get_nombre_proceso(int);//Recibe en ENUM el nombre del proceso y lo devuelve en string
char* get_nombre_mensaje(int);//Recibe en ENUM el nombre del msj y lo devuelve en string

/* ---------- Paths ---------- */
char* getCurrentPath(void);//ATENCION: Liberar el string devuelto
char* getParentPath(void);//ATENCION: Liberar el string devuelto
char* getConfigPath(char*);//ATENCION: Liberar el string devuelto. Recibe un "config.cfg" y devuelve el path completo "user.../config/config.cfg"

/* ---------- Extras ---------- */
uint64_t timestamp(void);
void liberar_lista(char**);//Recibe una lista de strings, generalmente devuelta por las commons, y libera la memoria usada


/**************************************************/
/* ---------- Posiblemente no usados ---------- */
void loggear(t_log* logger, t_log_level level, const char* message_format, ...) ;
t_config* cargarConfiguracion(char *nombreArchivo, t_log* logger);
void eliminar_blancos(char* text);
void imprimir_lista_strings(t_list* lista, char* nombre_lista);
char* getLogPath(char* nombre_archivo);


#endif /* SHARED_H_ */
