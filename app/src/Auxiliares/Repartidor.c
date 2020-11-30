/*
 * Repartidor.c
 *
 *  Created on: 27 nov. 2020
 *      Author: utnso
 */

#include "Repartidor.h"

void* correr_repartidor(t_repartidor* repartidor) {

	// TODO: Semáforo para ver si está disponible para moverse
	// TODO: Mover entrenador a posición nueva

	log_info(logger, "Repartidor N°%d | Inicia hilo de Repartidor", repartidor->id);
	while(1) {

		sem_wait(&repartidor->sem_moverse);

		imprimirRepartidor(repartidor);
		log_info(logger, "Repartidor N°%d | Estado %s", repartidor->id, get_nombre_instruccion(repartidor->instruccion));

		log_info(logger, "Repartidor N°%d | Se va a mover %d", repartidor->id , repartidor->quantum);

		while(repartidor->quantum > 0) {
			mover_una_posicion(repartidor);

			if(repartidor->frecuenciaDescanso == 0) {

				// TODO: descansar
				// TODO: cambiar estado a descansar
				//sleep(repartidor->tiempoDescanso);
				log_info(logger, "Repartidor N°%d tiene que descansar", repartidor->id);
				break;
			}

//			TODO: Probando a ver si desaloja
//			log_info(logger, "ENTRENADOR %d | Liberar CPU: %s", entrenador->id , (entrenador->liberar_cpu)?"SI":"NO");
//			if(entrenador->liberar_cpu) {
//				log_info(logger, "ENTRENADOR %d | Tiene que liberar CPU", entrenador->id);
//				break;
//			}
		}

		if(repartidor->quantum != 0) {
			//TODO: Está descansando
			repartidor->instruccion = DESCANSAR;
		} else if (repartidor->instruccion == BUSCAR_PEDIDO){
			//TODO: Esperar pedido
			repartidor->instruccion = ESPERAR_PEDIDO;
		}
//		else {
//			repartidor->instruccion = ENTREGAR_PEDIDO;
////			finalizarPCB(repartidor);
//		}

		log_info(logger, "Repartidor N°%d | Pasó a estado %s", repartidor->id, get_nombre_instruccion(repartidor->instruccion));

		if(repartidor->instruccion == ESPERAR_PEDIDO || repartidor->instruccion == DESCANSAR) {
			bloquearPCB(repartidor);
		} else {
			finalizarPCB(repartidor);
		}


		//
////		if(!entrenador->liberar_cpu) {
//			//TODO: Si el estado es DISPONIBLE: va a capturar pokemon
//			if(entrenador->estado == ATRAPAR) {
//	//			log_info(logger, "ENTRENADOR | Entrenador %d se va a mover %d posiciones", entrenador->id, entrenador->quantum_disponible);
//
//				if(!entrenador->esta_limitado) {
//
//					int movimientos = distancia_a_pokemon(entrenador, entrenador->pokemon_a_capturar);
//
//					if(movimientos > entrenador->quantum_disponible) {
//						movimientos = entrenador->quantum_disponible;
//					}
//
//					log_info(logger, "ENTRENADOR %d | Se va a mover %d", entrenador->id , movimientos);
//
//					int i = 0;
//					while(i < movimientos) {
//						mover_una_posicion(entrenador);
//
////						 TODO: Probando a ver si desaloja
//						log_info(logger, "ENTRENADOR %d | Liberar CPU: %s", entrenador->id , (entrenador->liberar_cpu)?"SI":"NO");
//						if(entrenador->liberar_cpu) {
//							log_info(logger, "ENTRENADOR %d | Tiene que liberar CPU", entrenador->id);
//							break;
//						}
//						i++;
//					}
//
//					if(distancia_a_pokemon(entrenador, entrenador->pokemon_a_capturar) == 0) {
//
//						entrenador->llego_a_posicion = 1;
//						int id_mensaje_catch = publicar_catch(entrenador->pokemon_a_capturar);
//						entrenador->id_mensaje_espera=id_mensaje_catch;
//						entrenador->quantum_disponible -= 1;
//						registrar_metricas_entrenador(entrenador, 1);
//
//						if(id_mensaje_catch == 0) {
//							// TODO: acción por default
//							atrapar_pokemon(entrenador);
//							entrenador->estado=DISPONIBLE;
//						} else {
//							entrenador->estado=EN_ESPERA;
//							log_info(logger,string_from_format("ENTRENADOR %d | A la espera de mensaje CAUGHT_POKEMON", entrenador->id));
//						}
//					} else {
//						entrenador->llego_a_posicion = 0;
//					}
//				}
//			}
//
//			//TODO: Si el estado es INTERBLOQUEO: va a intercambiar pokemon
//			if(entrenador->estado == INTERBLOQUEO) {
//
//				if(!entrenador->esta_limitado) {
//
//					int movimientos = distancia_a_posicion(entrenador, entrenador->posicion_intercambio);
//
//					if(movimientos > entrenador->quantum_disponible) {
//						movimientos = entrenador->quantum_disponible;
//					}
//
//					log_info(logger, "ENTRENADOR %d | Se va a mover %d", entrenador->id , movimientos);
//
//					int i = 0;
//					while(i < movimientos) {
//						mover_una_posicion(entrenador);
//
//						// TODO: Probando a ver si desaloja
//						log_info(logger, "ENTRENADOR %d | Liberar CPU: %s", entrenador->id , (entrenador->liberar_cpu)?"SI":"NO");
//						if(entrenador->liberar_cpu) {
//							log_info(logger, "ENTRENADOR %d | Tiene que liberar CPU: %s", entrenador->id);
//							break;
//						}
//						i++;
//					}
//
//					if(distancia_a_posicion(entrenador, entrenador->posicion_intercambio) == 0) {
//
//						entrenador->llego_a_posicion = 1;
//						log_info(logger,"CANTIDAD DE ENTRENADORES EN DEADLOCK:%d",list_size(entrenadores_en_deadlock()));
//						t_entrenador* entrenador_2 = entrenador_que_necesita_pokemon(entrenadores_en_deadlock(), entrenador->pokemon_a_capturar);
//						t_pokemon* pokemon_2 = pokemon_para_intercambio(entrenador_2);
//						// TODO: Buscar el entrenador->id_entrenador_intercambio en bloqueados con estado INTERBLOQUEO
//
//						// TODO: Intercambiar x2
//						transferir_pokemon(entrenador, entrenador_2, entrenador->pokemon_a_capturar);
//						transferir_pokemon(entrenador_2, entrenador, pokemon_2);
//						registrar_metricas_entrenador(entrenador, 5);
//
//						// TODO: Valido si cumplió objetivo
//						if(cumplio_objetivo(entrenador_2)) {
//							// TODO: Si cumple objetivo pasa a FINALIZADOS
//							sacar_entrenador_de_bloqueados(entrenador_2);
//							agregar_entrenador_a_finalizados(entrenador_2);
//							registrar_solucion_deadlock();
//							estoy_resolviendo_deadlock = 0;
//						}
//					} else {
//						entrenador->llego_a_posicion = 0;
//					}
//				}
//			}
////		} else {
////			log_info(logger, "ENTRENADOR %d| Desaloja la CPU", entrenador->id);
////		}
//		log_info(logger, "ENTRENADOR %d| Deja de moverse, en estado: %s", entrenador->id, get_nombre_estado(entrenador->estado));
//		sem_post(&sem_finaliza_movimiento);
	}

	return NULL;
}

void mover_una_posicion(t_repartidor* repartidor) {

	// Verifico si me tengo que mover en X o en Y
	if(repartidor->posX != repartidor->objetivo_posX) {

		// Valido si me muevo para la derecha o izquierda
		if(repartidor->posX < repartidor->objetivo_posX) {
			repartidor->posX += 1;
		} else {
			repartidor->posX -= 1;
		}

	} else {
		// Valido si me muevo para abajo o para arriba
		if(repartidor->posY < repartidor->objetivo_posY) {
			repartidor->posY += 1;
		} else {
			repartidor->posY -= 1;
		}
	}

	char* desc = string_from_format("Repartidor N°%d", repartidor->id);
	log_info(logger, "%s | Posición actual X,Y: %d,%d", desc, repartidor->posX, repartidor->posY);

	// TODO: Decrementar quantum disponible
	repartidor->quantum -= 1;
	repartidor->frecuenciaDescanso -= 1;
//	registrar_metricas_entrenador(entrenador, 1);

	// TODO: Agregar metrica global

	aplicar_retardo_operacion();

}

