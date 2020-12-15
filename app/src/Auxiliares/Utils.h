/*
 * Utils.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_UTILS_H_
#define AUXILIARES_UTILS_H_

#include "shared.h"
#include "Configuracion.h"
#include <math.h>
#include <protocolo.h>

typedef struct{
	char id[L_ID];
	char restaurante_seleccionado[L_ID];
	int socketEscucha;
	int pos_x;
	int pos_y;
	t_list* pedidos;//Pedidos que realiza un cliente
}t_info_cliente;

typedef struct{
	char id[L_ID];
	int socketEscucha;
	int socketEnvio;
	int pos_x;
	int pos_y;
}t_info_restaurante;

typedef enum estado {
	BUSCAR_PEDIDO
	, ESPERAR_PEDIDO
	, ENTREGAR_PEDIDO
	, DESCANSAR
} t_instruccion;

typedef struct {
	uint32_t id;
	uint32_t posX;
	uint32_t posY;
	uint32_t frecuenciaDescanso;
	uint32_t tiempoDescanso;
	uint32_t disponible;
	t_instruccion instruccion;
	sem_t sem_moverse;
	uint32_t objetivo_posX;
	uint32_t objetivo_posY;
	uint32_t quantum;
} t_repartidor;

typedef struct {
	uint32_t id_pedido;
	uint32_t instruccion; // esto es para saber si tiene que ir a buscar el pedido o entregarlo
	uint32_t id_repartidor;
	char restaurante[L_ID];
	uint32_t restaurante_posX;
	uint32_t restaurante_posY;
	uint32_t cliente_posX;
	uint32_t cliente_posY;
	sem_t sem_pedido_listo;
	double ultima_estimacion;
	uint32_t tiempo_espera_ready;
} t_pcb;

sem_t
	sem_pedidos
	, sem_repartidor_disponible
	, sem_ready
	, sem_limite_exec;

pthread_mutex_t
	mutex_nuevos
	, mutex_listos
	, mutex_ejecutando
	, mutex_bloqueados
	, mutex_finalizados;

pthread_mutex_t mutexRestaurantes,
				mutexClientes,
				mutex_id_rest_default,
				mutexRepartidores;

t_list* repartidores;
t_list* pedidos_planificables;
t_list* listos;
t_list* ejecutando;
t_list* bloqueados;
t_list* finalizados;

t_list* restaurantesConectados;
t_list* clientesConectados;
t_info_restaurante* infoRestoDefault;

t_info_cliente* buscarClienteConectado(char* id);
t_info_cliente* buscarClientePorPedidoYnombreRestaurante(int id_pedido,char* nombre_restaurante);//FIX GONZA: TOMABA EL PRIMER PEDIDO,PERO SE PUEDE REPETIR EL ID_PEDIDO PARA OTRO RESTAURANTE
t_info_restaurante* buscarRestauranteConectado(char* nombre_restaurante);
t_rta_consultar_restaurantes* obtenerRestaurantes();

void imprimirRepartidor(t_repartidor* repartidor);
void imprimirPCB(t_pcb* pcb);
void crearPCB(t_info_cliente* cliente, int id_pedido);
void disponibilizar_repartidor(t_repartidor* repartidor);
t_repartidor* repartidor_mas_cercano(int posX, int poxY);
int distancia_a_posicion(t_repartidor* repartidor, int posX, int posY);
int distancia_absoluta(int pos1, int pos2);
void asignar_repartidor(t_repartidor* repartidor, t_pcb* pcb);
t_repartidor* obtener_repartidor(int id_repartidor);
char* get_nombre_instruccion(t_instruccion enum_instruccion);
void aplicar_retardo_operacion();
void ejecutarPCB(t_pcb* pcb);
void bloquearPCB(t_repartidor* repartidor, t_instruccion instruccion_anterior);
void retirarPedido(t_pcb* pcb);
t_pcb* sacarPCBDeEjecutando(t_repartidor* repartidor);
void finalizarPCB(t_repartidor* repartidor);
void liberarPCB(t_pcb* pcb_a_liberar);
void notificar_pedido_listo(int id_pedido,char* nombre_restaurante);
t_pcb* buscarPCB(int id_pedido,char* nombre_restaurante);
int conectar_a_comanda_simple();

#endif /* AUXILIARES_UTILS_H_ */
