#include <windows.h>
#include "rasterizer.h"
#include "rasterizer_math.h"
#include <math.h>
#include <strsafe.h>
#include <stdio.h>
#include "main.h"

COLORREF* frmBuffer = NULL;

int RT_WINDOW_WIDTH;
int RT_WINDOW_HEIGHT;

void Draw();

void Update(HWND wndHandle);

void SetBg(COLORREF clr);

// Initializes the rasterizer and renders the scene

void StartRasterizer(HWND wndHandle, int nWidth, int nHeight) {
	RT_WINDOW_WIDTH = nWidth;
	RT_WINDOW_HEIGHT = nHeight;

	if (!frmBuffer) {
		frmBuffer = (COLORREF*)calloc(RT_WINDOW_WIDTH * RT_WINDOW_HEIGHT, sizeof(COLORREF));

		SetBg(RT_RGB(255, 255, 255));
		Draw();
	}

	Update(wndHandle);
}

// Puts pixel on the screen, converts from canvas coordinate system to screen coordinate system itself.

void PutPixel(int x, int y, COLORREF clr) {
	x = RT_WINDOW_WIDTH / 2 + x;
	y = RT_WINDOW_HEIGHT / 2 - y - 1;

	if (x < 0 || x >= RT_WINDOW_WIDTH || y < 0 || y >= RT_WINDOW_HEIGHT) {
		return;
	}

	frmBuffer[RT_WINDOW_HEIGHT * y + x] = clr;
}

// Sets the background color of the screen.

void SetBg(COLORREF clr) {
	for (int x = -RT_WINDOW_WIDTH / 2; x < RT_WINDOW_WIDTH / 2; x++) {
		for (int y = -RT_WINDOW_HEIGHT / 2; y < RT_WINDOW_HEIGHT / 2; y++) {
			PutPixel(x, y, clr);
		}
	}
}

// Updates window handle DC with bitmap of framebuffer

void Update(HWND wndHandle) {
	HDC wndDc = GetDC(wndHandle);

	HBITMAP map = CreateBitmap(RT_WINDOW_WIDTH, RT_WINDOW_HEIGHT, 1, 8 * 4, (void*)frmBuffer);

	HDC src = CreateCompatibleDC(wndDc);
	SelectObject(src, map);

	BitBlt(wndDc,
		0,
		0,
		RT_WINDOW_WIDTH,
		RT_WINDOW_HEIGHT,
		src,
		0,
		0,
		SRCCOPY);

	DeleteDC(src);
	DeleteObject(map);
}

// Performs linear interpolation between two points (i/d0 and i/d1), assumes i1 > i0 . Returns array of the interpolated values.

float* Interpolate(float i0, float d0, float i1, float d1) {
	if (i0 == i1) {
		float* res = (float*)malloc(sizeof(float));
		*res = d0;
		return res;
	}

	float* res = (float*)malloc(sizeof(float) * (i1 - i0));

	float coeff = (d1 - d0) / (i1 - i0);
	float incY = d0;

	for (float p = i0; p <= i1; p++) {
		res[(int)(p - i0)] = incY;
		incY += coeff;
	}

	return res;
}

// Rasterizes a line defined by two points and a color the screen.

void RasterizeLine(Point a, Point b, COLORREF clr) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	if (fabs(dx) > fabs(dy)) {
		// line is more parallel to the horizontal
		if (a.x > b.x) {
			Point temp = a;
			a = b;
			b = temp;
		}

		float* ys = Interpolate(a.x, a.y, b.x, b.y);

		for (float p = a.x; p <= b.x; p++) {
			PutPixel(p, ys[(int)(p - a.x)], clr);
		}
	}
	else {
		// line is more parallel to the vertical
		if (a.y > b.y) {
			Point temp = a;
			a = b;
			b = temp;
		}

		float* xs = Interpolate(a.y, a.x, b.y, b.x);

		for (float p = a.y; p <= b.y; p++) {
			PutPixel(xs[(int)(p - a.y)], p, clr);
		}
	}

}