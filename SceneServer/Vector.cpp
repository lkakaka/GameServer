#include "Vector.h"
#include <math.h>

template class Vector<int>;
template class Vector<float>;

template<typename T>
Vector<T>::Vector() : x(0), y(0) {}

template<typename T>
Vector<T>::Vector(T x, T y) : x(x), y(y) {}

template<typename T>
float Vector<T>::distance(Vector* pos) {
	return sqrt((x - pos->x) * (x - pos->x) + (y - pos->y) * (y - pos->y));
}

template<typename T>
Vector<float> Vector<T>::dir(Vector* pos) {
	float dis = distance(pos);
	if (dis <= 1e-6) return Vector<float>(0.0, 0.0);
	return Vector<float>((x - pos->x) / dis, (y - pos->y) / dis);
}

template<typename T>
Vector<float> Vector<T>::normalize() {
	float len = sqrt(x * x + y * y);
	if (len <= 1e-6) return Vector<float>(0.0, 0.0);
	return Vector<float>(x / len, y / len);
}

template<typename T>
Vector<T> Vector<T>::operator+(Vector<float>& v) {
	return Vector<T>(x + v.x, y + v.y);
}

template<typename T>
Vector<T> Vector<T>::operator*(float mul) {
	return Vector<T>(x * mul, y * mul);
}