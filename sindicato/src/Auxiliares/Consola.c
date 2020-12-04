#include "Consola.h"

typedef enum t_tipoComando {
	crearRestaurante_,
	crearReceta_,
	exit_
} t_tipoComando;

typedef struct{
	char* nombreRestaurante;
	char* cantidadCocineros;
	char* posicion;
	char* afinidadCocineros;
	char* platos;
	char* preciosPlatos;
	char* cantidadHornos;
}t_crear_restaurante;

typedef struct{
	char* nombre;
	char* pasos;
	char* tiempoPasos;
}t_crear_receta;

t_tipoComando buscar_enum_sfs(char*);
int validar_crear_restaurante(char**);
int validar_crear_receta(char**);
int crearRestaurante(t_crear_restaurante* args);
int crearReceta(t_crear_receta* args);

void crear_consola() {

	char *line;
	int consolaActiva = 1;

	while (consolaActiva) {

		line = readline("Ingrese un comando> ");

		char** comando = string_split(line, " ");
		free(line);

		t_tipoComando comando_e = buscar_enum_sfs(comando[0]);

		switch (comando_e) {

		case crearRestaurante_:;
			log_trace(logger, "Se ingresó comando CrearRestaurante.");
			if(!validar_crear_restaurante(comando))
				puts("Faltan ingresar datos para la creación del restaurante.");
			else{
				t_crear_restaurante* argsCrearRestaurante = malloc(sizeof(t_crear_restaurante));
				argsCrearRestaurante->nombreRestaurante = comando[1];
				argsCrearRestaurante->cantidadCocineros = comando[2];
				argsCrearRestaurante->posicion=comando[3];
				argsCrearRestaurante->afinidadCocineros=comando[4];
				argsCrearRestaurante->platos=comando[5];
				argsCrearRestaurante->preciosPlatos=comando[6];
				argsCrearRestaurante->cantidadHornos=comando[7];
				if (crearRestaurante(argsCrearRestaurante))
					printf("Se creó el restaurante %s.\n", comando[1]);
				else
					printf("No se pudo crear el restaurante %s.\n", comando[1]);
				free(argsCrearRestaurante);
			}
			break;

		case crearReceta_:;
			log_trace(logger, "Se ingresó comando CrearReceta.");
			if(!validar_crear_receta(comando))
				puts("Faltan ingresar datos para la creación de la receta.");
			else{
				t_crear_receta* argsCrearReceta = malloc(sizeof(t_crear_receta));
				argsCrearReceta->nombre = comando[1];
				argsCrearReceta->pasos = comando[2];
				argsCrearReceta->tiempoPasos = comando[3];
				if (crearReceta(argsCrearReceta))
					printf("Se creó la receta %s.\n", comando[1]);
				else
					printf("No se pudo crear la receta %s.\n", comando[1]);
				free(argsCrearReceta);
			}
			break;

		case exit_:
			consolaActiva = 0;
			break;

		default:
			printf("No se reconoce el comando %s.", comando[0]);
		}
		liberar_lista(comando);
	}
	/*Si se eligio exit, termina solo la consola*/
}

t_tipoComando buscar_enum_sfs(char *sval) {
	t_tipoComando result = crearRestaurante_;
	char* comandos_str[] = { "CrearRestaurante", "CrearReceta", "exit", NULL };
	if (sval == NULL)
		return -2;
	for (int i = 0; comandos_str[i] != NULL; i++, result++)
		if (strcmp(sval, comandos_str[i]) == 0)
			return result;
	return -1;
}

int validar_crear_restaurante(char** comandos_crear_restaurante){
	return comandos_crear_restaurante[1] != NULL && comandos_crear_restaurante[2] != NULL &&  comandos_crear_restaurante[3] != NULL &&
			comandos_crear_restaurante[4] != NULL && comandos_crear_restaurante[5] != NULL && comandos_crear_restaurante[6] != NULL &&
			comandos_crear_restaurante[7] != NULL;
}
int validar_crear_receta(char** comandos_crear_receta){
	return comandos_crear_receta[1] != NULL && comandos_crear_receta[2] != NULL &&  comandos_crear_receta[3] != NULL;
}

