#pragma once

typedef struct {
	float x;
	float y;
} Point;

void Swap(Point* a, Point* b);

float* ArrayConcat(float* a, int aSize, float* b, int bSize);