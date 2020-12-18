/*
 * app.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef APP_H_
#define APP_H_

#include <shared.h>
#include <signal.h>
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
} t_restaurante;

pthread_t hilo_servidor;
pthread_t thread_PLP;
pthread_t thread_PCP;

void prueba_planificacion();

void inicializar();
void iniciarListas();
void iniciarSemaforos();
void iniciarRestauranteDefault();
void iniciarRepartidores();

void crear_hilo_PLP();
void crear_hilo_PCP();

void signalHandler(int);

#endif /* APP_H_ */
