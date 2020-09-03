/*
 ============================================================================
 Name        : sindicato.c
 Author      : thread_away
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "sindicato.h"


int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	printf("Checking si linkea shared lib...\npor ej. ParentPath:%s\n", getParentPath());
	return EXIT_SUCCESS;
}
