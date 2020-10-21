/*
 * restaurante.h
 *
 *  Created on: 3 sep. 2020
 *      Author: utnso
 */

#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_
#define L_STRING 32

#include <shared.h>
#include "Auxiliares/Logueo.h"
#include "Auxiliares/Configuracion.h"
#include "Auxiliares/Conexion.h"
#include "Auxiliares/Utils.h"



pthread_t hilo_servidor;


typedef struct {
	uint32_t id;//Agregado en Restaurante
	char  afinidad[L_STRING];//Milanesas
} t_cocinero;

typedef struct {
	uint32_t id;
	uint32_t id_Cliente;
	char estado[L_STRING];//Confirmado
	t_list* platos;//[Milanesas, Pastas, Ensalada]
	t_list* cant_Platos;//[3, 5, 4]
	uint32_t precioTotal;// 300
} t_pedido;

typedef struct {
	uint32_t id;
	char nombre[L_STRING];//Milanesas
	uint32_t precio;// 30
} t_plato;

typedef struct {
	uint32_t id;
	char nombre[L_STRING];//Milanesas
	t_list* pasos;//[Trocear, Empanar, Reposar, Hornear]
	t_list* tiempos_pasos;//[4, 5, 3, 10]
} t_receta;



#endif /* RESTAURANTE_H_ */
