#include "Vector.h"

template class Vector2<int>;
template class Vector2<float>;

template<typename T>
Vector2<T>::Vector2() : x(0), y(0) {}

template<typename T>
Vector2<T>::Vector2(T x, T y) : x(x), y(y) {}

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

//template<typename T>
//Vector2<T>& Vector2<T>::operator=(const Vector2<T>& v) {
//	x = v.x;
//	y = v.y;
//	return *this;
//}