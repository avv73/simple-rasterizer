#pragma once

typedef struct {
	float x;
	float y;
} Vector2;

typedef struct {
	float x;
	float y;
	float z;
} Vector3;

void Swap(Vector2* a, Vector2* b);

float* ArrayConcat(float* a, int aSize, float* b, int bSize);