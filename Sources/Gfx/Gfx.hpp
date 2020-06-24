#pragma once

#include "ImageBuffer.hpp"

#include <cmath>
#include <functional>

namespace gfx
{
    // Уровни проверки на выход за пределы буфера
    constexpr std::uint_fast8_t SAFE_CHECK_DISABLE     {0};
    constexpr std::uint_fast8_t SAFE_CHECK_KEY_POINTS  {1u << 0u};
    constexpr std::uint_fast8_t SAFE_CHECK_ALL_POINTS  {1u << 1u};

    /**
     * Точка на плокскости
     * @tparam T ип компонентов 2D-точки
     */
    template <typename T>
    struct Point2D
    {
        T x;
        T y;
    };

    /**
     * Описывающий прямоугольник
     * @tparam T Тип компонентов 2D-точек
     */
    template <typename T>
    struct BBox2D
    {
        Point2D<T> min;
        Point2D<T> max;
    };

    /**
     * Задать конкретной точке конкретный цвет
     * @tparam T Тип пикселей в буфере изображения
     * @param imageBuffer Указатель на объект буфера изображения
     * @param x Координаты по X
     * @param y Координаты по Y
     * @param color Цвет
     * @param safeChecks Осуществлять проверку на выход за пределы
     */
    template<typename T>
    void SetPint(ImageBuffer<T>* imageBuffer, int x, int y, const T& color, bool safeChecks = true)
    {
        if(safeChecks){
            if(!imageBuffer->isPointIn(x,y)) return;
        }

        (*imageBuffer)[y][x] = color;
    }

    /**
     * Задать конкретной точке конкретный цвет (учитывая глубину точки)
     * @tparam T0 Тип пикселей в буфере изображения
     * @tparam T1 Тип пикселей в буфере глубины
     * @param imageBuffer Указатель на объект буфера изображения
     * @param depthBuffer Указатель на объект буфера глубины
     * @param x Координаты по X
     * @param y Координаты по Y
     * @param color Цвет
     * @param depth Глубина
     * @param safeChecks Осуществлять проверку на выход за пределы
     */
    template<typename T0, typename T1>
    void SetPoint(ImageBuffer<T0>* imageBuffer,
                  ImageBuffer<T1>* depthBuffer,
                  int x,
                  int y,
                  const T0& color,
                  const T1 depth,
                  bool safeChecks = true)
    {
        if(safeChecks){
            if(!imageBuffer->isPointIn(x,y)) return;
            if(!depthBuffer->isPointIn(x,y)) return;
        }

        if(depth < (*depthBuffer)[y][x]){
            (*imageBuffer)[y][x] = color;
            (*depth)[y][x] = depth;
        }
    }

