/*
 * protocolo.h
 *
 *  Created on: 17 sep. 2020
 *      Author: utnso
 */

#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#define L_ID 32
#define L_REST 32
#define L_PLATO 32
#define L_PASO_REC 32
//#define L_STRING 32

#include "shared.h"

typedef enum{
	PENDIENTE,
	CONFIRMADO,
	TERMINADO
}t_estado_pedido;

typedef enum{
	FAIL,
	OK
}t_resultado;

typedef struct{
	uint32_t size;
	void* stream;
}t_buffer;

typedef struct{
	uint32_t tipoMensaje;
	t_buffer* buffer;
}t_paquete;

typedef struct{
	char nombre[L_PASO_REC];
	uint32_t tiempo;
}__attribute__((packed)) t_paso_receta;

typedef struct{
	char nombre[L_PLATO];
	uint32_t precio;
}t_plato;

typedef struct{
	char nombre[L_PLATO];
	uint32_t cantTotal;
	uint32_t cantLista;
}__attribute__((packed)) t_comida;

/****************Protocolos****************/

/********Socket envio********///SE USA DESDE CLIENTE HACIA APP, O DESDE RESTAURANTE HACIA APP
//Recibe:
typedef struct{
	char id[L_ID];
	uint32_t posX;
	uint32_t posY;
	uint32_t tipoProceso;
}t_socket_envio;

/********Socket escucha********///SE USA DESDE CLIENTE HACIA APP, O DESDE RESTAURANTE HACIA APP
//Recibe:
typedef struct{
	char id[L_ID];
	uint32_t tipoProceso;
}t_socket_escucha;


/********Consultar Restaurantes********/
//Recibe: nada

//Retorna:
typedef struct{
	uint32_t cantRestaurantes;
	t_list* restaurantes;//char restaurante[L_REST]
}t_rta_consultar_restaurantes;


/********Seleccionar Restaurante********/
//Recibe:
typedef struct{
	char cliente[L_ID];//No tiene utilidad este parametro
	char restarurante[L_REST];
}__attribute__((packed)) t_seleccionar_restaurante;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Obtener Restaurante********/
//Recibe: char nombre_restaurante[L_REST]

//Retorna:
typedef struct{
	uint32_t posX;
	uint32_t posY;
	uint32_t cantHornos;
	uint32_t cantCocineros;
	t_list* cocineros;//char comida[L_PLATO](sin afinidad: "N")
	uint32_t cantPlatos;
	t_list* platos;//t_plato
}t_rta_obtener_restaurante;


/********Consultar Platos********/
//Recibe: char nombre_restaurante[L_REST]

//Retorna:
typedef struct{
	uint32_t cantPlatos;
	t_list* platos;//t_plato
}t_rta_consultar_platos;


/********Crear Pedido********/
//Recibe: nada

//Retorna: unit32_t id_pedido


/********Guardar Pedido********/
//Recibe:
typedef struct{
	char restaurante[L_REST];
	uint32_t id_pedido;
}__attribute__((packed)) t_guardar_pedido;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Anadir Plato********/
//Recibe:
typedef struct{
	char plato[L_PLATO];
	uint32_t id_pedido;
}__attribute__((packed)) t_anadir_plato;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Guardar Plato********/
//Recibe:
typedef struct{
	char restaurante[L_REST];
	uint32_t id_pedido;
	char plato[L_PLATO];
	uint32_t cantPlato;
}__attribute__((packed)) t_guardar_plato;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Confirmar Pedido********/
//Recibe:
typedef struct{
	char restaurante[L_REST];
	uint32_t id_pedido;
}__attribute__((packed)) t_confirmar_pedido;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Plato Listo*******/
//Recibe:
typedef struct{
	char restaurante[L_REST];
	uint32_t id_pedido;
	char plato[L_PLATO];
}__attribute__((packed)) t_plato_listo;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Consultar Pedido********/
//Recibe: unit32_t id_pedido

//Retorna:
typedef struct{
	char restaurante[L_REST];
	uint32_t estado;
	uint32_t cantComidas;
	t_list* comidas;//t_comida
}t_rta_consultar_pedido;


/********Obtener Pedido********/
//Recibe:
typedef struct{
	char restaurante[L_REST];
	uint32_t id_pedido;
}__attribute__((packed)) t_obtener_pedido;

//Retorna:
typedef struct{
	uint32_t estado;//t_estado_pedido
	uint32_t cantComidas;
	t_list* comidas;//t_comida
}t_rta_obtener_pedido;


/********Finalizar Pedido********/
//Recibe:
typedef struct{
	char restaurante[L_REST];
	uint32_t id_pedido;
}__attribute__((packed)) t_finalizar_pedido;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Terminaar Pedido********/
//Recibe:
typedef struct{
	char nombre_restaurante[L_REST];
	uint32_t id_pedido;
}__attribute__((packed)) t_terminar_pedido;

//Retorna: uint32_t(0 = FAIL, 1 = OK)


/********Obtener Receta********/
//Recibe: char nombre_plato[L_PLATO];

//Retorna:
typedef struct{
	char nombre[L_PLATO];
	uint32_t cantPasos;
	t_list* pasos;//t_paso_receta
}t_rta_obtener_receta;


t_tipoMensaje recibir_tipo_mensaje(int fd_socket_emisor, t_log*);//DEVUELVE EL TIPO DE MENSAJE O -1 CASO DE ERROR

int enviar_handshake(int socketReceptor, t_log* logger);//envia solamente el tipo de mensaje (HANDSHAKE)
void* recibir_handshake(int socketEmisor, t_log* logger);//recibe NULL (no hace falta hacerle free)

