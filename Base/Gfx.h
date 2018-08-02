#pragma once
#include <Windows.h>

#include "TextureBuffer.h"
#include "GfxTypes.h"

namespace gfx
{
	/**
	 * \brief Установка цвета заданного пикселя
	 * \param image Буфер изображения
	 * \param x Номер пикселя в ряду
	 * \param y Номер ряда пикслей в изображении
	 * \param color 
	 */
	void SetPoint(TextureBuffer * image, int x, int y, ColorBGR color);

	/**
	 * \brief Установка цвета заданного пикселя (с проверкой выхода за пределы)
	 * \param image Буфер изображения
	 * \param x Номер пикселя в ряду
	 * \param y Номер ряда пикслей в изображении
	 * \param color 
	 */
	void SetPointSafe(TextureBuffer * image, int x, int y, ColorBGR color);

	/**
	 * \brief Рисование линии (алгоритм предшествующий алгоритму Брезенхэма)
	 * \param image Буфер изображения
	 * \param pointSrc Начальная точка
	 * \param pointDst Конечная точка
	 * \param color Цвет
	 * \param safePoints Использовать функцию SetPointSafe для точек
	 */
	void SetLinePreBresenham(TextureBuffer * image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color, bool safePoints = false);

	/**
	 * \brief Рисование линии
	 * \param image Буфер изображения
	 * \param pointSrc Начальная точка
	 * \param pointDst Конечная точка
	 * \param color Цвет
	 * \param safePoints Использовать функцию SetPointSafe для точек
	 */
	void SetLine(TextureBuffer * image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color, bool safePoints = false);

	/**
	 * \brief Рисование линии (с проверкой выхода начальной и конечной точки за пределы)
	 * \param image Буфер изображения
	 * \param pointSrc Начальная точка
	 * \param pointDst Конечная точка
	 * \param color Цвет
	 */
	void SetLineSafe(TextureBuffer * image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color);

	/**
	 * \brief Представление (показ) изображения на поверхности WinAPI окна
	 * \param image Текстурный буфер
	 * \param hWnd Хендл окна
	 */
	void PresentFrame(TextureBuffer * image, HWND hWnd);
}
