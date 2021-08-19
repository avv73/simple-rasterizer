#define UNICODE
#define _UNICODE
#include <windows.h>
#include <stdio.h>
#include <strsafe.h>
#include "rasterizer.h"
#include "main.h"
#include "rasterizer_math.h"

// Configurables.

const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH = 600;

int isMinimized = 0;


// Custom configuration of the scene to render. Called by the rasterizer.

void Draw() {
	Point a = { -200, -250 };
	Point b = { 200, 50 };
	Point c = { 20, 250 };

	//RasterizeWireframeTriangle(a, b, c, RT_RGB(0, 0, 0));
	RasterizeFilledTriangle(a, b, c, RT_RGB(0, 255, 0));

	Point d = { 191, -215 };
	Point e = { 3, -202 };
	Point f = { 181, -75 };
	RasterizeFilledTriangle(d, e, f, RT_RGB(255, 0, 0));

	Point g = { -220, 242 };
	Point h = { -227, 74 };
	Point i = { -25, 270 };
	RasterizeFilledTriangle(g, h, i, RT_RGB(0, 0, 255));
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

const TCHAR CLSNAME[] = TEXT("Simple Rasterizer");

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdline, int cmdshow)
{
	WNDCLASSEX wc; // = { };
	MSG msg;
	HWND hwnd;

	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLSNAME;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, TEXT("Could not register window class"),
			NULL, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_LEFT,
		CLSNAME,
		CLSNAME,
		WS_MINIMIZEBOX | WS_SYSMENU, // WS_OVERLAPPEDWINDOW
		0,
		0,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		hInst,
		NULL);
	if (!hwnd) {
		MessageBox(NULL, TEXT("Could not create window"), NULL, MB_ICONERROR);
		return 0;
	}

	ShowWindow(hwnd, cmdshow);

	UpdateWindow(hwnd);

	StartRasterizer(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp)
{
	int xPos;
	int yPos;

	switch (wm) {
	case WM_DESTROY:
		exit(0);
		return 0;
	case WM_EXITSIZEMOVE:
		xPos = (int)(short)LOWORD(lp);
		yPos = (int)(short)HIWORD(lp);

		POINT p1;
		p1.x = xPos;
		p1.y = yPos;

		POINT p2;
		p2.x = xPos + WINDOW_WIDTH;
		p2.y = yPos;

		POINT p3;
		p3.x = xPos;
		p3.y = yPos + WINDOW_HEIGHT;

		POINT p4;
		p4.x = xPos + WINDOW_WIDTH;
		p4.y = yPos + WINDOW_HEIGHT;

		if (MonitorFromPoint(p1, MONITOR_DEFAULTTONULL) != NULL ||
			MonitorFromPoint(p2, MONITOR_DEFAULTTONULL) != NULL ||
			MonitorFromPoint(p3, MONITOR_DEFAULTTONULL) != NULL ||
			MonitorFromPoint(p4, MONITOR_DEFAULTTONULL) != NULL) {
			// rerender if window goes off screen
			StartRasterizer(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);
		}

		break;
	case WM_SIZE:
		if (wp == SIZE_MINIMIZED) {
			isMinimized = 1;
		}

		else if (wp == SIZE_RESTORED && isMinimized) {
			StartRasterizer(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);
			isMinimized = 0;
		}
		break;
	case WM_RBUTTONDOWN:
		// print mouse position on right click

		xPos = (int)(short)LOWORD(lp);
		yPos = (int)(short)HIWORD(lp);

		int xPosC = xPos - WINDOW_WIDTH / 2;
		int yPosC = -yPos + WINDOW_HEIGHT / 2 - 1;

		TCHAR txt[100];

		TCHAR header[] = TEXT("Information");
		LPCSTR textPlc = TEXT("Screen coordinates X: %d  Y: %d\nCanvas coordinates X: %d Y: %d");

		StringCbPrintf(txt, 100 * sizeof(TCHAR), textPlc, xPos, yPos, xPosC, yPosC);

		MessageBox(NULL, txt, header, NULL);

		break;
	}
	return DefWindowProc(hwnd, wm, wp, lp);
}