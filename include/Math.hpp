#pragma once

#include <iostream>
#include <string>

struct Vector2f
{
	Vector2f()
	:x(0.0f), y(0.0f)
	{}

	Vector2f(float p_x, float p_y)
	:x(p_x), y(p_y)
	{}
	
	float getX() 
	{
		return x;
	}

	float getY()
	{
		return y;
	}

	void print()
	{
		std::cout << x << ", " << y << std::endl;
	}

	Vector2f operator/(const Vector2f& other) const
	{
		return Vector2f(x / other.x, y / other.y);
	}

	Vector2f operator+(const Vector2f& other) const
	{
		return Vector2f(x + other.x, y + other.y);
	}

	Vector2f operator-(const Vector2f& other) const
	{
		return Vector2f(x - other.x, y - other.y);
	}

	float x, y;
};