#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <Shared.h>
#include "Logueo.h"

typedef struct {
	char* punto_montaje_afip;
	char* ip_restaurante;
	int32_t puerto_restaurante;
	int32_t block_size;
	int32_t blocks;
	char* magic_number;
	int32_t id_proceso;
} t_sindicato_config;


t_sindicato_config sindicato_conf;
t_config *config;

void cargarConfigSindicato(char* path);

#endif /* CONFIGURACION_H_ */
