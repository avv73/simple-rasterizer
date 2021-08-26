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

// Converts a vector in viewport coordinate system to vector in canvas coordinate system.
Vector2 ViewportToCanvas(Vector2 a) {
	Vector2 res = { a.x * RT_WINDOW_WIDTH / mainScn.vwpSize, a.y * RT_WINDOW_HEIGHT / mainScn.vwpSize };
	return res;
}

// Projects a 3D vertex in homogenous coordinates into 2D point in canvas coordinates.
Vector2 ProjectVertex(Vector4 v) {
	Vector2 res = { v.x * mainScn.prjPlaneZ / v.z, v.y * mainScn.prjPlaneZ / v.z };
	return ViewportToCanvas(res);
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

// Performs linear interpolation between two Vector2s (i/d0 and i/d1), assumes i1 > i0 . Returns array of the interpolated values.
float* Interpolate(float i0, float d0, float i1, float d1) {
	// round off float values
	i0 = ceil(i0);
	i1 = ceil(i1);

	if (i0 == i1) {
		float* res = (float*)malloc(sizeof(float));
		*res = d0;
		return res;
	}

	float* res = (float*)malloc(sizeof(float) * (i1 - i0 + 1));

	float coeff = (d1 - d0) / (i1 - i0);
	float incY = d0;

	for (float p = i0; p <= i1; p++) {
		res[(int)(p - i0)] = incY;
		incY += coeff;
	}

	return res;
}

// Rasterizes a line defined by two Vector2s and a color to the screen.
void RasterizeLine(Vector2 a, Vector2 b, COLORREF clr) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	if (fabs(dx) > fabs(dy)) {
		// line is more parallel to the horizontal
		if (a.x > b.x) {
			Swap(&a, &b);
		}

		float* ys = Interpolate(a.x, a.y, b.x, b.y);

		for (float p = a.x; p <= b.x; p++) {
			PutPixel(p, ys[(int)(p - a.x)], clr);
		}
	}
	else {
		// line is more parallel to the vertical
		if (a.y > b.y) {
			Swap(&a, &b);
		}

		float* xs = Interpolate(a.y, a.x, b.y, b.x);

		for (float p = a.y; p <= b.y; p++) {
			PutPixel(xs[(int)(p - a.y)], p, clr);
		}
	}
}

// Rasterizes the outline (wireframe) of a triangle, defined by three Vector2s and a color.
void RasterizeWireframeTriangle(Vector2 a, Vector2 b, Vector2 c, COLORREF clr) {
	RasterizeLine(a, b, clr);
	RasterizeLine(b, c, clr);
	RasterizeLine(c, a, clr);
}

// Rasterizes a filled triangle with a solid color, defined by three Vector2s and a color.
void RasterizeFilledTriangle(Vector2 a, Vector2 b, Vector2 c, COLORREF clr) {
	float* xL = NULL;
	float* xR = NULL;

	if (b.y < a.y) {
		Swap(&b, &a);
	}

	if (c.y < a.y) {
		Swap(&c, &a);
	}

	if (c.y < b.y) {
		Swap(&c, &b);
	}

	float* xAB = Interpolate(a.y, a.x, b.y, b.x);
	float* xBC = Interpolate(b.y, b.x, c.y, c.x);
	float* xAC = Interpolate(a.y, a.x, c.y, c.x);

	float* xABC = ArrayConcat(xAB, b.y - a.y, xBC, c.y - b.y + 1);   // ignoring the last value of B in xAB

	int m = floor((c.y - a.y + 1) / 2);
	if (xAC[m] < xABC[m]) {
		xL = xAC;
		xR = xABC;
	}
	else {
		xL = xABC;
		xR = xAC;
	}

	for (float y = a.y; y <= c.y; y++) {
		for (float x = xL[(int)(y - a.y)]; x <= xR[(int)(y - a.y)]; x++) {
			PutPixel(x, y, clr);
		}
	}
}

// Rasterizes a filled triangle with a gradient color, defined by three Vector2s, color, and gradient factor [0-1] of each Vector2.
void RasterizeShadedTriangle(Vector2 a, Vector2 b, Vector2 c, COLORREF clr, float aH, float bH, float cH) {
	float* xL = NULL;
	float* hL = NULL;
	float* xR = NULL;
	float* hR = NULL;

	if (b.y < a.y) {
		Swap(&b, &a);
	}

	if (c.y < a.y) {
		Swap(&c, &a);
	}

	if (c.y < b.y) {
		Swap(&c, &b);
	}

	float* xAB = Interpolate(a.y, a.x, b.y, b.x);
	float* hAB = Interpolate(a.y, aH, b.y, bH);

	float* xBC = Interpolate(b.y, b.x, c.y, c.x);
	float* hBC = Interpolate(b.y, bH, c.y, cH);

	float* xAC = Interpolate(a.y, a.x, c.y, c.x);
	float* hAC = Interpolate(a.y, aH, c.y, cH);

	float* xABC = ArrayConcat(xAB, b.y - a.y, xBC, c.y - b.y + 1);   // ignoring the last value of B in xAB
	float* hABC = ArrayConcat(hAB, b.y - a.y, hBC, c.y - b.y + 1);   // same but in hAB

	int m = floor((c.y - a.y + 1) / 2);
	if (xAC[m] < xABC[m]) {
		xL = xAC;
		hL = hAC;

		xR = xABC;
		hR = hABC;
	}
	else {
		xL = xABC;
		hL = hABC;

		xR = xAC;
		hR = hAC;
	}

	for (int y = a.y; y <= c.y; y++) {
		float currXL = xL[(int)(y - a.y)];
		float currXR = xR[(int)(y - a.y)];

		float* h_segms = Interpolate(currXL, hL[(int)(y - a.y)], currXR, hR[(int)(y - a.y)]);

		for (int x = currXL; x <= currXR; x++) {
			float shR = RT_GetRValue(clr) * h_segms[(int)(x - currXL)];
			float shG = RT_GetGValue(clr) * h_segms[(int)(x - currXL)];
			float shB = RT_GetBValue(clr) * h_segms[(int)(x - currXL)];

			PutPixel(x, y, RT_RGB(shR, shG, shB));
		}
	}
}