char* generarContenidoRestauranteEnBloques(t_crear_restaurante* argsCrearRestaurante) {
	char* buffer = string_from_format("CANTIDAD_COCINEROS=%s\nPOSICION=%s\nAFINIDAD_COCINEROS=%s\nPLATOS=%s\nPRECIO_PLATOS=%s\nCANTIDAD_HORNOS=%s",
			argsCrearRestaurante->cantidadCocineros,
			argsCrearRestaurante->posicion,
			argsCrearRestaurante->afinidadCocineros,
			argsCrearRestaurante->platos,
			argsCrearRestaurante->preciosPlatos,
			argsCrearRestaurante->cantidadHornos);
	return buffer;
}

char* generarContenidoRecetaEnBloques(t_crear_receta* argsCrearReceta) {
	char* buffer = string_from_format("PASOS=%s\nTIEMPO_PASOS=%s",
			argsCrearReceta->pasos,
			argsCrearReceta->tiempoPasos);
	return buffer;
}

int crearRestaurante(t_crear_restaurante* argsCrearRestaurante){
	int result = 0;
	/*0.Se valida si ya existe el restaurante*/
	char* ruta_restaurante = string_from_format("%s%s", ruta_restaurantes, argsCrearRestaurante->nombreRestaurante);
	char* ruta_info = string_from_format("%s/Info.AFIP", ruta_restaurante);
	if(existeFile(ruta_info)){
		log_warning(logger, "El restaurante %s ya existe en el FileSystem AFIP.",argsCrearRestaurante->nombreRestaurante);
	}
	else{
		// 1. Creo directorio con nombre restaurante
		crearDirectorio(ruta_restaurante);
		// 2. Armar buffer a escribir
		char* linea = generarContenidoRestauranteEnBloques(argsCrearRestaurante);
		// 3. Calcular cantidad de bloques necesarios
		int tamanio = strlen(linea)+1;
		int* bloques_necesarios = listar_bloques_necesarios_file_nuevo(tamanio);
		// 4. Guardo la información en los bloques
		persistirDatos(linea, bloques_necesarios);
		// 5. Creo info.AFIP
		t_metadata* new_metadata = malloc(sizeof(t_metadata));
		new_metadata->size = tamanio;
		new_metadata->initial_block = bloques_necesarios[0];
		crearMetadataArchivo(ruta_info, new_metadata);
		free(new_metadata);
		//aplicar_retardo_fs("Creación de Restaurante");LO DEJO COMENTADO PORQUE SEGURO SE LES OLVIDO A LOS AYUDANTES
		free(linea);
		free(bloques_necesarios);
		log_info(logger, "[Creacion nuevo archivo] Restaurante creado: %s.", argsCrearRestaurante->nombreRestaurante);
		result = 1;
	}
	free(ruta_restaurante);
	free(ruta_info);
	return result;
}

int crearReceta(t_crear_receta* argsCrearReceta){
	int result = 0;
	/*0.Se valida si ya existe la receta*/
	char* ruta_receta = string_from_format("%s%s.AFIP", ruta_recetas, argsCrearReceta->nombre);
	if(existeFile(ruta_receta))
		log_warning(logger, "La receta %s ya existe en el FileSystem AFIP.", argsCrearReceta->nombre);
	else{
		// 1. Armar buffer a escribir
		char* linea = generarContenidoRecetaEnBloques(argsCrearReceta);
		// 2. Calcular cantidad de bloques necesarios
		int tamanio = strlen(linea)+1;
		int* bloques_necesarios = listar_bloques_necesarios_file_nuevo(tamanio);
		// 3. Guardo la información en los bloques
		persistirDatos(linea, bloques_necesarios);
		// 4. Creo nombreReceta.AFIP
		t_metadata* new_metadata = malloc(sizeof(t_metadata));
		new_metadata->size = tamanio;
		new_metadata->initial_block = bloques_necesarios[0];
		crearMetadataArchivo(ruta_receta, new_metadata);
		free(new_metadata);
		//aplicar_retardo_fs("Creación de Restaurante");LO DEJO COMENTADO PORQUE SEGURO SE LES OLVIDO A LOS AYUDANTES
		free(linea);
		free(bloques_necesarios);
		log_info(logger, "[Creacion nuevo archivo] Receta creada: %s.", argsCrearReceta->nombre);
		result = 1;
	}
	free(ruta_receta);
	return result;
}
