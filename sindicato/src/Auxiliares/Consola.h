#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <readline/readline.h>
#include "../sindicato.h"

pthread_t thread_consola;

void *crear_consola();

#endif /* CONSOLA_H_ */
