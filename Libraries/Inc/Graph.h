#ifndef __GRAPH__H__
#define __GRAPH__H__

typedef struct _graphType{
	float * xData;
	float * yData;
	uint16_t count;
	uint8_t xAxisPos;
	uint8_t yAxisPos;
	float xDensity;
	float yDensity;
	void (* Print) (struct _graphType * graph_var, uint32_t color);
	void (* Add) (struct _graphType * graph_var, float xData, float);
	void (* Pop) (struct _graphType * graph_var, float * xData, float *);
	void (* ChangeDensity) (struct _graphType * graph_var, float xDen, float yDen);
	void (* ChangeAxis) (struct _graphType * graph_var, uint8_t xAxisPos, uint8_t yAxisPos);
}graph_t;

graph_t * _Graph_Init(float * xData, float * yData, uint16_t count, uint8_t xAxisPos, uint8_t yAxisPos, float xDen, float yDen);
graph_t * Graph_InitNull(uint8_t xAxisPos, uint8_t yAxisPos, float xDen, float yDen);


#define Graph_Init(xData, yData, xAxis, yAxis, xDen, yDen) \
	_Graph_Init(xData, yData, sizeof(xData) / sizeof(float), xAxis, yAxis, xDen, yDen)

//가운데 지점을 원점으로 하여 초기화하는 기법입니다.
#define Graph_InitCenter(xData, yData, xDen, yDen) \
	_Graph_Init(xData, yData, sizeof(xData) / sizeof(float), (OLED_SIZEX>>1), (OLED_SIZEY>>1), xDen, yDen)

//왼쪽 아래 지점을 원점으로 하여 초기화하는 기법입니다.
#define Graph_InitEdge(xData, yData, xDen, yDen) \
	_Graph_Init(xData, yData, sizeof(xData) / sizeof(float), 1, 52, xDen, yDen)


void Graph_PrintPoint(graph_t * graph_var, float x, float y, uint32_t color);
void Graph_Reflect(graph_t * g, float * xData, float * yData);
void Graph_Delete(graph_t * graph_var);

void Graph_UI(graph_t * gr);
void Graph_UI2(graph_t * gr1, graph_t * gr2);


graph_t * regularPolygon(uint8_t number, float radius, float angle, uint8_t xAxisPos, uint8_t yAxisPos, float xDen, float yDen);



#endif
