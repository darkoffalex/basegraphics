#pragma once

#include "ImageBuffer.hpp"

#include <cmath>
#include <functional>

namespace gfx
{
    // Уровни проверки на выход за пределы буфера
    constexpr std::uint_fast8_t SAFE_CHECK_DISABLE     {0};
    constexpr std::uint_fast8_t SAFE_CHECK_KEY_POINTS  {1 << 0};
    constexpr std::uint_fast8_t SAFE_CHECK_ALL_POINTS  {1 << 1};

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
}