// Rasterizes a triangle, accepts the triangle itself and array of all projected vertices of the object.
void RasterizeTriangle(Triangle tr, Vector2* prjsVertx) {
	RasterizeWireframeTriangle(prjsVertx[(int)tr.vtxIndList.x],
		prjsVertx[(int)tr.vtxIndList.y],
		prjsVertx[(int)tr.vtxIndList.z],
		tr.clr
	);
}

// Rasterizes and projects an transformed & clipped model.
void RasterizeModel(Model* m) {
	Vector2* prjsVertx = (Vector2*)malloc(sizeof(Vector2) * m->vCnt);
	for (int i = 0; i < m->vCnt; i++) {
		Vector3 currV = m->vertx[i];
		Vector4 homV = { currV.x, currV.y, currV.z, 1 };
		prjsVertx[i] = ProjectVertex(homV);
	}

	for (int i = 0; i < m->trCnt; i++) {
		RasterizeTriangle(m->trs[i], prjsVertx);
	}

	free(prjsVertx);
}

void ClipTriangle(Triangle tr, Plane p, Triangle* mainTrs, int* lastTr, Vector3* pnts) {
	Vector3 a = pnts[(int)tr.vtxIndList.x];
	Vector3 b = pnts[(int)tr.vtxIndList.y];
	Vector3 c = pnts[(int)tr.vtxIndList.z];

	int in0 = DotProduct(p.norm, a) + p.dist > 0;
	int in1 = DotProduct(p.norm, b) + p.dist > 0;
	int in2 = DotProduct(p.norm, c) + p.dist > 0;

	int totalIn = in0 + in1 + in2;
	if (totalIn == 0) {
		// fully clipped out
	}
	else if (totalIn == 3) {
		// all vertices in plane
		mainTrs[*lastTr] = tr;
		*lastTr = *lastTr + 1;
	}
	else if (totalIn == 1) {
		// one vertex in plane
	}
	else if (totalIn == 2) {
		// two vertices in plane
	}
}

Model* TransformAndClip(Model* m, float** transf, Plane* clipPln, int plnCnt) {
	// bounding sphere transform & early discard
	Vector4 boundCntH = { m->boundCnt.x, m->boundCnt.y, m->boundCnt.z, 1 };
	Vector4 cnt = MultiplyMatrixVector(transf, boundCntH);
	float radi2 = m->boundRad * m->boundRad;

	Vector3 cnt3 = { cnt.x, cnt.y, cnt.z };

	for (int i = 0; i < mainScn.cmr.clipPlnCnt; i++) {
		float distance2 = DotProduct(mainScn.cmr.clipPln[i].norm, cnt3) + mainScn.cmr.clipPln[i].dist;
		if (distance2 < -radi2) {
			return NULL;
		}
	}

	// transform
	Vector3* vertxs = (Vector3*)malloc(sizeof(Vector3) * m->vCnt);
	for (int i = 0; i < m->vCnt; i++) {
		Vector4 mVertxH = { m->vertx[i].x, m->vertx[i].y, m->vertx[i].z, 1 };
		Vector4 res = MultiplyMatrixVector(transf, mVertxH);
		Vector3 transfVertx = { res.x, res.y, res.z };

		vertxs[i] = transfVertx;
	}

	// clip each model against each plane
	Triangle* cpyTr = ArrayCopyTriangle(m->trs, m->trCnt);

	for (int i = 0; i < plnCnt; i++) {
		Triangle* newTrs = (Triangle*)malloc(sizeof(Triangle) * m->trCnt);
		int* cnt = (int*)calloc(1, sizeof(int));

		for (int j = 0; j < m->trCnt; j++) {
			ClipTriangle(cpyTr[j], clipPln[i], newTrs, cnt, vertxs);
		}

		free(cnt);
		free(cpyTr);
		cpyTr = newTrs;
	}

	Model* res = (Model*)malloc(sizeof(Model));
	res->boundCnt = cnt3;
	res->boundRad = m->boundRad;
	res->trs = cpyTr;
	res->vertx = vertxs;
	res->vCnt = m->vCnt;
	res->trCnt = m->trCnt;

	return res;
}

// Rasterizes all current instances in the scene
void RasterizeScene() {
	float** camMat = MultiplyMM4(TransposeMM4(mainScn.cmr.rot), CreateTranslationMatrix(ScaleVector(-1, mainScn.cmr.pos)));							// camera matrix; Cr^-1 X Ct^-1

	for (int i = 0; i < mainScn.instCnt; i++) {
		Instance currInst = mainScn.insts[i];
		float** localTransf = MultiplyMM4(CreateTranslationMatrix(currInst.pos), MultiplyMM4(currInst.rot, CreateScalingMatrix(currInst.scl)));		// local transformation matrix of model instance; It X Ir X Is
		localTransf = MultiplyMM4(camMat, localTransf);

		Model* clip = TransformAndClip(mainScn.insts[i].m, localTransf, mainScn.cmr.clipPln, mainScn.cmr.clipPlnCnt);

		if (clip) {
			RasterizeModel(clip);
		}

		free(localTransf);
		free(clip);
	}

	free(camMat);
}