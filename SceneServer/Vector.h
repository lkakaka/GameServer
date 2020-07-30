#pragma once

template<size_t dim, typename T>
class VecBase {
public:
	enum { dim_value = dim };
};

template <typename T>
class Vector2 : VecBase<2, T> {
public:
	T x, y;

	Vector2();
	Vector2(T x, T y);

	//float distance(Vector2* pos);
	//Vector2<float> dir(Vector2* pos);
	inline float length() { return sqrt(length_2()); }
	inline float length_2() { return x * x + y * y; }
	Vector2<T> normalize();
	Vector2<T> operator+(Vector2<T>&& v);
	Vector2<T> operator-(Vector2<T>& v);
	Vector2<T> operator*(float mul);
	Vector2<T> operator=(Vector2<T>& v);
};

