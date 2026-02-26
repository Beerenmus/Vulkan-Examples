//
// Created by marco on 22.02.26.
//

#ifndef MATRIX_H
#define MATRIX_H

#include <array>
#include <cmath>

template<typename T>
class Matrix {

    private:
        std::array<T, 16> mx {};

    private:
        void multiply(std::array<T, 16> m);

    public:
        Matrix();

        void clear();

        void rotate_x(T angle);
        void rotate_y(T angle);
        void rotate_z(T angle);

        void translate(T x, T y, T z);
        void scale(T x, T y, T z);
};

using Matrix4f = Matrix<float>;
using Matrix4d = Matrix<double>;

template<typename T>
Matrix<T>::Matrix() {
    clear();
}

template<typename T>
void Matrix<T>::multiply(std::array<T, 16> m) {

    std::array<T, 16> result = {};

    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            result[col * 4 + row] =
                mx[0 * 4 + row] * m[col * 4 + 0] +
                mx[1 * 4 + row] * m[col * 4 + 1] +
                mx[2 * 4 + row] * m[col * 4 + 2] +
                mx[3 * 4 + row] * m[col * 4 + 3];
        }
    }

    mx = result;
}


template<typename T>
void Matrix<T>::clear() {

    mx = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

template<typename T>
void Matrix<T>::rotate_x(T angle)
{
    T c = std::cos(angle);
    T s = std::sin(angle);

    std::array<T, 16> rm = {
        1, 0, 0, 0,
        0, c, s, 0,
        0,-s, c, 0,
        0, 0, 0, 1
    };

    multiply(rm);
}

template<typename T>
void Matrix<T>::rotate_y(T angle)
{
    T c = std::cos(angle);
    T s = std::sin(angle);

    std::array<T, 16> rm = {
        c, 0,-s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1
   };

    multiply(rm);
}

template<typename T>
void Matrix<T>::rotate_z(T angle)
{
    T c = std::cos(angle);
    T s = std::sin(angle);

    std::array<T, 16> rm = {
        c, s, 0, 0,
       -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    multiply(rm);
}

template<typename T>
void Matrix<T>::translate(T x, T y, T z)
{
    std::array<T, 16> tm = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    };

    multiply(tm); // M = M * T
}

template<typename T>
void Matrix<T>::scale(T x, T y, T z)
{
    std::array<T, 16> sm = {
        x, 0,  0,  0,
        0,  y, 0,  0,
        0,  0,  z, 0,
        0,  0,  0,  1
    };

    multiply(sm);
}

#endif //MATRIX_H