#pragma once

#include <cmath>

namespace gfx
{
	/**
	* \brief Шаблонная структкра для двумерных точек и векторов
	* \tparam T Тип (int по умолчанию)
	*/
	template <typename T = int>
	struct Vector2D
	{
		T x;
		T y;

		Vector2D(T X = 0, T Y = 0) :x(X), y(Y) {}

		Vector2D<T> operator-(const Vector2D<T>& other)
		{
			Vector2D<T> result;
			result.x = this->x - other.x;
			result.y = this->y - other.y;
			return result;
		}

		Vector2D<T> operator+(const Vector2D<T>& other)
		{
			Vector2D<T> result;
			result.x = this->x + other.x;
			result.y = this->y + other.y;
			return result;
		}

		Vector2D<T> GetAbsolute()
		{
			Vector2D<T> result;
			result.x = std::abs(this->x);
			result.y = std::abs(this->y);
			return result;
		}
	};

	/**
	* \brief Шаблонная структкра для трехмерных точек и векторов
	* \tparam T Тип (int по умолчанию)
	*/
	template <typename T = int>
	struct Vector3D
	{
		T x;
		T y;
		T z;

		Vector3D(T X = 0, T Y = 0, T Z = 0) :x(X), y(Y), z(Z) {}

		Vector3D<T> operator-(const Vector3D<T>& other)
		{
			Vector3D<T> result;
			result.x = this->x - other.x;
			result.y = this->y - other.y;
			result.z = this->z - other.z;
			return result;
		}

		Vector2D<T> operator+(const Vector3D<T>& other)
		{
			Vector3D<T> result;
			result.x = this->x + other.x;
			result.y = this->y + other.y;
			result.z = this->z + other.z;
			return result;
		}

		Vector2D<T> GetAbsolute()
		{
			Vector2D<T> result;
			result.x = std::abs(this->x);
			result.y = std::abs(this->y);
			result.z = std::abs(this->z);
			return result;
		}
	};

	/**
	 * \brief Шаблонная структура для описания прямугольной области
	 * \tparam T Тип используемый в вектрах (int по умолчанию)
	 */
	template <typename T = int>
	struct Box2D
	{
		Vector2D<T> topLeft;
		Vector2D<T> bottomRight;

		Box2D(Vector2D<T> inTopLeft = { 0,0 }, Vector2D<T> inBottomRight = {0,0}) : topLeft(inTopLeft), bottomRight(inBottomRight) {}
	};
}
