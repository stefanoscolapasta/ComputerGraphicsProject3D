#pragma once

#ifndef COLOR_H
#define COLOR_H

#include "Lib.h"

#define RGB 255.0f


float f_RGB(int value) {
	return value / RGB;
}

vec4 vec_RGBA(int r, int g, int b, int a) {
	return vec4(f_RGB(r), f_RGB(g), f_RGB(b), f_RGB(a));
}

vec4 vec_RGBA(int r, int g, int b) {
	return vec4(f_RGB(r), f_RGB(g), f_RGB(b), f_RGB(255));
}

vec4 vec_RGBA(float r, float g, float b, float a) {
	return vec4(r, g, b, a);
}
vec4 vec_RGBA(float r, float g, float b) {
	return vec4(r, g, b, 1.0f);
}


#endif // !COLOR_H