int enviar_socket_envio(t_socket_envio* mensaje, int socketReceptor, t_log* logger);
t_socket_envio* recibir_socket_envio(int socketEmisor, t_log* logger);

int enviar_socket_escucha(t_socket_escucha* mensaje, int socketReceptor, t_log* logger);
t_socket_escucha* recibir_socket_escucha(int socketEmisor, t_log* logger);

int enviar_consultar_restaurantes(int socketReceptor, t_log* logger);
int enviar_rta_consultar_restaurantes(t_rta_consultar_restaurantes* mensaje, int socketReceptor, t_log* logger);
void* recibir_consultar_restaurantes(int socketEmisor, t_log* logger);//recibe NULL (no hace falta hacerle free)
t_rta_consultar_restaurantes* recibir_rta_consultar_restaurantes(int socketEmisor, t_log* logger);

int enviar_seleccionar_restaurante(t_seleccionar_restaurante* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_seleccionar_restaurante(uint32_t resultado, int socketReceptor, t_log* logger);
t_seleccionar_restaurante* recibir_seleccionar_restaurante(int socketEmisor, t_log* logger);
uint32_t recibir_rta_seleccionar_restaurante(int socketEmisor, t_log* logger);

int enviar_obtener_restaurante(char* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_obtener_restaurante(t_rta_obtener_restaurante* mensaje, int socketReceptor, t_log* logger);
char* recibir_obtener_restaurante(int socketEmisor, t_log* logger);
t_rta_obtener_restaurante* recibir_rta_obtener_restaurante(int socketEmisor, t_log* logger);

int enviar_consultar_platos(char* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_consultar_platos(t_rta_consultar_platos* mensaje, int socketReceptor, t_log* logger);
char* recibir_consultar_platos(int socketEmisor, t_log* logger);
t_rta_consultar_platos* recibir_rta_consultar_platos(int socketEmisor, t_log* logger);

int enviar_crear_pedido(int socketReceptor, t_log* logger);
int enviar_rta_crear_pedido(uint32_t id_pedido,int socketReceptor, t_log* logger);
void* recibir_crear_pedido(int socketEmisor, t_log* logger);//recibe NULL (no hace falta hacerle free)
uint32_t recibir_rta_crear_pedido(int socketEmisor, t_log* logger);

int enviar_guardar_pedido(t_guardar_pedido* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_guardar_pedido(uint32_t resultado, int socketReceptor, t_log* logger);
t_guardar_pedido* recibir_guardar_pedido(int socketEmisor, t_log* logger);
uint32_t recibir_rta_guardar_pedido(int socketEmisor, t_log* logger);

int enviar_anadir_plato(t_anadir_plato* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_anadir_plato(uint32_t resultado, int socketReceptor, t_log* logger);
t_anadir_plato* recibir_anadir_plato(int socketEmisor, t_log* logger);
uint32_t recibir_rta_anadir_plato(int socketEmisor, t_log* logger);

int enviar_guardar_plato(t_guardar_plato* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_guardar_plato(uint32_t resultado, int socketReceptor, t_log* logger);
t_guardar_plato* recibir_guardar_plato(int socketEmisor, t_log* logger);
uint32_t recibir_rta_guardar_plato(int socketEmisor, t_log* logger);

int enviar_confirmar_pedido(t_confirmar_pedido* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_confirmar_pedido(uint32_t resultado, int socketReceptor, t_log* logger);
t_confirmar_pedido* recibir_confirmar_pedido(int socketEmisor, t_log* logger);
uint32_t recibir_rta_confirmar_pedido(int socketEmisor, t_log* logger);

int enviar_plato_listo(t_plato_listo* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_plato_listo(uint32_t resultado, int socketReceptor, t_log* logger);
t_plato_listo* recibir_plato_listo(int socketEmisor, t_log* logger);
uint32_t recibir_rta_plato_listo(int socketEmisor, t_log* logger);

int enviar_consultar_pedido(uint32_t mensaje, int socketReceptor, t_log* logger);
int enviar_rta_consultar_pedido(t_rta_consultar_pedido* mensaje, int socketReceptor, t_log* logger);
uint32_t recibir_consultar_pedido(int socketEmisor, t_log* logger);
t_rta_consultar_pedido* recibir_rta_consultar_pedido(int socketEmisor, t_log* logger);

int enviar_obtener_pedido(t_obtener_pedido* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_obtener_pedido(t_rta_obtener_pedido* mensaje, int socketReceptor, t_log* logger);
t_obtener_pedido* recibir_obtener_pedido(int socketEmisor, t_log* logger);
t_rta_obtener_pedido* recibir_rta_obtener_pedido(int socketEmisor, t_log* logger);

int enviar_finalizar_pedido(t_finalizar_pedido* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_finalizar_pedido(uint32_t resultado, int socketReceptor, t_log* logger);
t_finalizar_pedido* recibir_finalizar_pedido(int socketEmisor, t_log* logger);
uint32_t recibir_rta_finalizar_pedido(int socketEmisor, t_log* logger);

int enviar_terminar_pedido(t_terminar_pedido* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_terminar_pedido(uint32_t resultado, int socketReceptor, t_log* logger);
t_terminar_pedido* recibir_terminar_pedido(int socketEmisor, t_log* logger);
uint32_t recibir_rta_terminar_pedido(int socketEmisor, t_log* logger);

int enviar_obtener_receta(char* mensaje, int socketReceptor, t_log* logger);
int enviar_rta_obtener_receta(t_rta_obtener_receta* mensaje, int socketReceptor, t_log* logger);
char* recibir_obtener_receta(int socketEmisor, t_log* logger);
t_rta_obtener_receta* recibir_rta_obtener_receta(int socketEmisor, t_log* logger);

#endif /* PROTOCOLO_H_ */
