#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "../sindicato.h"
#include <readline/readline.h>
#include <readline/history.h>

typedef enum t_tipoComando {
select_
} t_tipoComando;

void *crear_consola();
char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);
t_tipoComando buscar_enum_sfs(char *sval);



#endif /* CONSOLA_H_ */
