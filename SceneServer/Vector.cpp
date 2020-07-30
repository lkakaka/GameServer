#include "Vector.h"
#include <math.h>

template class Vector2<int>;
template class Vector2<float>;

template<typename T>
Vector2<T>::Vector2() : x(0), y(0) {}

template<typename T>
Vector2<T>::Vector2(T x, T y) : x(x), y(y) {}

//template<typename T>
//float Vector2<T>::distance(Vector2* pos) {
//	return sqrt((double(x) - pos->x) * (double(x) - pos->x) + (double(y) - pos->y) * (double(y) - pos->y));
//}

//template<typename T>
//Vector2<float> Vector2<T>::dir(Vector2* pos) {
//	float dis = distance(pos);
//	if (dis <= 1e-6) return Vector2<float>(0.0, 0.0);
//	return Vector2<float>((x - pos->x) / dis, (y - pos->y) / dis);
//}

template<typename T>
Vector2<T> Vector2<T>::normalize() {
	float len = length();
	if (len <= 1e-6) return Vector2<T>(0.0, 0.0);
	return Vector2<T>(x / len, y / len);
}

template<typename T>
Vector2<T> Vector2<T>::operator+(Vector2<T>&& v) {
	return Vector2<T>(x + v.x, y + v.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator-(Vector2<T>& v) {
	return Vector2<T>(x - v.x, y - v.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator*(float mul) {
	return Vector2<T>(x * mul, y * mul);
}

template<typename T>
Vector2<T> Vector2<T>::operator=(Vector2<T>& v) {
	x = v.x;
	y = v.y;
	return *this;
}