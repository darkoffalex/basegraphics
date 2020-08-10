/**
 * Мини-библиотека для работы с математикой
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include <cmath>
#include <vector>
#include <algorithm>

#define M_PI 3.14159265358979323846  /* pi */

namespace math
{
    /**
     * 2-мерный вектор или точка на плоскости
     * @tparam T Тип компонентов вектора
     */
    template <typename T = float>
    struct Vec2
    {
        T x, y;

        Vec2() noexcept :x(0.0f),y(0.0f){};
        Vec2(const T& s1, const T& s2) noexcept :x(s1), y(s2) {}

        Vec2<T> operator*(const T& value) const
        {
            return {this->x * value, this->y * value};
        }

        Vec2<T> operator/(const T& value) const
        {
            return {this->x / value, this->y / value};
        }

        Vec2<T> operator*(const Vec2<T>& other) const
        {
            return {this->x * other.x, this->y * other.y};
        }

        Vec2<T> operator+(const Vec2<T>& other) const
        {
            return {this->x + other.x, this->y + other.y};
        }

        Vec2<T> operator-(const Vec2<T>& other) const
        {
            return {this->x - other.x, this->y - other.y};
        }

        Vec2<T> operator-() const
        {
            return {-this->x, -this->y};
        }
    };

    /**
     * 3-мерный вектор или точка в пространстве
     * @tparam T Тип компонентов вектора
     */
    template <typename T = float>
    struct Vec3
    {
        union { public: T x, r; };
        union { public: T y, g; };
        union { public: T z, b; };

        Vec3() noexcept :x(0.0f),y(0.0f),z(0.0f){};
        Vec3(const T& s1, const T& s2, const T& s3) noexcept :x(s1), y(s2), z(s3) {}

        Vec2<T> getVec2(){return {this->x,this->y};}

        Vec3<T> operator*(const T& value) const
        {
            return {this->x * value, this->y * value, this->z * value};
        }

        Vec3<T> operator/(const T& value) const
        {
            return {this->x / value, this->y / value, this->z / value};
        }

        Vec3<T> operator*(const Vec3<T>& other) const
        {
            return {this->x * other.x, this->y * other.y, this->z * other.z};
        }

        Vec3<T> operator+(const Vec3<T>& other) const
        {
            return {this->x + other.x, this->y + other.y, this->z + other.z};
        }

        Vec3<T> operator-(const Vec3<T>& other) const
        {
            return {this->x - other.x, this->y - other.y, this->z - other.z};
        }

        Vec3<T> operator-() const
        {
            return {-this->x, -this->y, -this->z};
        }
    };

    /**
     * 4-мерный вектор или точка в 4D-пространстве
     * @tparam T
     */
    template <typename T = float>
    struct Vec4
    {
        union { public: T x, r; };
        union { public: T y, g; };
        union { public: T z, b; };
        union { public: T w, a; };

        Vec4() noexcept :x(0.0f),y(0.0f),z(0.0f),w(0.0f){};
        Vec4(const T& s1, const T& s2, const T& s3, const T& s4) noexcept :x(s1), y(s2), z(s3), w(s4) {}

        Vec3<T> getVec3(){return {this->x,this->y,this->z};}

        Vec4<T> operator*(const T& value) const
        {
            return {this->x * value, this->y * value, this->z * value, this->w * value};
        }

        Vec4<T> operator/(const T& value) const
        {
            return {this->x / value, this->y / value, this->z / value, this->w / value};
        }

        Vec4<T> operator*(const Vec4<T>& other) const
        {
            return {this->x * other.x, this->y * other.y, this->z * other.z, this->w * other.w};
        }

        Vec4<T> operator+(const Vec4<T>& other) const
        {
            return {this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w};
        }

        Vec4<T> operator-(const Vec4<T>& other) const
        {
            return {this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w};
        }

        Vec4<T> operator-() const
        {
            return {-this->x, -this->y, -this->z, -this->w};
        }
    };

    /**
     * Описывающий объект отрезок/прямоугольник/параллелипипе/тессеракт
     * @tparam T Тип (размерность) точки
     */
    template <typename T = Vec3<float>>
    struct BBox
    {
        T min;
        T max;
    };

    /**
     * Длина 2-мерного вектора
     * @tparam T Тип компонентов вектора
     * @param v Вектор
     * @return Длинна вектора
     */
    template <typename T = float>
    float Length(const Vec2<T>& v)
    {
        return static_cast<float>(sqrtf((v.x * v.x) + (v.y * v.y)));
    }

