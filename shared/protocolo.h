/*
 * protocolo.h
 *
 *  Created on: 17 sep. 2020
 *      Author: utnso
 */

#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#define L_STRING 32

#include "shared.h"

typedef enum{
	PENDIENTE,
	CONFIRMADO,
	TERMINADO
}t_estado_Pedido;

typedef enum{
	FAIL,
	OK
}t_resultado;

typedef struct
{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct
{
	uint32_t tipoMensaje;
	t_buffer* buffer;
} t_paquete;

typedef struct {
	char nombre[L_STRING];
	uint32_t tiempo;
}__attribute__((packed)) t_paso_receta;

typedef struct {
	char nombre[L_STRING];
	uint32_t precio;
	uint32_t cantPasos;
	t_list* pasos;
}t_receta;

typedef struct {
	char nombre[L_STRING];
	uint32_t cantTotal;
	uint32_t cantLista;
}__attribute__((packed)) t_comida;

/****************Protocolos****************/

/********Consultar Restaurantes********/
//Recibe: nada

//Retorna:
typedef struct {
	uint32_t cantRestaurantes;
	t_list* restaurantes;//char restaurante[L_STRING]
}t_rta_consultar_restaurantes;


/********Seleccionar Restaurante********/
//Recibe:
typedef struct {
	uint32_t cliente;
	char restarurante[L_STRING];
}__attribute__((packed)) t_seleccionar_restaurante;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Obtener Restaurante********/
//Recibe: char nombre_restaurante[L_STRING]

//Retorna:
typedef struct {
	uint32_t posX;
	uint32_t posY;
	uint32_t cantHornos;
	uint32_t cantCocineros;
	uint32_t cantAfinidades;
	t_list* afinidades_cocineros;//char comida[L_STRING]
	uint32_t cantRecetas;
	t_list* recetas;//t_receta
}t_rta_obtener_restaurante;


/********Consultar Platos********/
//Recibe: char nombre_restaurante[L_STRING]

//Retorna:
typedef struct {
	uint32_t cantComidas;
	t_list* comidas;//t_comida
}t_rta_consultar_platos;


/********Crear Pedido********/
//Recibe: nada (por ahora)

//Retorna: unit32_t id_pedido


/********Guardar Pedido********/
//Recibe:
typedef struct {
	char nombre_restaurante[L_STRING];
	uint32_t id_pedido;
}__attribute__((packed)) t_guardar_pedido;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Anadir Plato********/
//Recibe:
typedef struct {
	char plato[L_STRING];
	uint32_t id_pedido;
}__attribute__((packed)) t_anadir_plato;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Guardar Plato********/
//Recibe:
typedef struct {
	char nombre_restaurante[L_STRING];
	uint32_t id_pedido;
	char comida[L_STRING];
	uint32_t cantidad_comida;
}__attribute__((packed)) t_guardar_plato;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Confirmar Pedido********/
//Recibe: unit32_t id_pedido

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Plato Listo*******/
//Recibe:
typedef struct {
	char nombre_restaurante[L_STRING];
	uint32_t id_pedido;
	char comida[L_STRING];
}__attribute__((packed)) t_plato_listo;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Consultar Pedido********/
//Recibe: unit32_t id_pedido

//Retorna:
typedef struct {
	char nombre_restaurante[L_STRING];
	uint32_t estado;
	uint32_t cantComidas;
	t_list* comidas;//t_comida
}t_rta_consultar_pedido;


/********Obtener Pedido********/
//Recibe:
typedef struct {
	char nombre_restaurante[L_STRING];
	uint32_t id_pedido;
}__attribute__((packed)) t_obtener_pedido;

//Retorna:
typedef struct {
	uint32_t cantComidas;
	t_list* comidas;//t_comida
}t_rta_obtener_pedido;


/********Finalizar Pedido********/
//Recibe:
typedef struct {
	char nombre_restaurante[L_STRING];
	uint32_t id_pedido;
}__attribute__((packed)) t_finalizar_pedido;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Terminaar Pedido********/
//Recibe:
typedef struct {
	char nombre_restaurante[L_STRING];
	uint32_t id_pedido;
}__attribute__((packed)) t_terminar_pedido;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Obtener Receta********/
//Recibe: char nombre_plato[L_STRING];

//Retorna: t_receta
/* typedef struct {
	 char nombre[L_STRING];
	 uint32_t cantPasos;
	 t_list* pasos;
	 uint32_t precio;
 }t_receta;*/



int enviarMensaje(t_tipoMensaje, void* puntero_content, int fd_socketReceptor, t_log*);//Al crearse dos sockets por cada nueva conexion, y crear dos hilos por proceso conectado, los tipos de procesos y ids pueden enviarse solo en un struct del SOCKET_ESCUCHA O HANDSHAKE

int enviarRespuesta(t_tipoRespuesta, void* puntero_content, int fd_socketReceptor, t_log*);


void* recibirMensaje(int fd_socket_emisor, t_log*);//Recibe el socket emisor y un logger y retorna un void* que hay que castear de acuerdo al tipoMensaje recibido en el primer recv

void* recibirRespuesta(int fd_socketEmisor, t_tipoRespuesta tipoRespuesta_enum, t_log*);//Recibe el socket emisor, el tipo de respuesta recibido en el primer rcv  y un logger y retorna un void* que hay que castear de acuerdo al tipoMensaje recibido en el primer recv


#endif /* PROTOCOLO_H_ */
