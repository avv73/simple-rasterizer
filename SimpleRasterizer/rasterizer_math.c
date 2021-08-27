#include "rasterizer_math.h"
#include <stdio.h>
#include <windows.h>
#include <math.h>

#define PI 3.14159265

void Swap(Vector2* a, Vector2* b) {
	Vector2 temp = *a;
	*a = *b;
	*b = temp;
}

float* ArrayConcat(float* a, int aSize, float* b, int bSize) {
	float* res = (float*)malloc(sizeof(float) * (aSize + bSize));
	int s = 0;

	for (int i = 0; i < aSize; i++) {
		res[s] = a[i];
		s++;
	}

	for (int i = 0; i < bSize; i++) {
		res[s] = b[i];
		s++;
	}

	return res;
}

Vector3 AddVector(Vector3 v1, Vector3 v2) {
	Vector3 res = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	return res;
}

Vector3 CrossProduct(Vector3 v1, Vector3 v2) {
	Vector3 res = { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
	return res;
}

Vector3 TriangleNormal(Vector3 a, Vector3 b, Vector3 c) {
	Vector3 ab = AddVector(b, ScaleVector(-1, a));
	Vector3 ac = AddVector(c, ScaleVector(-1, a));
	return CrossProduct(ab, ac);
}

float* Vector3ToArray(Vector3 v) {
	float* res = (float*)malloc(sizeof(float) * 3);
	res[0] = v.x;
	res[1] = v.y;
	res[2] = v.z;

	return res;
}

float** CreateYRotationMatrix(float dg) {
	dg = dg * PI / 180.0;

	float sinT = sin(dg);
	float cosT = cos(dg);

	float* firstR = (float*)malloc(sizeof(float) * 4);
	float* secR = (float*)malloc(sizeof(float) * 4);
	float* thR = (float*)malloc(sizeof(float) * 4);
	float* frR = (float*)malloc(sizeof(float) * 4);

	firstR[0] = cosT;
	firstR[1] = 0;
	firstR[2] = -sinT;
	firstR[3] = 0;

	secR[0] = 0;
	secR[1] = 1;
	secR[2] = 0;
	secR[3] = 0;

	thR[0] = sinT;
	thR[1] = 0;
	thR[2] = cosT;
	thR[3] = 0;

	frR[0] = 0;
	frR[1] = 0;
	frR[2] = 0;
	frR[3] = 1;

	float** result = (float**)malloc(sizeof(float*) * 4);
	result[0] = firstR;
	result[1] = secR;
	result[2] = thR;
	result[3] = frR;

	return result;
}

float** CreateTranslationMatrix(Vector3 tr) {
	float* firstR = (float*)malloc(sizeof(float) * 4);
	float* secR = (float*)malloc(sizeof(float) * 4);
	float* thR = (float*)malloc(sizeof(float) * 4);
	float* frR = (float*)malloc(sizeof(float) * 4);

	firstR[0] = 1;
	firstR[1] = 0;
	firstR[2] = 0;
	firstR[3] = tr.x;

	secR[0] = 0;
	secR[1] = 1;
	secR[2] = 0;
	secR[3] = tr.y;

	thR[0] = 0;
	thR[1] = 0;
	thR[2] = 1;
	thR[3] = tr.z;

	frR[0] = 0;
	frR[1] = 0;
	frR[2] = 0;
	frR[3] = 1;

	float** result = (float**)malloc(sizeof(float*) * 4);
	result[0] = firstR;
	result[1] = secR;
	result[2] = thR;
	result[3] = frR;

	return result;
}

float** CreateScalingMatrix(float s) {
	float* firstR = (float*)malloc(sizeof(float) * 4);
	float* secR = (float*)malloc(sizeof(float) * 4);
	float* thR = (float*)malloc(sizeof(float) * 4);
	float* frR = (float*)malloc(sizeof(float) * 4);

	firstR[0] = s;
	firstR[1] = 0;
	firstR[2] = 0;
	firstR[3] = 0;

	secR[0] = 0;
	secR[1] = s;
	secR[2] = 0;
	secR[3] = 0;

	thR[0] = 0;
	thR[1] = 0;
	thR[2] = s;
	thR[3] = 0;

	frR[0] = 0;
	frR[1] = 0;
	frR[2] = 0;
	frR[3] = 1;

	float** result = (float**)malloc(sizeof(float*) * 4);
	result[0] = firstR;
	result[1] = secR;
	result[2] = thR;
	result[3] = frR;

	return result;
}

Vector4 MultiplyMatrixVector(float** mat, Vector4 v) {
	float res[4] = { 0, 0, 0, 0 };
	float vC[4] = { v.x, v.y, v.z, v.w };

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			res[i] += mat[i][j] * vC[j];
		}
	}

	Vector4 vRes = { res[0], res[1], res[2], res[3] };
	return vRes;
}

