#ifndef __PROFILE__H__
#define __PROFILE__H__

#include "main.h"
#include "OLED.h"
#include "Graph.h"
#include "Switch.h"

#include <stdio.h>
#include <string.h>
#include "max6675.h"
#include "control.h"


void profile();
void Profile_Set(graph_t * gr1, graph_t * gr2);


#endif
