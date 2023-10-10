#include "Strukturen.hpp"

float vector::length() {
	return sqrtf(x * x + y * y + z * z);
}

void vector::normalized() {

	float length = this->length();

	x /= length;
	y /= length;
	z /= length;
}