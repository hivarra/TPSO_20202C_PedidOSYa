#include "Consola.h"
#include "commons/string.h"

int validar_crear_restaurante(char** comandos_crear_restaurante){
	return comandos_crear_restaurante[1] != NULL && comandos_crear_restaurante[2] != NULL &&  comandos_crear_restaurante[3] != NULL &&
			comandos_crear_restaurante[4] != NULL && comandos_crear_restaurante[5] != NULL && comandos_crear_restaurante[6] != NULL &&
			comandos_crear_restaurante[7] != NULL;
}
int validar_crear_receta(char** comandos_crear_receta){
	return comandos_crear_receta[1] != NULL && comandos_crear_receta[2] != NULL &&  comandos_crear_receta[3] != NULL;
}
void *crear_consola() {

	char *line;
	rl_attempted_completion_function = character_name_completion;

	while (1) {
		int i = 0;
		line = readline("Ingrese un comando> ");
		if (line) {
			add_history(line);
		}

		char *lineaCompleta = string_new();
		string_append(&lineaCompleta, line);


		char** comando = string_split(line, " ");
		free(line);

		t_tipoComando comando_e = buscar_enum_sfs(comando[0]);

		if (comando_e == -2) {
			for (i = 0; comando[i] != NULL; i++)
				free(comando[i]);
			if (comando)
				free(comando);
			continue;
		}

		switch (comando_e) {

		case crearRestaurante_:;
			printf("Se ingresó comando CrearRestaurante \n");
			if(!validar_crear_restaurante(comando))
				printf("Faltan ingresar datos para la creación del restaurante\n");
			else{
				t_crear_restaurante* argsCrearRestaurante = malloc(sizeof(t_crear_restaurante));
				argsCrearRestaurante->nombreRestaurante = comando[1];
				argsCrearRestaurante->cantidadCocineros = comando[2];
				argsCrearRestaurante->posicion=comando[3];
				argsCrearRestaurante->afinidadCocineros=comando[4];
				argsCrearRestaurante->platos=comando[5];
				argsCrearRestaurante->preciosPlatos=comando[6];
				argsCrearRestaurante->cantidadHornos=comando[7];
				crearRestaurante(argsCrearRestaurante);
				free(argsCrearRestaurante);
				printf("Se finalizó el comando CrearRestaurante \n");
			}

			break;

		case crearReceta_:;
			printf("Se ingresó comando CrearReceta \n");
			if(!validar_crear_receta(comando))
				printf("Faltan ingresar datos para la creación de la receta\n");
			else{
				t_crear_receta* argsCrearReceta = malloc(sizeof(t_crear_receta));
				argsCrearReceta->nombre = comando[1];
				argsCrearReceta->pasos = comando[2];
				argsCrearReceta->tiempoPasos = comando[3];
				crearReceta(argsCrearReceta);
				free(argsCrearReceta);
				printf("Se finalizó el comando CrearReceta \n");
			}
			break;

		default:

			printf("No se reconoce el comando %s .\n", comando[0]);
		}
		for(i=0;comando[i]!=NULL;i++)
			free(comando[i]);
		if(comando) free(comando);

		free(lineaCompleta);
	}
	return 0;
}

char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}

char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "CREAR_RESTAURANTE", "CREAR_RECETA", NULL };

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((name = character_names[list_index++])) {
		if (strncmp(name, text, len) == 0) {
			return strdup(name);
		}
	}
	return NULL;
}

t_tipoComando buscar_enum_sfs(char *sval) {
	t_tipoComando result = crearRestaurante_;
	int i = 0;
	char* comandos_str[] = { "CrearRestaurante", "CrearReceta", NULL };
	if (sval == NULL)
		return -2;
	for (i = 0; comandos_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, comandos_str[i]))
			return result;
	return -1;
}
