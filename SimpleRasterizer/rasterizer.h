#pragma once
#include <windows.h>
#include "rasterizer_math.h"


#define RT_RGB(r,g,b) (RGB(b,g,r))              // COLORREF are handled in different byte-order
#define RT_GetRValue(a) (GetBValue(a))
#define RT_GetGValue(a) (GetGValue(a))
#define RT_GetBValue(a) (GetRValue(a)) 

void StartRasterizer(HWND hwnd, int nWidth, int nHeight);

void RasterizeLine(Point a, Point b, COLORREF clr);

void RasterizeWireframeTriangle(Point a, Point b, Point c, COLORREF clr);

void RasterizeFilledTriangle(Point a, Point b, Point c, COLORREF clr);

void RasterizeShadedTriangle(Point a, Point b, Point c, COLORREF clr, float aH, float bH, float cH);