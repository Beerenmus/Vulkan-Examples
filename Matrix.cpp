#include "Matrix.hpp"

void Matrix::mult(std::array<float, 16> mx) {

	std::array<float, 16> tm;

	for (unsigned int x = 0; x < 16; x++) {
		
		unsigned int a = x % 4;
		unsigned int b = (x / 4) * 4;

		tm[x] = mx[a] * m_matrix[b] +
				mx[a+4] * m_matrix[b+1] +
				mx[a+8] * m_matrix[b+2] +
				mx[a+12] * m_matrix[b+3];
	}
	
	for (size_t x = 0; x < 16; x++) {
		m_matrix[x] = tm[x];
	}
}

Matrix::Matrix() {
	clear();
}

Matrix::Matrix(std::array<float, 16> mx) : m_matrix(mx) {}

void Matrix::clear() {

	m_matrix[0] = 1;	m_matrix[4] = 0;	m_matrix[ 8] = 0;	m_matrix[12] = 0;
	m_matrix[1] = 0;	m_matrix[5] = 1;	m_matrix[ 9] = 0;	m_matrix[13] = 0;
	m_matrix[2] = 0;	m_matrix[6] = 0;	m_matrix[10] = 1;	m_matrix[14] = 0;
	m_matrix[3] = 0;	m_matrix[7] = 0;	m_matrix[11] = 0;	m_matrix[15] = 1;
}

void Matrix::rotate_x(float alpha) {

	std::array<float, 16> rm;

	double pi = 3.1415926535;
	double s = sin((pi * alpha) / 180.0f);
	double c = cos((pi * alpha) / 180.0f);

	rm[0] = 1;	rm[4] = 0;	rm[8] = 0;   rm[12] = 0;
	rm[1] = 0;	rm[5] = c;	rm[9] = -s;   rm[13] = 0;
	rm[2] = 0;	rm[6] = s;	rm[10] = c;  rm[14] = 0;
	rm[3] = 0;	rm[7] = 0;	rm[11] = 0;  rm[15] = 1;

	mult(rm);
}

void Matrix::rotate_y(float alpha) {

	std::array<float, 16> rm;

	double pi = 3.1415926535;
	double s = sin((pi * alpha) / 180.0f);
	double c = cos((pi * alpha) / 180.0f);

	rm[0] = c;	rm[4] = 0;	rm[ 8] = s;   rm[12] = 0;
	rm[1] = 0;	rm[5] = 1;	rm[ 9] = 0;   rm[13] = 0;
	rm[2] = -s;	rm[6] = 0;	rm[10] = c;  rm[14] = 0;
	rm[3] = 0;	rm[7] = 0;	rm[11] = 0;  rm[15] = 1;

	mult(rm);
}

void Matrix::rotate_z(float alpha) {

	std::array<float, 16> rm;

	double pi = 3.1415926535;
	double s = sin((pi * alpha) / 180.0f);
	double c = cos((pi * alpha) / 180.0f);

	rm[0] = c;	rm[4] = -s;	rm[8] = 0;   rm[12] = 0;
	rm[1] = s;	rm[5] = c;	rm[9] = 0;   rm[13] = 0;
	rm[2] = 0;	rm[6] = 0;	rm[10] = 1;  rm[14] = 0;
	rm[3] = 0;	rm[7] = 0;	rm[11] = 0;  rm[15] = 1;

	mult(rm);
}

void Matrix::rows(vector a, vector b, vector c) {

	std::array<float, 16> tm;

	tm[0] = a.x;	tm[4] = a.y;	tm[ 8] = a.z;  tm[12] = 0;
	tm[1] = b.x;	tm[5] = b.y;	tm[ 9] = b.z;  tm[13] = 0;
	tm[2] = c.x;	tm[6] = c.y;	tm[10] = c.z;  tm[14] = 0;

	tm[3] = 0;	tm[7] = 0;	tm[11] = 0;  tm[15] = 1;

	mult(tm);
}

void Matrix::columns(vector a, vector b, vector c) {

	std::array<float, 16> tm;

	tm[0] = a.x;	tm[4] = b.x;	tm[ 8] = c.x;  tm[12] = 0;
	tm[1] = a.y;	tm[5] = b.y;	tm[ 9] = c.y;  tm[13] = 0;
	tm[2] = a.z;	tm[6] = b.z;	tm[10] = c.z;  tm[14] = 0;

	tm[3] = 0;	tm[7] = 0;	tm[11] = 0;  tm[15] = 1;

	mult(tm);
}

void Matrix::rotate(double alpha, vector v) {

	if (v.length() < 0.0001) return;

	v.normalized();

	double pi = M_PI;
	double s = sin((pi * alpha) / 180.0f);
	double c = cos((pi * alpha) / 180.0f);

	float x = v.x;
	float y = v.y;
	float z = v.z;


	std::array<float, 16> tm;

	tm[0] = x * x * (1-c) + c;			tm[4] = y * x * (1 - c) - z * s;	tm[ 8] = z * x * (1 - c) + y * s;	tm[12] = 0;
	tm[1] = x * y * (1 - c) + z * s;	tm[5] = y * y * (1 - c) + c;		tm[ 9] = z * y * (1 - c) - x * s;	tm[13] = 0;
	tm[2] = x * z * (1 - c) - y * s;	tm[6] = y * z * (1 - c) + x * s;	tm[10] = z * z * (1 - c) + c;		tm[14] = 0;
	tm[3] = 0;							tm[7] = 0;							tm[11] = 0;							tm[15] = 1;

	mult(tm);
}

void Matrix::translate(float tx, float ty, float tz) {

	std::array<float, 16> tm;

	tm[0] = 1;	tm[4] = 0;	tm[ 8] = 0;  tm[12] = tx;
	tm[1] = 0;	tm[5] = 1;	tm[ 9] = 0;  tm[13] = ty;
	tm[2] = 0;	tm[6] = 0;	tm[10] = 1;  tm[14] = tz;
	tm[3] = 0;	tm[7] = 0;	tm[11] = 0;  tm[15] = 1;
	
	mult(tm);
}

vector Matrix::operator*(vector v) {

	float x = v.x * m_matrix[0] + v.y * m_matrix[4] + v.z * m_matrix[ 8];
	float y = v.x * m_matrix[1] + v.y * m_matrix[5] + v.z * m_matrix[ 9];
	float z = v.x * m_matrix[2] + v.y * m_matrix[6] + v.z * m_matrix[10];

	return vector(x, y, z);
}

vertex Matrix::operator*(vertex v) {

	float x = v.wx * m_matrix[0] + v.wy * m_matrix[4] + v.wz * m_matrix[ 8] + m_matrix[12];
	float y = v.wx * m_matrix[1] + v.wy * m_matrix[5] + v.wz * m_matrix[ 9] + m_matrix[13];
	float z = v.wx * m_matrix[2] + v.wy * m_matrix[6] + v.wz * m_matrix[10] + m_matrix[14];

	return vertex(x, y, z);
}

localsystem Matrix::operator*(localsystem ls)
{
	ls.pos = this->operator * (ls.pos);
	ls.right = this->operator * (ls.right);
	ls.up = this->operator * (ls.up);
	ls.sight = this->operator * (ls.sight);

	return ls;
}

Matrix::operator std::array<float, 16>() {
	return m_matrix;
}

Matrix::operator std::vector<float> () {
    return std::vector<float>(m_matrix.begin(), m_matrix.end());
}


