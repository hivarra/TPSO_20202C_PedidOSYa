#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <shared.h>

t_config *config;
t_log* logger;

void cargarConfigSindicato(char*);
void cargar_logger_sindicato(void);
void mostrar_propiedades(void);


#endif /* CONFIGURACION_H_ */
