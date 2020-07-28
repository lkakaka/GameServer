#pragma once

template <typename T>
class Vector {

public:
	T x;
	T y;

	Vector();
	Vector(T x, T y);

	float distance(Vector* pos);
	Vector<float> dir(Vector* pos);
	Vector<float> normalize();
	Vector<T> operator+(Vector<float>& v);
	Vector<T> operator*(float mul);
};

