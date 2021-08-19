#include "rasterizer_math.h"
#include <stdio.h>
#include <windows.h>

void Swap(Point* a, Point* b) {
	Point temp = *a;
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