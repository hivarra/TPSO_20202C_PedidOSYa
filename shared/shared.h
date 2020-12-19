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
//#include <stdbool.h>
//#include <stdarg.h>
#include <unistd.h>
//#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netdb.h>
//#include <sys/socket.h>
#include <pthread.h>
#include <libgen.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/error.h>
#include <errno.h>

/* ---------- Definición de tipos ---------- */
typedef enum{
	APP,
	CLIENTE,
	COMANDA,
	RESTAURANTE,
	SINDICATO
} t_tipoProceso;

typedef enum{
	HANDSHAKE_INICIAL,
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
	RTA_HANDSHAKE,
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
}t_tipoMensaje;

/* ---------- Logger ---------- */
//t_log* configurar_logger(char* nombreLog, char* nombreProceso);
void destruir_logger(t_log* logger);

/* ---------- Configuración ---------- */
void destruir_config(t_config* config);

/* ---------- Conexiónes ---------- */
int crear_conexion(char*, char*);//Recibe char* ip, char* puerto, y se conecta al servidor devolviendo el socket creado, si no se conecta devuelve -1

//int definirSocket(t_log* logger);
//int bindearSocketYEscuchar(int socket, char *ip, int puerto, t_log* logger);
//int aceptarConexiones(int socket, t_log* logger);
//int conectar_a_servidor(char* ip, int puerto, t_log* logger);
//int conectarseAServidor(int socket, char* ip, int puerto, t_log* logger);

/* ---------- Tipos mensajes y procesos ---------- */
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
int crear_carpeta_log(char* path_log);//recibe el path completo del log y crea una carpeta para poder guardar el log. Devuelve 1 si se creo o si estaba creada, 0 si no se creo

#endif /* SHARED_H_ */
