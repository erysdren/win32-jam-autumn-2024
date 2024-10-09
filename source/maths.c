
#include "bisected.h"

float wrapf(float value, float mod)
{
	float cmp = (float)(value < 0);
	return cmp * mod + fmod(value, mod) - cmp;
}

float minf(float a, float b)
{
	return a < b ? a : b;
}

float maxf(float a, float b)
{
	return a > b ? a : b;
}

float clampf(float value, float min, float max)
{
	return minf(maxf(value, min), max);
}
