#pragma once

// Simple 2-dimensional vector struct

template <typename T>
struct Vec2
{
	T x;
	T y;

	Vec2(T a, T b)
		: x(a), y(b) {}

	Vec2()
		: x(0), y(0) {}

	Vec2 operator+(const Vec2<T>& v) const
	{
		return { this->x + v.x, this->y + v.y };
	}

	Vec2 operator-(const Vec2<T>& v) const
	{
		return { this->x - v.x, this->y - v.y };
	}

	Vec2 operator*(T a) const
	{
		return { this->x * a, this->y * a };
	}

	Vec2 operator/(T a) const
	{
		return { this->x / a, this->y / a };
	}

	Vec2& operator+=(const Vec2<T>& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	Vec2& operator-=(const Vec2<T>& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	T dot(Vec2<T>& v)
	{
		return this->x * v.x + this->y * v.y;
	}

	void add(const Vec2<T>& v2)
	{
		this->x += v2.x;
		this->y += v2.y;
	}

	void subtract(const Vec2<T>& v2)
	{
		this->x -= v2.x;
		this->y -= v2.y;
	}

	void multiply(const T a)
	{
		this->x *= a;
		this->y *= a;
	}
};

template <typename T>
T distSquared(Vec2<T> v1, Vec2<T> v2)
{
	Vec2<T> diff = v1 - v2;
	return diff.x * diff.x + diff.y * diff.y;
}