    /**
     * Длина 3-мерного вектора
     * @tparam T Тип компонентов вектора
     * @param v Вектор
     * @return Длинна вектора
     */
    template <typename T = float>
    float Length(const Vec3<T>& v)
    {
        return static_cast<float>(sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z)));
    }

    /**
     * Нормализация вектора
     * @tparam T Тип компонентов вектора
     * @param v Исходный 2-мерный вектор
     * @return Нормализованный 2-мерный вектор
     */
    template <typename T = float>
    Vec2<T> Normalize(const Vec2<T>& v)
    {
        float len = Length(v);
        if(len > 0) return {v.x / len, v.y / len};
        return {0,0};
    }

    /**
     * Нормализация вектора
     * @tparam T Тип компонентов вектора
     * @param v Исходный 3-мерный вектор
     * @return Нормализованный 3-мерный вектор
     */
    template <typename T = float>
    Vec3<T> Normalize(const Vec3<T>& v)
    {
        float len = Length(v);
        if(len > 0) return {v.x / len, v.y / len, v.z / len};
        return {0,0,0};
    }

    /**
     * Скалярное произведение 2-мерных векторов
     * @tparam T тип компонентов вектора
     * @param v1 Вектор 1
     * @param v2 Вектор 2
     * @return Произведение
     */
    template <typename T = float>
    T Dot(const Vec2<T>& v1, const Vec2<T>& v2)
    {
        return (v1.x + v2.x) * (v1.y + v2.y);
    }

    /**
     * Скалярное произведение 3-мерных векторов
     * @tparam T тип компонентов вектора
     * @param v1 Вектор 1
     * @param v2 Вектор 2
     * @return Произведение
     */
    template <typename T = float>
    T Dot(const Vec3<T>& v1, const Vec3<T>& v2)
    {
        return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    }

    /**
     * Векторное произведение 3-мерных векторов
     * @tparam T тип компонентов вектора
     * @param v1 Вектор 1
     * @param v2 Вектор 2
     * @return Векторное произведение
     */
    template <typename T = float>
    Vec3<T> Cross(const Vec3<T>& v1, const Vec3<T>& v2)
    {
        return {
            (v1.y * v2.z - v1.z * v2.y),
            (v1.z * v2.x - v1.x * v2.z),
            (v1.x * v2.y - v1.y * v2.x)
        };
    }

    /**
     * Получить отраженный вектор относительно нормали
     * @tparam T тип компонентов вектора
     * @param v Исходный вектор
     * @param normal Нормаль
     * @return Отраженный вектор
     */
    template <typename T = float>
    Vec3<T> Reflect(const Vec3<T>& v, const Vec3<T>& normal)
    {
        return v - normal * 2.0f * Dot(v, normal);
    }

    /**
     * Получить преломленный вектор (спизжено из GLM, что происходит - хер знает)
     * @tparam T тип компонентов вектора
     * @param v Исходный вектор
     * @param normal Нормаль
     * @param eta Коэффициент преломления
     * @return Отраженный вектор
     */
    template <typename T = float>
    Vec3<T> Refract(const Vec3<T>& v, const Vec3<T>& normal, float eta)
    {
        auto dot = math::Dot(v,normal);
        auto k = 1.0f - eta * eta * (1.0f - dot * dot);
        if (k < 0.0f) return {0.0f,0.0f,0.0f};
        return (v * eta) - (normal * (eta * dot + sqrtf(k)));
    }

    /**
     * Интерполяция между двумя значениями
     * @tparam T тип значений
     * @param a Первое значение
     * @param b Второе значение
     * @param ratio Соотношение
     * @return Итоговое значение вектора
     */
    template <typename T = float, typename R = float>
    T Mix(const T& a, const T& b, R ratio)
    {
        return a + ((b - a) * ratio);
    }

    /**
     * Матрица 2x2
     * @tparam T Тип ячеек матрицы
     */
    template <typename T = float>
    struct Mat2
    {
        T data[4] = {};

        Mat2() = default;

        explicit Mat2(T val){
            data[0] = val;
            data[3] = val;
        }

        explicit Mat2(const Vec2<T>& i, const Vec2<T>& j)
        {
            data[0] = i.x; data[1] = j.x;
            data[2] = i.y; data[3] = j.y;
        }

        const T* row(size_t row) const {
            return this->data + 2 * row;
        }

        T* operator[](size_t row) {
            return this->data + 2 * row;
        }

        Mat2<T> operator*(const T& value)
        {
            Mat2<T> result = *this;
            for(T &n : result.data){
                n *= value;
            }
            return result;
        }

        Vec2<T> operator*(const Vec2<T>& v) const
        {
            return {
                data[0] * v.x + data[1] * v.y,
                data[2] * v.x + data[3] * v.y
            };
        }

        Mat2 operator*(const Mat2<T>& m) const
        {
            return Mat2(
                (*(this) * Vec2<T>(m.data[0],m.data[2])),
                (*(this) * Vec2<T>(m.data[1],m.data[3]))
            );
        }
    };

    /**
     * Матрица 3x3
     * @tparam T Тип ячеек матрицы
     */
    template <typename T = float>
    struct Mat3
    {
        T data[9] = {};

        Mat3() = default;

        explicit Mat3(T val){
            data[0] = val;
            data[4] = val;
            data[8] = val;
        }

        explicit Mat3(const Vec3<T>& i, const Vec3<T>& j, const Vec3<T>& k)
        {
            data[0] = i.x; data[1] = j.x; data[2] = k.x;
            data[3] = i.y; data[4] = j.y; data[5] = k.y;
            data[6] = i.z; data[7] = j.z; data[8] = k.z;
        }

        const T* row(size_t row) const {
            return this->data + 3 * row;
        }

        T* operator[](size_t row) {
            return this->data + 3 * row;
        }

        Mat3<T> operator*(const T& value)
        {
            Mat3<T> result = *this;
            for(T &n : result.data){
                n *= value;
            }
            return result;
        }

        Vec3<T> operator*(const Vec3<T>& v) const
        {
            return {
                    data[0] * v.x + data[1] * v.y + data[2] * v.z,
                    data[3] * v.x + data[4] * v.y + data[5] * v.z,
                    data[6] * v.x + data[7] * v.y + data[8] * v.z
            };
        }

        Mat3 operator*(const Mat3<T>& m) const
        {
            return Mat3(
                    (*(this) * Vec3<T>(m.data[0],m.data[3],m.data[6])),
                    (*(this) * Vec3<T>(m.data[1],m.data[4],m.data[7])),
                    (*(this) * Vec3<T>(m.data[2],m.data[5],m.data[8]))
            );
        }
    };

    /**
     * Матрица 4x4
     * @tparam T
     */
    template <typename T = float>
    struct Mat4
    {
        T data[16] = {};

        Mat4() = default;

        explicit Mat4(T val){
            data[0] = val;
            data[5] = val;
            data[10] = val;
            data[15] = val;
        }

        explicit Mat4(const Vec4<T>& i, const Vec4<T>& j, const Vec4<T>& k, const Vec4<T>& t)
        {
            data[0] = i.x; data[1] = j.x; data[2] = k.x; data[3] = t.x;
            data[4] = i.y; data[5] = j.y; data[6] = k.y; data[7] = t.y;
            data[8] = i.z; data[9] = j.z; data[10] = k.z; data[11] = t.z;
            data[12] = i.w; data[13] = j.w; data[14] = k.w; data[15] = t.w;
        }

        explicit Mat4(const Mat3<T>& m3, const Vec4<T>& t = {0.0f,0.0f,0.0f,1.0f})
        {
            data[0] = m3.data[0]; data[1] = m3.data[1]; data[2] = m3.data[2]; data[3] = t.x;
            data[4] = m3.data[3]; data[5] = m3.data[4]; data[6] = m3.data[5]; data[7] = t.y;
            data[8] = m3.data[6]; data[9] = m3.data[7]; data[10] = m3.data[8]; data[11] = t.z;
            data[12] = 0.0f;      data[13] = 0.0f;      data[14] = 0.0f;      data[15] = t.w;
        }

        const T* row(size_t row) const {
            return this->data + 4 * row;
        }

        T* operator[](size_t row) {
            return this->data + 4 * row;
        }

        Mat4<T> operator*(const T& value)
        {
            Mat4<T> result = *this;
            for(T &n : result.data){
                n *= value;
            }
            return result;
        }

        Vec4<T> operator*(const Vec4<T>& v) const
        {
            return {
                    data[0] * v.x + data[1] * v.y + data[2] * v.z + data[3] * v.w,
                    data[4] * v.x + data[5] * v.y + data[6] * v.z + data[7] * v.w,
                    data[8] * v.x + data[9] * v.y + data[10] * v.z + data[11] * v.w,
                    data[12] * v.x + data[13] * v.y + data[14] * v.z + data[15] * v.w,
            };
        }

        Mat4 operator*(const Mat4<T>& m) const
        {
            return Mat4(
                    (*(this) * Vec4<T>(m.data[0],m.data[4],m.data[8],m.data[12])),
                    (*(this) * Vec4<T>(m.data[1],m.data[5],m.data[9],m.data[13])),
                    (*(this) * Vec4<T>(m.data[2],m.data[6],m.data[10],m.data[14])),
                    (*(this) * Vec4<T>(m.data[3],m.data[7],m.data[11],m.data[15]))
            );
        }
    };

    /**
     * Транспонировать матрицу 2x2
     * @tparam T Тип ячеек матрицы
     * @param m Исходная матрица
     * @return Транспонированная матрица
     */
    template <typename T = float>
    Mat2<T> Transpose(const Mat2<T>& m)
    {
        return Mat2<T>(
                {m.data[0],m.data[1]},
                {m.data[2],m.data[3]}
        );
    }

    /**
     * Транспонировать матрицу 3x3
     * @tparam T Тип ячеек матрицы
     * @param m Исходная матрица
     * @return Транспонированная матрица
     */
    template <typename T = float>
    Mat3<T> Transpose(const Mat3<T>& m)
    {
        return Mat3<T>(
                {m.data[0],m.data[1],m.data[2]},
                {m.data[3],m.data[4],m.data[5]},
                {m.data[6],m.data[7],m.data[8]}
        );
    }

    /**
     * Транспонировать матрицу 4x4
     * @tparam T Тип ячеек матрицы
     * @param m Исходная матрица
     * @return Транспонированная матрица
     */
    template <typename T = float>
    Mat4<T> Transpose(const Mat4<T>& m)
    {
        return Mat4<T>(
                {m.data[0],m.data[1],m.data[2],m.data[3]},
                {m.data[4],m.data[5],m.data[6],m.data[7]},
                {m.data[8],m.data[9],m.data[10],m.data[11]},
                {m.data[12],m.data[13],m.data[14],m.data[15]}
        );
    }

    /**
     * Определитель 2-го порядка
     * @tparam T Тип ячеек матрицы
     * @param m Матрица 2x2
     * @return Значение определителя
     */
    template <typename T = float>
    T Determinant(const Mat2<T>& m)
    {
        return (m.row(0)[0] * m.row(1)[1]) - (m.row(0)[1] * m.row(1)[0]);
    }

    /**
     * Определитель 3-го порядка
     * @tparam T Тип ячеек матрицы
     * @param m Матрица 3x3
     * @return Значение определителя
     */
    template <typename T = float>
    T Determinant(const Mat3<T>& m)
    {
        return (m.row(0)[0] * m.row(1)[1] * m.row(2)[2])
               + (m.row(0)[1] * m.row(1)[2] * m.row(2)[0])
               + (m.row(0)[2] * m.row(1)[0] * m.row(2)[1])
               - (m.row(0)[2] * m.row(1)[1] * m.row(2)[0])
               - (m.row(0)[0] * m.row(1)[2] * m.row(2)[1])
               - (m.row(0)[1] * m.row(1)[0] * m.row(2)[2]);
    }

    /**
     * Определитель 4-го порядка
     * @tparam T Тип ячеек матрицы
     * @param m Матрица 4x4
     * @return Значение определителя
     */
    template <typename T = float>
    T Determinant(const Mat4<T>& m)
    {
        return (m.row(0)[0] * Determinant(Mat3<T>({m.row(1)[1],m.row(2)[1],m.row(3)[1]},{m.row(1)[2],m.row(2)[2],m.row(3)[2]},{m.row(1)[3],m.row(2)[3],m.row(3)[3]})))
               - (m.row(0)[1] * Determinant(Mat3<T>({m.row(1)[0],m.row(2)[0],m.row(3)[0]},{m.row(1)[2],m.row(2)[2],m.row(3)[2]},{m.row(1)[3],m.row(2)[3],m.row(3)[3]})))
               + (m.row(0)[2] * Determinant(Mat3<T>({m.row(1)[0],m.row(2)[0],m.row(3)[0]},{m.row(1)[1],m.row(2)[1],m.row(3)[1]},{m.row(1)[3],m.row(2)[3],m.row(3)[3]})))
               - (m.row(0)[3] * Determinant(Mat3<T>({m.row(1)[0],m.row(2)[0],m.row(3)[0]},{m.row(1)[1],m.row(2)[1],m.row(3)[1]},{m.row(1)[2],m.row(2)[2],m.row(3)[2]})));
    }


    /**
     * Обратная матрица для матрицы 2x2
     * @tparam T Тип ячеек матрицы
     * @param m Исходная матрица 2x2
     * @return Обратная матрица
     */
    template <typename T = float>
    Mat2<T> Inverse(const Mat2<T>& m)
    {
        Mat2<T> result;
        auto det = Determinant(m);

        if(det != 0){
            result = Mat2<T>({m.data[3],-m.data[2]},{-m.data[1],m.data[0]}) * (1 / det);
        }

        return result;
    }

    /**
     * Обратная матрица для матрицы 3x3
     * @tparam T Тип ячеек матрицы
     * @param m Исходная матрица 3x3
     * @return Обратная матрица
     */
    template <typename T = float>
    Mat3<T> Inverse(const Mat3<T>& m)
    {
        Mat3<T> result;
        auto det = Determinant(m);

        if(det != 0){
            T detInverse = 1 / det;
            result[0][0] = (m.row(1)[1] * m.row(2)[2] - m.row(2)[1] * m.row(1)[2]) * detInverse;
            result[0][1] = (m.row(0)[2] * m.row(2)[1] - m.row(0)[1] * m.row(2)[2]) * detInverse;
            result[0][2] = (m.row(0)[1] * m.row(1)[2] - m.row(0)[2] * m.row(1)[1]) * detInverse;
            result[1][0] = (m.row(1)[2] * m.row(2)[0] - m.row(1)[0] * m.row(2)[2]) * detInverse;
            result[1][1] = (m.row(0)[0] * m.row(2)[2] - m.row(0)[2] * m.row(2)[0]) * detInverse;
            result[1][2] = (m.row(1)[0] * m.row(0)[2] - m.row(0)[0] * m.row(1)[2]) * detInverse;
            result[2][0] = (m.row(1)[0] * m.row(2)[1] - m.row(2)[0] * m.row(1)[1]) * detInverse;
            result[2][1] = (m.row(2)[0] * m.row(0)[1] - m.row(0)[0] * m.row(2)[1]) * detInverse;
            result[2][2] = (m.row(0)[0] * m.row(1)[1] - m.row(1)[0] * m.row(0)[1]) * detInverse;
        }

        return result;
    }

    /**
     * Обратная матрица для матрицы 4x4
     * @tparam T Тип ячеек матрицы
     * @param m Исходная матрица 4x4
     * @return Обратная матрица
     */
    template <typename T = float>
    Mat4<T> Inverse(const Mat4<T>& m)
    {
        Mat4<T> inv;

        inv.data[0] = m.data[5]  * m.data[10] * m.data[15] -
                 m.data[5]  * m.data[11] * m.data[14] -
                 m.data[9]  * m.data[6]  * m.data[15] +
                 m.data[9]  * m.data[7]  * m.data[14] +
                 m.data[13] * m.data[6]  * m.data[11] -
                 m.data[13] * m.data[7]  * m.data[10];

        inv.data[4] = -m.data[4]  * m.data[10] * m.data[15] +
                 m.data[4]  * m.data[11] * m.data[14] +
                 m.data[8]  * m.data[6]  * m.data[15] -
                 m.data[8]  * m.data[7]  * m.data[14] -
                 m.data[12] * m.data[6]  * m.data[11] +
                 m.data[12] * m.data[7]  * m.data[10];

        inv.data[8] = m.data[4]  * m.data[9] * m.data[15] -
                 m.data[4]  * m.data[11] * m.data[13] -
                 m.data[8]  * m.data[5] * m.data[15] +
                 m.data[8]  * m.data[7] * m.data[13] +
                 m.data[12] * m.data[5] * m.data[11] -
                 m.data[12] * m.data[7] * m.data[9];

        inv.data[12] = -m.data[4]  * m.data[9] * m.data[14] +
                  m.data[4]  * m.data[10] * m.data[13] +
                  m.data[8]  * m.data[5] * m.data[14] -
                  m.data[8]  * m.data[6] * m.data[13] -
                  m.data[12] * m.data[5] * m.data[10] +
                  m.data[12] * m.data[6] * m.data[9];

        inv.data[1] = -m.data[1]  * m.data[10] * m.data[15] +
                 m.data[1]  * m.data[11] * m.data[14] +
                 m.data[9]  * m.data[2] * m.data[15] -
                 m.data[9]  * m.data[3] * m.data[14] -
                 m.data[13] * m.data[2] * m.data[11] +
                 m.data[13] * m.data[3] * m.data[10];

        inv.data[5] = m.data[0]  * m.data[10] * m.data[15] -
                 m.data[0]  * m.data[11] * m.data[14] -
                 m.data[8]  * m.data[2] * m.data[15] +
                 m.data[8]  * m.data[3] * m.data[14] +
                 m.data[12] * m.data[2] * m.data[11] -
                 m.data[12] * m.data[3] * m.data[10];

        inv.data[9] = -m.data[0]  * m.data[9] * m.data[15] +
                 m.data[0]  * m.data[11] * m.data[13] +
                 m.data[8]  * m.data[1] * m.data[15] -
                 m.data[8]  * m.data[3] * m.data[13] -
                 m.data[12] * m.data[1] * m.data[11] +
                 m.data[12] * m.data[3] * m.data[9];

        inv.data[13] = m.data[0]  * m.data[9] * m.data[14] -
                  m.data[0]  * m.data[10] * m.data[13] -
                  m.data[8]  * m.data[1] * m.data[14] +
                  m.data[8]  * m.data[2] * m.data[13] +
                  m.data[12] * m.data[1] * m.data[10] -
                  m.data[12] * m.data[2] * m.data[9];

        inv.data[2] = m.data[1]  * m.data[6] * m.data[15] -
                 m.data[1]  * m.data[7] * m.data[14] -
                 m.data[5]  * m.data[2] * m.data[15] +
                 m.data[5]  * m.data[3] * m.data[14] +
                 m.data[13] * m.data[2] * m.data[7] -
                 m.data[13] * m.data[3] * m.data[6];

        inv.data[6] = -m.data[0]  * m.data[6] * m.data[15] +
                 m.data[0]  * m.data[7] * m.data[14] +
                 m.data[4]  * m.data[2] * m.data[15] -
                 m.data[4]  * m.data[3] * m.data[14] -
                 m.data[12] * m.data[2] * m.data[7] +
                 m.data[12] * m.data[3] * m.data[6];

        inv.data[10] = m.data[0]  * m.data[5] * m.data[15] -
                  m.data[0]  * m.data[7] * m.data[13] -
                  m.data[4]  * m.data[1] * m.data[15] +
                  m.data[4]  * m.data[3] * m.data[13] +
                  m.data[12] * m.data[1] * m.data[7] -
                  m.data[12] * m.data[3] * m.data[5];

        inv.data[14] = -m.data[0]  * m.data[5] * m.data[14] +
                  m.data[0]  * m.data[6] * m.data[13] +
                  m.data[4]  * m.data[1] * m.data[14] -
                  m.data[4]  * m.data[2] * m.data[13] -
                  m.data[12] * m.data[1] * m.data[6] +
                  m.data[12] * m.data[2] * m.data[5];

        inv.data[3] = -m.data[1] * m.data[6] * m.data[11] +
                 m.data[1] * m.data[7] * m.data[10] +
                 m.data[5] * m.data[2] * m.data[11] -
                 m.data[5] * m.data[3] * m.data[10] -
                 m.data[9] * m.data[2] * m.data[7] +
                 m.data[9] * m.data[3] * m.data[6];

        inv.data[7] = m.data[0] * m.data[6] * m.data[11] -
                 m.data[0] * m.data[7] * m.data[10] -
                 m.data[4] * m.data[2] * m.data[11] +
                 m.data[4] * m.data[3] * m.data[10] +
                 m.data[8] * m.data[2] * m.data[7] -
                 m.data[8] * m.data[3] * m.data[6];

        inv.data[11] = -m.data[0] * m.data[5] * m.data[11] +
                  m.data[0] * m.data[7] * m.data[9] +
                  m.data[4] * m.data[1] * m.data[11] -
                  m.data[4] * m.data[3] * m.data[9] -
                  m.data[8] * m.data[1] * m.data[7] +
                  m.data[8] * m.data[3] * m.data[5];

        inv.data[15] = m.data[0] * m.data[5] * m.data[10] -
                  m.data[0] * m.data[6] * m.data[9] -
                  m.data[4] * m.data[1] * m.data[10] +
                  m.data[4] * m.data[2] * m.data[9] +
                  m.data[8] * m.data[1] * m.data[6] -
                  m.data[8] * m.data[2] * m.data[5];

        auto det = m.data[0] * inv.data[0] + m.data[1] * inv.data[4] + m.data[2] * inv.data[8] + m.data[3] * inv.data[12];

        if (det == 0) return Mat4<T>();

        return inv * (1/det);
    }

    /**
     * Вращать вектор или точку вокуруг оси X
     * @tparam T Тип компонентов
     * @param v Вектор или точка
     * @param angle Угол
     * @return Вектор или точка после вращения
     */
    template <typename T = float>
    Vec3<T> RotateAroundX(const Vec3<T>& v, const float& angle)
    {
        auto angleRad = angle * (M_PI / 180.0f);

        return {
            v.x,
            (v.y * cosf(angleRad)) - (v.z * sinf(angleRad)),
            (v.y * sinf(angleRad)) + (v.z * cosf(angleRad))
        };
    }

    /**
     * Вращать вектор или точку вокуруг оси Y
     * @tparam T Тип компонентов
     * @param v Вектор или точка
     * @param angle Угол
     * @return Вектор или точка после вращения
     */
    template <typename T = float>
    Vec3<T> RotateAroundY(const Vec3<T>& v, const float& angle)
    {
        auto angleRad = angle * (M_PI / 180.0f);

        return {
                (v.x * cosf(angleRad)) + (v.z * sinf(angleRad)),
                v.y,
                -(v.x * sinf(angleRad)) + (v.z * cosf(angleRad))
        };
    }

    /**
     * Вращать вектор или точку вокуруг оси Z
     * @tparam T Тип компонентов
     * @param v Вектор или точка
     * @param angle Угол
     * @return Вектор или точка после вращения
     */
    template <typename T = float>
    Vec3<T> RotateAroundZ(const Vec3<T>& v, const float& angle)
    {
        auto angleRad = angle * (M_PI / 180.0f);

        return {
                (v.x * cosf(angleRad)) - (v.y * sinf(angleRad)),
                (v.x * sinf(angleRad)) + (v.y * cosf(angleRad)),
                v.z
        };
    }

    /**
     * Вращать вектор или точку на плоскости
     * @tparam T Тип компонентов
     * @param v Вектор или точка
     * @param angle Угол
     * @return Вектор или точка после вращения
     */
    template <typename T = float>
    Vec2<T> Rotate2D(const Vec2<T>& v, const float& angle)
    {
        auto angleRad = angle * (M_PI / 180.0f);

        return {
                (v.x * cosf(angleRad)) - (v.y * sinf(angleRad)),
                (v.x * sinf(angleRad)) + (v.y * cosf(angleRad))
        };
    }


    /**
     * Получить матрицу поворта вокруг оси X
     * @tparam T Тип компонентов
     * @param angle Угол в градусах
     * @return Матрица 3*3
     */
    template <typename T = float>
    Mat3<T> GetRotationMatX(const float& angle)
    {
        auto angleRad = angle * (M_PI / 180.0f);
        return Mat3<T>(
                {1.0f,0.0f,0.0f},
                {0.0f,cosf(angleRad),sinf(angleRad)},
                {0.0f,-sinf(angleRad),cosf(angleRad)});
    }

    /**
     * Получить матрицу поворта вокруг оси Y
     * @tparam T Тип компонентов
     * @param angle Угол в градусах
     * @return Матрица 3*3
     */
    template <typename T = float>
    Mat3<T> GetRotationMatY(const float& angle)
    {
        auto angleRad = angle * (M_PI / 180.0f);
        return Mat3<T>(
                {cosf(angleRad),0.0f,-sinf(angleRad)},
                {0.0f,1.0f,0.0f},
                {sinf(angleRad),0.0f,cosf(angleRad)});
    }

    /**
     * Получить матрицу поворта вокруг оси Z
     * @tparam T Тип компонентов
     * @param angle Угол в градусах
     * @return Матрица 3*3
     */
    template <typename T = float>
    Mat3<T> GetRotationMatZ(const float& angle)
    {
        auto angleRad = angle * (M_PI / 180.0f);
        return Mat3<T>(
                {cosf(angleRad),sinf(angleRad),0.0f},
                {-sinf(angleRad),cosf(angleRad),0.0f},
                {0.0f,0.0f,1.0f});
    }

    /**
     * Получить матрицу поворота вокруг всех осей
     * @tparam T Тип компонентов
     * @param angles Углы (в градусах)
     * @return Матрица 3*3
     */
    template <typename T = float>
    Mat3<T> GetRotationMat(const Vec3<T>& angles)
    {
        return  GetRotationMatY(angles.y) * GetRotationMatX(angles.x) * GetRotationMatZ(angles.z);
    }

    /**
     * Получить матрицу поворота вокруг всех осей (4x4)
     * @tparam T Тип компонентов
     * @param angles Углы (в градусах)
     * @return Матрица 4*4
     */
    template <typename T = float>
    Mat4<T> GetRotationMat4(const Vec3<T>& angles)
    {
        auto rotMat3 = GetRotationMat(angles);
        return math::Mat4<T>(
                {rotMat3[0][0],rotMat3[0][1],rotMat3[0][2],0},
                {rotMat3[1][0],rotMat3[1][1],rotMat3[1][2],0},
                {rotMat3[2][0],rotMat3[2][1],rotMat3[2][2],0},
                {0,0,0,1});
    }

    /**
     * Получить матрицу масштабирования
     * @tparam T Тип компонентов
     * @param scale Масштаб по всем осям
     * @return Матрица 3*3
     */
    template <typename T = float>
    Mat3<T> GetScaleMat(const Vec3<T>& scale)
    {
        return Mat3<T>({scale.x,0.0f,0.0f},{0.0f,scale.y,0.0f},{0.0f,0.0f,scale.z});
    }

    /**
     * Получить матрицу масштабирования (4x4)
     * @tparam T Тип компонентов
     * @param scale Масштаб по всем осям
     * @return Матрица 4*4
     */
    template <typename T = float>
    Mat4<T> GetScaleMat4(const Vec3<T>& scale)
    {
        auto scaleMat3 = GetScaleMat(scale);
        return math::Mat4<T>(
                {scaleMat3[0][0], scaleMat3[0][1], scaleMat3[0][2], 0},
                {scaleMat3[1][0], scaleMat3[1][1], scaleMat3[1][2], 0},
                {scaleMat3[2][0], scaleMat3[2][1], scaleMat3[2][2], 0},
                {0,0,0,1});
    }

    /**
     * Получить матрицу смещения (4x4)
     * @tparam T Тип компонентов
     * @param v Вектор смещения
     * @return Матрица 4*4
     */
    template <typename T = float>
    Mat4<T> GetTranslationMat4(const Vec3<T>& v)
    {
        return math::Mat4<T>(
                {1,0,0,0},
                {0,1,0,0},
                {0,0,1,0},
                {v.x,v.y,v.z,1});
    }

    /**
     * Ортогональная проекция точки
     * @tparam T Тип компонентов
     * @param point Исходная точка
     * @param left Левая грань видимой области (в NDC-координатах)
     * @param right Права грань видимой области (в NDC-координатах)
     * @param bottom Нижняя грань видимой области (в NDC-координатах)
     * @param top Верхняя грань видимой области (в NDC-координатах)
     * @param zNear Ближняя грань видимой области (0 для Z значения)
     * @param zFar Дальняя грань видимой области (1 для Z значения)
     * @param aspectRatio Пропорции экрана
     * @return Спроецированная точка
     */
    template <typename T = float>
    Vec3<T> ProjectOrthogonal(const Vec3<T>& point, T left, T right, T bottom, T top, T zNear, T zFar, T aspectRatio = 1)
    {
        left *= aspectRatio;
        right *= aspectRatio;

        return {
            ((point.x - left) / ((right - left) / static_cast<T>(2))) - static_cast<T>(1),
            ((point.y - bottom) / ((top - bottom) / static_cast<T>(2))) - static_cast<T>(1),
            (point.z + zNear) / (zNear - zFar)
        };
    }

    /**
     * Перспективная проекция точки
     * @tparam T Тип компонентов
     * @param point Исходная точка
     * @param fov Угол обзора
     * @param zNear Ближняя грань видимой области (0 для Z значения)
     * @param zFar  Дальняя грань видимой области (1 для Z значения)
     * @param aspectRatio Пропорции экрана
     * @return Спроецированная точка
     */
    template <typename T = float>
    Vec3<T> ProjectPerspective(const Vec3<T>& point, const float& fov, T zNear, T zFar, T aspectRatio = 1)
    {
        auto halfFovRad = (fov / 2) * (M_PI / 180.0f);

        return {
                (point.x * (-1/(tanf(halfFovRad) * aspectRatio))) / point.z,
                (point.y * (-1/tanf(halfFovRad))) / point.z,
                //(point.z + zNear) / (zNear - zFar)
                ((point.z * (-zFar / (zNear - zFar))) + ((zFar * zNear) / (zFar - zNear))) / point.z
        };
    }

    /**
     * Получить матрицу ортогональной проекции
     * @tparam T Тип компонентов
     * @param left Левая грань видимой области (в NDC-координатах)
     * @param right Права грань видимой области (в NDC-координатах)
     * @param bottom Нижняя грань видимой области (в NDC-координатах)
     * @param top Верхняя грань видимой области (в NDC-координатах)
     * @param zNear Ближняя грань видимой области (0 для Z значения)
     * @param zFar Дальняя грань видимой области (1 для Z значения)
     * @param aspectRatio Пропорции экрана
     * @return Матрица 4*4
     */
    template <typename T = float>
    Mat4<T> GetProjectionMatOrthogonal(T left, T right, T bottom, T top, T zNear, T zFar, T aspectRatio = 1)
    {
        left *= aspectRatio;
        right *= aspectRatio;

        auto dx = (right - left);
        auto dy = (top - bottom);
        auto dz = (zFar - zNear);

        return Mat4<T>(
                {(static_cast<T>(2) / dx),0,0,0},
                {0,(static_cast<T>(2) / dy),0,0},
                {0,0,-(static_cast<T>(1) / dz),0},
                {-(right + left) / dx, -(top + bottom) / dy,-zNear / dz,1});
    }

    /**
     * Получить матрицу перспективной проекции
     * @tparam T Тип компонентов
     * @param fov Угол обзора
     * @param aspectRatio Пропорции экрана
     * @param zNear Ближняя грань видимой области (0 для Z значения)
     * @param zFar Дальняя грань видимой области (1 для Z значения)
     * @return Матрица 4*4
     */
    template <typename T = float>
    Mat4<T> GetProjectionMatPerspective(T fov, T aspectRatio, T zNear, T zFar)
    {
        auto halfFovRad = (fov / 2) * (M_PI / 180.0f);

        return Mat4<T>(
                {-static_cast<T>(1)/(tanf(halfFovRad) * aspectRatio), 0, 0, 0},
                {0,-static_cast<T>(1)/tanf(halfFovRad), 0, 0},
                {0,0,-zFar / (zNear - zFar),1},
                {0,0,(zFar * zNear) / (zFar - zNear),0});
    }

    /**
     * Перевод координат точки из клип-пространства в пространство экрана
     * @tparam T Тип компонентов
     * @param point Исходнач точка в клип-пространстве
     * @param width Ширина экрана в пикселях
     * @param height Высота экрана в пикселях
     * @return Точка в координатах экрана (верхний левый угол - начало координат)
     */
    template <typename T = float>
    Vec2<int> NdcToScreen(const Vec2<T>& point, unsigned width, unsigned height)
    {
        return {
                static_cast<int>(((point.x + 1.0f)/2.0f) * (width-1)),
                static_cast<int>(((-point.y + 1.0f)/2.0f) * (height-1)),
        };
    }
}