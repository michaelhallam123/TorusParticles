#pragma once
#include <iostream>
template <typename T>
struct vec2
{
	T x;
	T y;

	vec2(T a, T b)
		: x(a), y(b) {}

	vec2()
		: x(0), y(0) {}

	vec2 operator+(const vec2<T>& v) const
	{
		return { this->x + v.x, this->y + v.y };
	}

	vec2 operator-(const vec2<T>& v) const
	{
		return { this->x - v.x, this->y - v.y };
	}

	vec2 operator*(T a) const
	{
		return { this->x * a, this->y * a };
	}

	T dot(vec2<T>& v)
	{
		return this->x * v.x + this->y * v.y;
	}

	void Add(const vec2<T>& v2)
	{
		this->x += v2.x;
		this->y += v2.y;
	}

	void Subtract(const vec2<T>& v2)
	{
		this->x -= v2.x;
		this->y -= v2.y;
	}

	void Multiply(const T a)
	{
		this->x *= a;
		this->y *= a;
	}
};

template <typename T>
T distSquared(vec2<T> v1, vec2<T> v2)
{
	vec2<T> diff = v1 - v2;
	return diff.x * diff.x + diff.y * diff.y;
}