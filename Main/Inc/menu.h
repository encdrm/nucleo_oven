#ifndef __MENU__H__
#define __MENU__H__


#include "main.h"
#include "Graph.h"

typedef struct {
	void (*function)(void);
	char *message;
	uint16_t color;
} Menu_t;




void Menu();
void Menu_Setup(void);
void Menu_Loop(void);


#endif
