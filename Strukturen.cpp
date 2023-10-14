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

	vector operator * (float a, vector b) {
		return vector(a * b.x, a * b.y, a * b.z);
	}

	vertex operator + (vertex a, vector b) {
		return vertex(a.wx + b.x, a.wy + b.y, a.wz + b.z);
	}