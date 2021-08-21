#pragma once
#include <windows.h>
#include "rasterizer_math.h"


#define RT_RGB(r,g,b) (RGB(b,g,r))              // COLORREF are handled in different byte-order
#define RT_GetRValue(a) (GetBValue(a))
#define RT_GetGValue(a) (GetGValue(a))
#define RT_GetBValue(a) (GetRValue(a)) 

typedef struct {
	Vector3 cmrPos;
	float vwpSize;
	float prjPlaneZ;
} Scene;

Scene mainScn;

void StartRasterizer(HWND hwnd, int nWidth, int nHeight);

void RasterizeLine(Vector2 a, Vector2 b, COLORREF clr);

void RasterizeWireframeTriangle(Vector2 a, Vector2 b, Vector2 c, COLORREF clr);

void RasterizeFilledTriangle(Vector2 a, Vector2 b, Vector2 c, COLORREF clr);

void RasterizeShadedTriangle(Vector2 a, Vector2 b, Vector2 c, COLORREF clr, float aH, float bH, float cH);

Vector2 ProjectVertex(Vector3 v);