    /**
     * Растеризация линии в буфере изображения (алгоритм Брезенхэма)
     * @tparam T Тип пикселей в буфере изображения
     * @param imageBuffer Указатель на объект буфера изображения
     * @param x0 Координаты точки начала по X
     * @param y0 Координаты точки начала по Y
     * @param x1 Координаты точки конца по X
     * @param y1 Координаты точки конца по Y
     * @param color Цвет линии
     * @param safeChecks Осуществлять проверку на выход за пределы
     */
    template<typename T>
    void SetLine(ImageBuffer<T>* imageBuffer,
                 int x0, int y0,
                 int x1, int y1,
                 const T& color,
                 std::uint_fast8_t safeChecks = SAFE_CHECK_KEY_POINTS)
    {
        if(safeChecks & SAFE_CHECK_KEY_POINTS){
            if(!imageBuffer->isPointIn(x0,y0)) return;
            if(!imageBuffer->isPointIn(x1,y1)) return;
        }

        bool axisSwapped = false;
        if(abs(static_cast<int>(x1) - static_cast<int>(x0)) < abs(static_cast<int>(y1) - static_cast<int>(y0)))
        {
            axisSwapped = true;
            std::swap(x0,y0);
            std::swap(x1,y1);
        }

        int deltaX = abs(static_cast<int>(x1) - static_cast<int>(x0));
        int deltaY = abs(static_cast<int>(y1) - static_cast<int>(y0));

        if (x0 > x1){
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        int error = 0;
        int deltaErr = (deltaY + 1);

        int y = y0;
        int dirY = y1 - y0;

        if (dirY > 0) dirY = 1;
        if (dirY < 0) dirY = -1;

        bool check = safeChecks & SAFE_CHECK_ALL_POINTS;

        for(int x = x0; x <= x1; x++)
        {
            if(!axisSwapped) SetPint(imageBuffer, x, y, color, check);
            else SetPint(imageBuffer, y, x, color, check);

            error += deltaErr;
            if(error >= (deltaX + 1)){
                y += dirY;
                error -= (deltaX + 1);
            }
        }
    }

    /**
     * Растеризация окружности в буфере изображения (алгоритм Брезенхэма)
     * @tparam T Тип пикселей в буфере изображения
     * @param imageBuffer Указатель на объект буфера изображения
     * @param x1 Координаты точки центра окружности по X
     * @param y1 Координаты точки центра окружности по Y
     * @param r Радицс
     * @param color Цвет окружности
     * @param safeChecks Осуществлять проверку на выход за пределы
     */
    template<typename T>
    void SetCircle(ImageBuffer<T>* imageBuffer,
                   int x1, int y1, int r,
                   const T& color,
                   std::uint_fast8_t safeChecks = SAFE_CHECK_KEY_POINTS)
    {
        if(safeChecks & SAFE_CHECK_KEY_POINTS){
            if(!imageBuffer->isPointIn(x1+r,y1)) return;
            if(!imageBuffer->isPointIn(x1-r,y1)) return;
            if(!imageBuffer->isPointIn(x1,y1+r)) return;
            if(!imageBuffer->isPointIn(x1,y1-r)) return;
        }

        int x = 0;
        int y = r;
        int delta = 1 - 2 * r;
        int error = 0;

        bool check = safeChecks & SAFE_CHECK_ALL_POINTS;

        while (y >= 0)
        {
            SetPint(imageBuffer, x1 + x, y1 + y, color, check);
            SetPint(imageBuffer, x1 + x, y1 - y, color, check);
            SetPint(imageBuffer, x1 - x, y1 + y, color, check);
            SetPint(imageBuffer, x1 - x, y1 - y, color, check);

            error = 2 * (delta + y) - 1;

            if((delta < 0) && (error <= 0)){
                delta += 2 * ++x + 1;
                continue;
            }

            if ((delta > 0) && (error > 0)){
                delta -= 2 * --y + 1;
                continue;
            }

            delta += 2 * (++x - y--);
        }
    }

    /**
     * Растеризация контуров прямоугольника в буфере изображения
     * @tparam T Тип пикселей в буфере изображения
     * @param imageBuffer Указатель на объект буфера изображения
     * @param x0 Координаты первой точки по X
     * @param y0 Координаты первой точки по Y
     * @param x1 Координаты второй точки конца по X
     * @param y1 Координаты первой точки конца по Y
     * @param color Цвет линий
     * @param safeChecks Осуществлять проверку на выход за пределы
     */
    template<typename T>
    void SetBox(ImageBuffer<T>* imageBuffer,
                int x0, int y0,
                int x1, int y1,
                const T& color,
                std::uint_fast8_t safeChecks = SAFE_CHECK_KEY_POINTS)
    {
        SetLine(imageBuffer,x0,y0,x1,y0,color,safeChecks);
        SetLine(imageBuffer,x1,y0,x1,y1,color,safeChecks);
        SetLine(imageBuffer,x1,y1,x0,y1,color,safeChecks);
        SetLine(imageBuffer,x0,y1,x0,y0,color,safeChecks);
    }

    /**
     * Растеризация контуров прямоугольника в буфере изображения
     * @tparam T Тип пикселей в буфере изображения
     * @param imageBuffer Указатель на объект буфера изображения
     * @param x0 Координаты верхней левой точки по X
     * @param y0 Координаты верхней левой точки по X
     * @param width Ширина
     * @param height Высота
     * @param color Цвет линий
     * @param safeChecks Осуществлять проверку на выход за пределы
     */
    template<typename T>
    void SetRectangle(ImageBuffer<T>* imageBuffer,
                      int x0, int y0,
                      int width, int height,
                      const T& color,
                      std::uint_fast8_t safeChecks = SAFE_CHECK_KEY_POINTS)
    {
        SetBox(imageBuffer,x0,y0,x0+width,y0+height,color,safeChecks);
    }

    /**
     * Заливка фрагмента буфера ограниченного контукром отличным от сцвета фона
     * @tparam T Тип пикселей в буфере изображения
     * @param imageBuffer Указатель на объект буфера изображения
     * @param x0 Точка начала заливки по X
     * @param y0 Точка начала заливки по Y
     * @param backgroundColor Фоновый цвет
     * @param newColor Новый цвет
     * @param isColorEqual Функция обратного вызова для сравнения цветов
     */
    template<typename T>
    void Fill(ImageBuffer<T>* imageBuffer,
              int x0, int y0,
              const T& backgroundColor,
              const T& newColor,
              std::function<bool(const T&, const T&)> isColorEqual)
    {
        if(x0 < 0 || x0 > (imageBuffer->getWidth()-1) || y0 < 0 || y0 > (imageBuffer->getHeight()-1))
            return;

        if(!isColorEqual((*imageBuffer)[y0][x0],backgroundColor))
            return;

        (*imageBuffer)[y0][x0] = newColor;

        Fill(imageBuffer,x0+1,y0,backgroundColor,newColor,isColorEqual);
        Fill(imageBuffer,x0-1,y0,backgroundColor,newColor,isColorEqual);
        Fill(imageBuffer,x0,y0+1,backgroundColor,newColor,isColorEqual);
        Fill(imageBuffer,x0,y0-1,backgroundColor,newColor,isColorEqual);
    }

    /**
     * Нахождение описывающего прямоугольника
     * @tparam T Тип компонентов точек
     * @param points Массив точек
     * @return Описывающий прямоугольник (строкутура из двух точек)
     */
    template <typename T>
    BBox2D<T> FindBoundingBox2D(std::vector<Point2D<T>> points)
    {
        BBox2D<T> result;
        result.min.x = std::min_element(points.begin(), points.end(), [](Point2D<T> elem1, Point2D<T> elem2) {return elem1.x < elem2.x; })->x;
        result.min.y = std::min_element(points.begin(), points.end(), [](Point2D<T> elem1, Point2D<T> elem2) {return elem1.y < elem2.y; })->y;

        result.max.x = std::max_element(points.begin(), points.end(), [](Point2D<T> elem1, Point2D<T> elem2) {return elem1.x < elem2.x; })->x;
        result.max.y = std::max_element(points.begin(), points.end(), [](Point2D<T> elem1, Point2D<T> elem2) {return elem1.y < elem2.y; })->y;
        return result;
    }

    /**
     * Находится ли точка внутри треугольника
     * @tparam T Тип компонентов точки
     * @param p Проверяемая точка
     * @param a Точка треугольника A
     * @param b Точка треугольника B
     * @param c Точка треугольника C
     * @return Да или нет
     */
    template <typename T>
    bool IsPointInTriangle(Point2D<T> p, Point2D<T> a, Point2D<T> b, Point2D<T> c)
    {
        // Используем преобразовнное уравнение прямой проходящих по двум точкам - (y-y1)/(y2-y1) = (x-x1)/(x2-x1)
        // Находим 3 уравнения прямых по двум точкам каждой стороны треугольника, подставляя координаты проверяемой точки
        // Если точка выше - значение будет выше ноля, если ниже - ниже ноля, если равно - на прямой
        // Слудет учитывать что уравнение прямой от точки A к B не совсем то же, что от точкии B к А, это своего рода инверсия,
        // поэтому следует учитывать ориентацию прямых (в каком порядке идут точки), либо делать 2 прверки, для универсальности
        int aSide = (a.y - b.y)*p.x + (b.x - a.x)*p.y + (a.x*b.y - b.x*a.y);
        int bSide = (b.y - c.y)*p.x + (c.x - b.x)*p.y + (b.x*c.y - c.x*b.y);
        int cSide = (c.y - a.y)*p.x + (a.x - c.x)*p.y + (c.x*a.y - a.x*c.y);

        return (aSide >= 0 && bSide >= 0 && cSide >= 0) || (aSide < 0 && bSide < 0 && cSide < 0);
    }

    /**
     * Растеризация треугольника в буфере изображения
     * @tparam T Тип пикселей в буфере изображения
     * @param imageBuffer Буфер изображения
     * @param x0 Координаты первой точки по X
     * @param y0 Координаты первой точки по y
     * @param x1 Координаты второй точки по X
     * @param y1 Координаты второй точки по Y
     * @param x2 Координаты третьей точки по X
     * @param y2 Координаты третьей точки по y
     * @param color Цвет контукров и заливки
     * @param fill Нужно ли закрашивать треугольник
     * @param safeChecks Проверка точек на выход за пределы буфера
     */
    template <typename T>
    void SetTriangle(ImageBuffer<T>* imageBuffer,
            int x0, int y0,
            int x1, int y1,
            int x2, int y2,
            T color,
            bool fill = true,
            std::uint_fast8_t safeChecks = SAFE_CHECK_ALL_POINTS)
    {
        // Не рисовать если не прошло грубую проверку (если она включена)
        if(safeChecks & SAFE_CHECK_KEY_POINTS){
            if(!imageBuffer->isPointIn(x0,y0)) return;
            if(!imageBuffer->isPointIn(x1,y1)) return;
            if(!imageBuffer->isPointIn(x2,y2)) return;
        }

        // Написовать линии
        SetLine(imageBuffer,x0,y0,x1,y1,color,safeChecks);
        SetLine(imageBuffer,x1,y1,x2,y2,color,safeChecks);
        SetLine(imageBuffer,x2,y2,x0,y0,color,safeChecks);

        // Если не надо закрашивать - завершаем
        if(!fill) return;

        // Получить описывающий прямоугольник
        auto bbox = FindBoundingBox2D<int>({{x0,y0},{x1,y1},{x2,y2}});

        // Пройтись по точкам прямугольника, и если точни принадлежат треугольнику - закрасить их
        for(int y = bbox.min.y; y < bbox.max.y; y++)
        {
            for(int x = bbox.min.x; x < bbox.max.x; x++)
            {
                if(IsPointInTriangle<int>({x,y},{x0,y0},{x1,y1},{x2,y2}))
                {
                    SetPint(imageBuffer, x, y, color, safeChecks & SAFE_CHECK_ALL_POINTS);
                }
            }
        }
    }
}