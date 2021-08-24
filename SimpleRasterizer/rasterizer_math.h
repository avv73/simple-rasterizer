#pragma once
#include <Windows.h>

typedef struct {
	float x;
	float y;
} Vector2;

typedef struct {
	float x;
	float y;
	float z;
} Vector3;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} Vector4;

typedef struct {
	Vector3 vtxIndList;
	COLORREF clr;
} Triangle;

void Swap(Vector2* a, Vector2* b);

Vector3 AddVector(Vector3 v1, Vector3 v2);

Vector3 ScaleVector(float m, Vector3 v); 

float* ArrayConcat(float* a, int aSize, float* b, int bSize);

float** CreateYRotationMatrix(float dg);

float** CreateTranslationMatrix(Vector3 tr);

float** CreateScalingMatrix(float s);

Vector4 MultiplyMatrixVector(float** mat, Vector4 v);

float** MultiplyMM4(float** matA, float** matB);

float** TransposeMM4(float** mat);

float** IdentityMM4();