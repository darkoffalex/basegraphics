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

		// Конструктор структуры
		Vector2D(T X = 0, T Y = 0) :x(X), y(Y) {}

		// Разность
		Vector2D<T> operator-(const Vector2D<T>& other)
		{
			return Vector2D<T>(this->x - other.x, this->y - other.y);
		}

		// Сумма
		Vector2D<T> operator+(const Vector2D<T>& other)
		{
			return Vector2D<T>(this->x + other.x, this->y + other.y);
		}

		// Скалярное произведение (в координатном виде)
		T operator*(const Vector2D<T>& other)
		{
			return (this->x * other.x) + (this->y * other.y);
		}

		// Произведение вектора на число
		Vector2D<T> operator*(float muliplier)
		{
			return Vector2D<T>(this->x * muliplier, this->y * muliplier);
		}

		// Вектор с координатами равными модулю предыдщих координат
		Vector2D<T> GetAbsolute() const
		{
			return Vector2D<T>(std::abs(this->x), std::abs(this->y));
		}

		// Пполучить длину вектора
		float GetLength() const
		{
			return std::sqrt(this->x*this->x + this->y * this->y);
		}

		// Нормализовать вектор (нормальный вектор - вектор заданной длины, как правило равный единице)
		void Normalize(float l)
		{
			*this = (*this)*(l / this->GetLength());
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

		// Конструктор структуры
		Vector3D(T X = 0, T Y = 0, T Z = 0) :x(X), y(Y), z(Z) {}

		// Разность
		Vector3D<T> operator-(const Vector3D<T>& other)
		{
			return Vector3D<T>(this->x - other.x, this->y - other.y, this->z - other.z);
		}

		// Сумма
		Vector3D<T> operator+(const Vector3D<T>& other)
		{
			return Vector3D<T>(this->x + other.x, this->y + other.y, this->z + other.z);
		}

		// Скалярное произведение (в координатном виде)
		T operator*(const Vector3D<T>& other)
		{
			return (this->x * other.x) + (this->y * other.y) + (this->z * other.z);
		}

		// Произведение вектора на число
		Vector3D<T> operator*(float muliplier)
		{
			return Vector3D<T>(this->x * muliplier, this->y * muliplier, this->z * muliplier);
		}

		// Векторное произведение
		Vector3D<T> operator^(const Vector3D<T>& other)
		{
			return Vector3D<T>(this->y*other.z - this->z*other.y, this->z*other.x - this->x*other.z, this->x*other.y - this->y*other.x);
		}

		// Вектор с координатами равными модулю предыдщих координат
		Vector3D<T> GetAbsolute() const
		{
			return Vector3D<T>(std::abs(this->x), std::abs(this->y), std::abs(this->z));
		}

		// Пполучить длину вектора
		float GetLength() const
		{
			return std::sqrt(this->x*this->x + this->y * this->y + this->z * this->z);
		}

		// Нормализовать вектор (нормальный вектор - вектор заданной длины, как правило равный единице)
		void Normalize(float l)
		{
			*this = (*this)*(l / this->GetLength());
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
