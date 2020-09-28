#include "Consola.h"
#include "commons/string.h"

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

		}
		return 0;
	}
}

char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}

char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "SELECT", "INSERT", "CREATE", "DESCRIBE",
			"DROP", "SALIR", NULL };

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
	t_tipoComando result = select_;
	int i = 0;
	char* comandos_str[] = { "SELECT",
	NULL };
	if (sval == NULL)
		return -2;
	for (i = 0; comandos_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, comandos_str[i]))
			return result;
	return -1;
}


