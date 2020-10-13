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

typedef struct {
	uint32_t id;
	uint32_t posX;
	uint32_t posY;
	uint32_t frecuenciaDescanso;
	uint32_t tiempoDescanso;
}__attribute__((packed)) t_repartidor;

typedef struct {
	char* nombre;
	uint32_t posX;
	uint32_t posY;
}__attribute__((packed)) t_restaurante;

pthread_t hilo_servidor;
t_restaurante* restaurante_default;

void inicializar();
void iniciarListas();
void iniciarSemaforos();
void iniciarRestauranteDefault();
void iniciarRepartidores();
void imprimirRepartidor(t_repartidor* repartidor);

#endif /* APP_H_ */