float** MultiplyMM4(float** matA, float** matB) {
	float* firstR = (float*)calloc(4, sizeof(float));
	float* secR = (float*)calloc(4, sizeof(float));
	float* thR = (float*)calloc(4, sizeof(float));
	float* forR = (float*)calloc(4, sizeof(float));

	float** result = (float**)malloc(sizeof(float*) * 4);
	result[0] = firstR;
	result[1] = secR;
	result[2] = thR;
	result[3] = forR;

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			for (int k = 0; k < 4; k++) {
				result[x][y] += matA[x][k] * matB[k][y];
			}
		}
	}

	return result;
}

float** TransposeMM4(float** mat) {
	float* firstR = (float*)malloc(sizeof(float) * 4);
	float* secR = (float*)malloc(sizeof(float) * 4);
	float* thR = (float*)malloc(sizeof(float) * 4);
	float* frR = (float*)malloc(sizeof(float) * 4);

	float** result = (float**)malloc(sizeof(float*) * 4);
	result[0] = firstR;
	result[1] = secR;
	result[2] = thR;
	result[3] = frR;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i][j] = mat[j][i];
		}
	}

	return result;
}

float** IdentityMM4() {
	float* firstR = (float*)malloc(sizeof(float) * 4);
	float* secR = (float*)malloc(sizeof(float) * 4);
	float* thR = (float*)malloc(sizeof(float) * 4);
	float* frR = (float*)malloc(sizeof(float) * 4);

	firstR[0] = 1;
	firstR[1] = 0;
	firstR[2] = 0;
	firstR[3] = 0;

	secR[0] = 0;
	secR[1] = 1;
	secR[2] = 0;
	secR[3] = 0;

	thR[0] = 0;
	thR[1] = 0;
	thR[2] = 1;
	thR[3] = 0;

	frR[0] = 0;
	frR[1] = 0;
	frR[2] = 0;
	frR[3] = 1;

	float** result = (float**)malloc(sizeof(float*) * 4);
	result[0] = firstR;
	result[1] = secR;
	result[2] = thR;
	result[3] = frR;

	return result;
}

Vector3 ScaleVector(float m, Vector3 v) {
	Vector3 res = { v.x * m, v.y * m, v.z * m };
	return res;
}

float DotProduct(Vector3 v1, Vector3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3 IntersectVectorPlane(Vector3 a, Vector3 b, Plane p) {
	Vector3 side = { b.x - a.x, b.y - a.y, b.z - a.z };
	float t = (-p.dist - DotProduct(p.norm, a)) / (DotProduct(p.norm, side));

	Vector3 res = AddVector(a, ScaleVector(t, side));
	return res;
}

Triangle* ArrayCopyTriangle(Triangle* a, int len) {
	Triangle* newArr = (Triangle*)malloc(sizeof(Triangle) * len);

	for (int i = 0; i < len; i++) {
		newArr[i] = a[i];
	}

	return newArr;
}