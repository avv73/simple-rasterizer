#define UNICODE
#define _UNICODE
#include <windows.h>
#include <stdio.h>
#include <strsafe.h>
#include "rasterizer.h"
#include "main.h"
#include "rasterizer_math.h"
#include <math.h>

// Configurables.

const int WINDOW_HEIGHT = 600;
const int WINDOW_WIDTH = 600;

int isMinimized = 0;

// Called by the rasterizer, describes the scene to render

void Draw() {
	Vector3 a = { 1, 1, 1 };
	Vector3 b = { -1, 1, 1 };
	Vector3 c = { -1, -1, 1 };
	Vector3 d = { 1, -1, 1 };

	Vector3 e = { 1, 1, -1 };
	Vector3 f = { -1, 1, -1 };
	Vector3 g = { -1, -1, -1 };
	Vector3 h = { 1, -1, -1 };

	Vector3 vertxA[8] = { a, b, c, d, e, f, g, h };
	Vector3* vertx = (Vector3*)malloc(sizeof(Vector3) * 8);
	for (int i = 0; i < 8; i++) {
		vertx[i] = vertxA[i];
	}

	Vector3 qt = { 0, 1, 2 };
	Vector3 wt = { 0, 2, 3 };
	Vector3 et = { 4, 0, 3 };
	Vector3 tt = { 4, 3, 7 };
	Vector3 yt = { 5, 4, 7 };
	Vector3 ut = { 5, 7, 6 };
	Vector3 it = { 1, 5, 6 };
	Vector3 ot = { 1, 6, 2 };
	Vector3 pt = { 4, 5, 1 };
	Vector3 zt = { 4, 1, 0 };
	Vector3 xt = { 2, 6, 7 };
	Vector3 vt = { 2, 7, 3 };

	Triangle q = { qt, RT_RGB(255, 0, 0) };
	Triangle w = { wt, RT_RGB(255, 0, 0) };
	Triangle eE = { et, RT_RGB(0, 255, 0) };
	Triangle t = { tt, RT_RGB(0, 255, 0) };
	Triangle y = { yt, RT_RGB(0, 0, 255) };
	Triangle u = { ut, RT_RGB(0, 0, 255) };
	Triangle i = { it, RT_RGB(255, 255, 0) };
	Triangle o = { ot, RT_RGB(255, 255, 0) };
	Triangle p = { pt, RT_RGB(255, 0, 255) };
	Triangle z = { zt, RT_RGB(255, 0, 255) };
	Triangle x = { xt, RT_RGB(0, 255, 255) };
	Triangle v = { xt, RT_RGB(0, 255, 255) };

	Triangle trsA[12] = { q, w, eE, t, y, u, i, o, p, z, x, v };
	Triangle* trs = (Triangle*)malloc(sizeof(Triangle) * 12);
	for (int i = 0; i < 12; i++) {
		trs[i] = trsA[i];
	}

	Vector3 cubeB = { 0,0,0 };
	Model cubeA = { trs, 12, vertx, 8, cubeB, sqrt(3) }; // TODO: ???????
	Model* cube = (Model*)malloc(sizeof(Model));

	*cube = cubeA;


	Vector3 t1 = { -1.5, 0, 7 };
	Instance in1 = { cube, t1, 0.75, IdentityMM4() };

	Vector3 t2 = { 1.25, 2, 7.5 };
	Instance in2 = { cube, t2, 1, CreateYRotationMatrix(195) };

	Vector3 t3 = { 0, 0, -10 };
	Instance in3 = { cube, t3, 1, CreateYRotationMatrix(195) };

	Instance insA[3] = { in1, in2, in3 };
	Instance* ins = (Instance*)malloc(sizeof(Instance) * 3);
	for (int i = 0; i < 3; i++) {
		ins[i] = insA[i];
	}

	mainScn.insts = ins;
	mainScn.instCnt = 3;

	Vector3 tC = { -3, 1, 2 };
	float r2 = sqrt(2);

	Vector3 pl1P = { 0,0,1 };
	Vector3 pl2P = { r2, 0, r2 };
	Vector3 pl3P = { -r2, 0, r2 };
	Vector3 pl4P = { 0, -r2, r2 };
	Vector3 pl5P = { 0, r2, r2 };

	Plane nearPl = { pl1P, -1 };
	Plane leftPl = { pl2P, 0 };
	Plane rightPl = { pl3P, 0 };
	Plane topPl = { pl4P, 0 };
	Plane botPl = { pl5P, 0 };

	Plane plnA[5] = { nearPl, leftPl, rightPl, topPl, botPl };
	Plane* pln = (Plane*)malloc(sizeof(Plane) * 5);
	for (int i = 0; i < 5; i++) {
		pln[i] = plnA[i];
	}

	Camera cmr = { tC, CreateYRotationMatrix(-30), pln, 5 };
	mainScn.cmr = cmr;

	RasterizeScene();
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


	mainScn.prjPlaneZ = 1;
	mainScn.vwpSize = 1;

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