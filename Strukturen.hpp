#pragma once

#include <math.h>
#include <stdint.h>

	struct vector {
		
		float x, y, z;

		vector() : x(0), y(0), z(0) {}
		vector(float x, float y, float z) : x(x), y(y), z(z) {}

		float length();
		void normalized();
	};

	struct vertex {

		float wx, wy, wz;

		vertex() : wx(0), wy(0), wz(0) {}
		vertex(float x, float y, float z) : wx(x), wy(y), wz(z) {}
	};

	struct localsystem {

		vertex pos;
		vector right;
		vector up;
		vector sight;

		localsystem() : pos(0, 0, 0), right(1, 0, 0), up(0, 1, 0), sight(0, 0, 1) {}
	};
