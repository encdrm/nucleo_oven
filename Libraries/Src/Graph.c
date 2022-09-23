
#include "main.h"
#include "OLED.h"
#include "Graph.h"
#include "Switch.h"
#include <stdlib.h>
#include <math.h>




void _Graph_Print(graph_t * graph_var, uint32_t color){
	for(uint16_t i = 0; i < graph_var -> count - 1; i++){
		int32_t x1 = graph_var -> xAxisPos + graph_var -> xData [i] / graph_var -> xDensity;
		int32_t x2 = graph_var -> xAxisPos + graph_var -> xData [i + 1] / graph_var -> xDensity;
		int32_t y1 = graph_var -> yAxisPos - graph_var -> yData [i] / graph_var -> yDensity;
		int32_t y2 = graph_var -> yAxisPos - graph_var -> yData [i + 1] / graph_var -> yDensity;
		OLED_Line(x1, y1, x2, y2, color);
	}
}

void _Graph_PrintPoint(graph_t * graph_var, uint16_t idx, uint32_t color){
	int32_t x1 = graph_var -> xAxisPos + graph_var -> xData [idx] / graph_var -> xDensity;
	int32_t y1 = graph_var -> yAxisPos - graph_var -> yData [idx] / graph_var -> yDensity;
	OLED_Dot(x1, y1, 1, color);
	OLED_Dot(x1+1, y1, 1, color);
	OLED_Dot(x1, y1+1, 1, color);
	OLED_Dot(x1-1, y1, 1, color);
	OLED_Dot(x1, y1-1, 1, color);
}

void _Graph_Add(graph_t * graph_var, float xData, float yData){
	graph_var -> xData = realloc(graph_var -> xData, (graph_var -> count + 1) * sizeof(float));
	graph_var -> yData = realloc(graph_var -> yData, (graph_var -> count + 1) * sizeof(float));
	graph_var -> xData[graph_var -> count] = xData;
	graph_var -> yData[graph_var -> count] = yData;
	graph_var -> count += 1;
}

void _Graph_Pop(graph_t * graph_var, float * xData, float * yData){
	if(graph_var -> count > 0){
		if(xData != NULL){
			*xData = graph_var -> xData[graph_var -> count - 1];
		}
		if(yData != NULL){
			*yData = graph_var -> yData[graph_var -> count - 1];
		}
		graph_var -> xData = realloc(graph_var -> xData, (graph_var -> count - 1) * sizeof(float));
		graph_var -> yData = realloc(graph_var -> yData, (graph_var -> count - 1) * sizeof(float));
		graph_var -> count -= 1;
	}
}

static void _Graph_ChangeDensity(struct _graphType * graph_var, float xDen, float yDen){
	graph_var -> xDensity = xDen;
	graph_var -> yDensity = yDen;
}

static void _Graph_ChangeAxis(struct _graphType * graph_var, uint8_t xAxisPos, uint8_t yAxisPos){
	graph_var -> xAxisPos = xAxisPos;
	graph_var -> yAxisPos = yAxisPos;
}


graph_t * _Graph_Init(float * xData, float * yData, uint16_t count, uint8_t xAxisPos, uint8_t yAxisPos, float xDen, float yDen){
	//배열을 받는 경우에는 malloc로 굳이 동적할당 할 필요는 없다. 하지만 멤버변수 정도는 동적 할당을 한다.
	graph_t * graph_var = (graph_t *)malloc(sizeof(graph_t));
	graph_var -> xData = (float *) malloc(sizeof(float) * count);
	graph_var -> yData = (float *) malloc(sizeof(float) * count);
	for(uint16_t i = 0; i < count; i++){
		graph_var -> xData[i] = xData[i];
		graph_var -> yData[i] = yData[i];
	}
	graph_var -> count = count;
	graph_var -> xAxisPos = xAxisPos;
	graph_var -> yAxisPos = yAxisPos;
	graph_var -> xDensity = xDen;
	graph_var -> yDensity = yDen;
	graph_var -> Print = _Graph_Print;
	graph_var -> Add = _Graph_Add;
	graph_var -> Pop = _Graph_Pop;
	graph_var -> ChangeDensity = _Graph_ChangeDensity;
	graph_var -> ChangeAxis = _Graph_ChangeAxis;

	return graph_var;
}

graph_t * Graph_InitNull(uint8_t xAxisPos, uint8_t yAxisPos, float xDen, float yDen){
	graph_t * graph_var = (graph_t *)malloc(sizeof(graph_t));
	graph_var -> xData = (float *) malloc(0);
	graph_var -> yData = (float *) malloc(0);
	graph_var -> count = 0;
	graph_var -> xAxisPos = xAxisPos;
	graph_var -> yAxisPos = yAxisPos;
	graph_var -> xDensity = xDen;
	graph_var -> yDensity = yDen;
	graph_var -> Print = _Graph_Print;
	graph_var -> Add = _Graph_Add;
	graph_var -> Pop = _Graph_Pop;
	return graph_var;
}


void Graph_Delete(graph_t * graph_var){
	free(graph_var -> xData);
	free(graph_var -> yData);
	free(graph_var);
}

graph_t * regularPolygon(uint8_t number, float radius, float angle, uint8_t xAxisPos, uint8_t yAxisPos, float xDen, float yDen){
	float * xData = (float *)malloc(sizeof(float) * (number + 1));
	float * yData = (float *)malloc(sizeof(float) * (number + 1));
	for(uint8_t i = 0; i < number + 1; i++){
		xData[i] = radius * cosf(angle + 6.28319f * i / number);
		yData[i] = radius * sinf(angle + 6.28319f * i / number);
	}
	return _Graph_Init(xData, yData, number + 1, xAxisPos, yAxisPos, xDen, yDen);
}

void Graph_UI(graph_t * gr){
	uint16_t idx = 0;
	OLED_Line(0, 53, 95, 53, 0xFF00FF);
	gr -> Print(gr, 0x0000FF);
	_Graph_PrintPoint(gr, idx, 0xFF8800);
	OLED_Printf("/s/6/rx:%d, /yy:%d", (int)gr->xData[idx], (int)gr->yData[idx]);
	for(;;){
		uint16_t sw = Switch_Read();
		if(sw == SW_RIGHT && idx < gr->count - 1){
			idx ++;
			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr -> Print(gr, 0x0000FF);
			_Graph_PrintPoint(gr, idx, 0xFF8800);
			OLED_Printf("/s/6/rx:%d, /yy:%d", (int)gr->xData[idx], (int)gr->yData[idx]);
		}
		else if(sw == SW_LEFT && idx > 0){
			idx --;
			OLED_Clear();
			OLED_Line(0, 53, 95, 53, 0xFF00FF);
			gr -> Print(gr, 0x0000FF);
			_Graph_PrintPoint(gr, idx, 0xFF8800);
			OLED_Printf("/s/6/rx:%d, /yy:%d", (int)gr->xData[idx], (int)gr->yData[idx]);
		}
	}


}


