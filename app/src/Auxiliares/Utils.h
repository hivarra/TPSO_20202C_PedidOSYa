/*
 * Utils.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_UTILS_H_
#define AUXILIARES_UTILS_H_

#include "shared.h"
#include "Logueo.h"
#include "Configuracion.h"
#include <math.h>

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
	uint32_t restaurante_posX;
	uint32_t restaurante_posY;
	// TODO: Lo de abajo cambiarlo porque tiene que salir de otra estructura
	uint32_t cliente_posX;
	uint32_t cliente_posY;
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

t_list* repartidores;
t_list* pedidos_planificables;
t_list* listos;
t_list* ejecutando;
t_list* bloqueados;
t_list* finalizados;

void imprimirRepartidor(t_repartidor* repartidor);
void imprimirPCB(t_pcb* pcb);
void disponibilizar_repartidor(t_repartidor* repartidor);
t_repartidor* repartidor_mas_cercano(int posX, int poxY);
int distancia_a_posicion(t_repartidor* repartidor, int posX, int posY);
int distancia_absoluta(int pos1, int pos2);
void asignar_repartidor(t_repartidor* repartidor, t_pcb* pcb);
t_repartidor* obtener_repartidor(int id_repartidor);
char* get_nombre_instruccion(t_instruccion enum_instruccion);
void aplicar_retardo_operacion();
void ejecutarPCB(t_pcb* pcb);
void bloquearPCB(t_repartidor* repartidor);
void retirarPedido(t_pcb* pcb);
t_pcb* sacarPCBDeEjecutando(t_repartidor* repartidor);
void finalizarPCB(t_repartidor* repartidor);

#endif /* AUXILIARES_UTILS_H_ */
