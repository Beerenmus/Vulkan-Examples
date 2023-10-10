#pragma once

#include <math.h>

#include <array>
#include <vector>

#include "Strukturen.hpp"

# define M_PI 3.14159265358979323846


	class Matrix
	{
		private:
			std::array<float, 16> m_matrix;

		private:
			void mult(std::array<float, 16> mx);

		public:
			Matrix();
			Matrix(std::array<float, 16> mx);
			void clear();
			void rotate_x(float alpha);
			void rotate_y(float alpha);
			void rotate_z(float alpha);
			void rows(vector a, vector b, vector c);
			void columns(vector a, vector b, vector c);
			void rotate(double alpha, vector v);

			void translate(float tx, float ty, float tz);

			vector operator * (vector v);
			vertex operator * (vertex v);
			localsystem operator * (localsystem ls);


			operator std::array<float, 16>();
            operator std::vector<float> ();
	};

