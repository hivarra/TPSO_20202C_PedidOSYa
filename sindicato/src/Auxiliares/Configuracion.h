#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <shared.h>
#include "Logueo.h"

typedef struct {
	char* punto_montaje;
	uint32_t puerto_escucha;
	uint32_t block_size;
	uint32_t blocks;
	char* magic_number;
} t_sindicato_config;


t_sindicato_config sindicato_conf;
t_config *config;

void cargarConfigSindicato(char* path);
void mostrar_propiedades();


#endif /* CONFIGURACION_H_ */
