/*
 * Configuracion.h
 *
 *  Created on: 10 sep. 2020
 *      Author: utnso
 */

#ifndef AUXILIARES_CONFIGURACION_H_
#define AUXILIARES_CONFIGURACION_H_

#include <shared.h>

// Definición de tipos
typedef struct {
	char* ip_comanda;
	char* puerto_comanda;
	uint32_t retardo_ciclo_cpu;
	uint32_t grado_multiprocesamiento;
	char* algoritmo_planificacion;
	double alpha;
	double estimacion_inicial;
	char** repartidores;
	char** frecuencias_descanso;
	char** tiempos_descanso;
	char** platos_default;
	uint32_t pos_rest_default_x;
	uint32_t pos_rest_default_y;
} t_app_conf;

// Variables globales
t_config* config;
t_app_conf app_conf;

t_log* logger;

void cargar_configuracion_app(char *path_config);
void cargar_logger_app(void);
void mostrar_propiedades(void);

#endif /* AUXILIARES_CONFIGURACION_H_ */
