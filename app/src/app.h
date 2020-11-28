/*
 * app.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef APP_H_
#define APP_H_

#include <shared.h>
#include "Auxiliares/Logueo.h"
#include "Auxiliares/Configuracion.h"
#include "Auxiliares/Conexion.h"
#include "Auxiliares/Utils.h"
#include "Auxiliares/Planificador_LP.h"
#include "Auxiliares/Planificador_CP.h"
#include "Auxiliares/Repartidor.h"

typedef struct {
	char* nombre;
	uint32_t posX;
	uint32_t posY;
}__attribute__((packed)) t_restaurante;

//typedef struct {
//	uint32_t id_pedido;
//	uint32_t instruccion; // esto es para saber si tiene que ir a buscar el pedido o entregarlo
//	uint32_t id_repartidor;
//	uint32_t posX;
//	uint32_t posY;
//} t_pcb;


pthread_t hilo_servidor;
pthread_t thread_PLP;
pthread_t thread_PCP;

t_restaurante* restaurante_default;

void prueba_planificacion();

void inicializar();
void iniciarListas();
void iniciarSemaforos();
void iniciarRestauranteDefault();
void iniciarRepartidores();
//void imprimirRepartidor(t_repartidor* repartidor);
//void imprimirPCB(t_pcb* pcb);

void crear_hilo_PLP();
void crear_hilo_PCP();

#endif /* APP_H_ */
