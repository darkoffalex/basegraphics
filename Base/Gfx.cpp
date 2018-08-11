#include "Gfx.h"

namespace gfx
{
	/**
	* \brief Установка цвета заданного пикселя
	* \param image Буфер изображения
	* \param x Номер пикселя в ряду
	* \param y Номер ряда пикслей в изображении
	* \param color
	*/
	void SetPoint(TextureBuffer* image, int x, int y, ColorBGR color)
	{
		(*image)[y][x] = color;
	}

	/**
	* \brief Установка цвета заданного пикселя (с проверкой выхода за пределы)
	* \param image Буфер изображения
	* \param x Номер пикселя в ряду
	* \param y Номер ряда пикслей в изображении
	* \param color
	*/
	void SetPointSafe(TextureBuffer* image, int x, int y, ColorBGR color)
	{
		if(image->IsPointInBounds(x,y)){
			SetPoint(image, x, y, color);
		}
	}

	/**
	* \brief Рисование линии (алгоритм предшествующий алгоритму Брезенхэма)
	* \param image Буфер изображения
	* \param pointSrc Начальная точка
	* \param pointDst Конечная точка
	* \param color Цвет
	* \param safePoints Использовать функцию SetPointSafe для точек
	*/
	void SetLinePreBresenham(TextureBuffer* image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color, bool safePoints)
	{
		// Получить разницу между координатами точек (дельта)
		Vector2D<int> delta = pointDst - pointSrc;
		// Абсолютное значение дельты
		Vector2D<int> deltaAbs = delta.GetAbsolute();
		// Отклонение
		float accretion = 0;

		// Для случаев, когда delta по X больше или равно delta по Y (угол наклона прямой в обе стороны не больше 45 градусов)
		if (deltaAbs.x >= deltaAbs.y)
		{
			// Начальная точка по Y
			int y = pointSrc.y;
			// Направление (следующие точки будут выше, ниже, либо на уровне предыдущей)
			int direction = delta.y != 0 ? (delta.y > 0 ? 1 : -1) : 0;

			// Пройтись от начальной точки по X до конецчной
			for (int x = pointSrc.x; delta.x > 0 ? x <= pointDst.x : x >= pointDst.x; delta.x > 0 ? x++ : x--)
			{
				// Установить точку
				if (safePoints) {
					SetPointSafe(image, x, y, color);
				}
				else {
					SetPoint(image, x, y, color);
				}

				// К отклонению добавить одно отношение дельты Y к дельте X
				accretion += static_cast<float>(deltaAbs.y) / static_cast<float>(deltaAbs.x);

				// Если отклонение помноженное за все пройденные шаги начало первышть половинку пикселя (считаем половину - центром пикселя) 
				// следует сместить положение следующей точки (вниз или вверх) на единицу
				if (accretion >= 0.5f)
				{
					accretion -= 1.0f;
					y += direction;
				}
			}
		}
		// Для случаев, когда delta по X меньше delta по Y (угол наклона прямой в обе стороны больше 45 градусов)
		// Меняем оси местами (рисуем не игреки для иксов, а иксы для игреков)
		else
		{
			// Начальная точка по X
			int x = pointSrc.x;
			// Направление (следующие точки будут правее, левее, либо на уровне предыдущей)
			int direction = delta.x != 0 ? (delta.x > 0 ? 1 : -1) : 0;

			// Пройтись от начальной точки по Y до конецчной
			for (int y = pointSrc.y; delta.y > 0 ? y <= pointDst.y : y >= pointDst.y; delta.y > 0 ? y++ : y--)
			{
				// Установить точку
				if (safePoints) {
					SetPointSafe(image, x, y, color);
				}
				else {
					SetPoint(image, x, y, color);
				}

				// К отклонению добавить одно отношение дельты X к дельте Y
				accretion += static_cast<float>(deltaAbs.x) / static_cast<float>(deltaAbs.y);

				// Если отклонение помноженное за все пройденные шаги начало первышть половинку пикселя (считаем половину - центром пикселя) 
				// следует сместить положение следующей точки (вправо или влево) на единицу
				if (accretion >= 0.5f)
				{
					accretion -= 1.0f;
					x += direction;
				}
			}
		}
	}

	/**
	* \brief Рисование линии (алгоритм Брезенхэма)
	* \param image Буфер изображения
	* \param pointSrc Начальная точка
	* \param pointDst Конечная точка
	* \param color Цвет
	* \param safePoints Использовать функцию SetPointSafe для точек
	* 
	* \details Данный подход является развитем SetLinePreBresenham. В нужных местах происходит умножение на deltaAbs.x либо deltaAbs.y,
	* благодаря чему удается избавиться от чисел с плавающей точкой
	*/
	void SetLine(TextureBuffer* image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color, bool safePoints)
	{
		// Получить разницу между координатами точек (дельта)
		Vector2D<int> delta = pointDst - pointSrc;
		// Абсолютное значение дельты
		Vector2D<int> deltaAbs = delta.GetAbsolute();

		// Отклонение
		int accretion = 0;

		// Для случаев, когда delta по X больше или равно delta по Y (угол наклона прямой в обе стороны не больше 45 градусов)
		if(deltaAbs.x >= deltaAbs.y)
		{
			// Начальная точка по Y
			int y = pointSrc.y;
			// Направление (следующие точки будут выше, ниже, либо на уровне предыдущей)
			int direction = delta.y != 0 ? (delta.y > 0 ? 1 : -1) : 0;

			// Пройтись от начальной точки по X до конецчной
			for (int x = pointSrc.x; delta.x > 0 ? x <= pointDst.x : x >= pointDst.x; delta.x > 0 ? x++ : x--)
			{
				// Установить точку
				if(safePoints){
					SetPointSafe(image, x, y, color);
				}else{
					SetPoint(image, x, y, color);
				}
				
				// К отклонению добавить одно абсолютное значение delta по Y
				accretion += deltaAbs.y;

				// Если отклонение помноженное на 2 за все пройденные шаги начало превосходить
				// delta по X - следует сместить положение следующей точки (вниз или вверх) на единицу
				// ВАЖНО: Уменожение на 2 происходит для большей точности (считаем от центра пикселя)
				// Для большей ясности ознакомьтесь с алгоритмом предшествующим алгоритму Брезенхэма
				if (accretion * 2 >= deltaAbs.x)
				{
					y += direction;
					accretion -= deltaAbs.x;
				}
			}
		}
		// Для случаев, когда delta по X меньше delta по Y (угол наклона прямой в обе стороны больше 45 градусов)
		// Меняем оси местами (рисуем не игреки для иксов, а иксы для игреков)
		else
		{
			// Начальная точка по X
			int x = pointSrc.x;
			// Направление (следующие точки будут правее, левее, либо на уровне предыдущей)
			int direction = delta.x != 0 ? (delta.x > 0 ? 1 : -1) : 0;

			// Пройтись от начальной точки по Y до конецчной
			for (int y = pointSrc.y; delta.y > 0 ? y <= pointDst.y : y >= pointDst.y; delta.y > 0 ? y++ : y--)
			{
				// Установить точку
				if (safePoints) {
					SetPointSafe(image, x, y, color);
				}
				else {
					SetPoint(image, x, y, color);
				}

				// К отклонению добавить одно абсолютное значение delta по X
				accretion += deltaAbs.x;

				// Если отклонение помноженное на 2 за все пройденные шаги начало превосходить
				// delta по Y - следует сместить положение следующей точки (вправо или влево) на единицу
				if (accretion * 2 >= deltaAbs.y)
				{
					x += direction;
					accretion -= deltaAbs.y;
				}
			}
		}
	}

	/**
	* \brief Рисование линии (с проверкой выхода начальной и конечной точки за пределы)
	* \param image Буфер изображения
	* \param pointSrc Начальная точка
	* \param pointDst Конечная точка
	* \param color Цвет
	*/
	void SetLineSafe(TextureBuffer* image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color)
	{
		if(image->IsPointInBounds(pointSrc.x,pointSrc.y) && image->IsPointInBounds(pointDst.x,pointDst.y))
		{
			SetLine(image, pointSrc, pointDst, color);
		}
	}

	/**
	* \brief Рисование прямоугольника
	* \param image Буфер изображения
	* \param box Параметры прямоугольника (2 точки)
	* \param color Цвет
	*/
	void SetRectangle(TextureBuffer* image, Box2D<int> box, ColorBGR color)
	{
		SetLine(image, box.topLeft, { box.bottomRight.x, box.topLeft.y }, color, true);
		SetLine(image, { box.bottomRight.x, box.topLeft.y }, box.bottomRight, color, true);
		SetLine(image, box.bottomRight, {box.topLeft.x,box.bottomRight.y}, color, true);
		SetLine(image, { box.topLeft.x,box.bottomRight.y }, box.topLeft, color, true);
	}

	/**
	* \brief Рисование треугольника (полигон)
	* \param image Буфер изображения
	* \param p0 Точка 1
	* \param p1 Точка 2
	* \param p2 Точка 3
	* \param color Цвет
	*/
	void SetPolygon(TextureBuffer * image, Vector2D<int> p0, Vector2D<int> p1, Vector2D<int> p2, ColorBGR color)
	{
		if(image->IsPointInBounds(p0.x,p0.y) && image->IsPointInBounds(p1.x,p1.y) && image->IsPointInBounds(p2.x,p2.y))
		{
			SetLine(image, p0, p1, color);
			SetLine(image, p1, p2, color);
			SetLine(image, p2, p0, color);

			Box2D<int> bbox = FindTriangleBoundingBox2D(p0, p1, p2);
			for (int y = bbox.topLeft.y; y <= bbox.bottomRight.y; y++)
			{
				for (int x = bbox.topLeft.x; x <= bbox.bottomRight.x; x++)
				{
					if(IsInTriangle({x,y},p0,p1,p2)){
						SetPoint(image, x, y, color);
					}
				}
			}
		}
	}

	/**
	* \brief Находится ли точка внутри треугольника
	* \param p Проверяемая точка
	* \param a Точка треугольника A
	* \param b Точка треугольника B
	* \param c Точка треугольника C
	* \return Да или нет
	*/
	bool IsInTriangle(Vector2D<int> p, Vector2D<int> a, Vector2D<int> b, Vector2D<int> c)
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
	* \brief Представление (показ) изображения на поверхности WinAPI окна
	* \param image Текстурный буфер
	* \param hWnd Хендл окна
	*/
	void PresentFrame(TextureBuffer* image, HWND hWnd)
	{
		if(image->GetSize() > 0)
		{
			// Получить хендл на временный bit-map (4 байта на пиксель)
			HBITMAP hBitMap = CreateBitmap(image->GetWidth(), image->GetHeight(), 1, 8 * 4, image->GetData());

			// Получить device context окна
			HDC hdc = GetDC(hWnd);

			// Временный DC для переноса bit-map'а
			HDC srcHdc = CreateCompatibleDC(hdc);

			// Связать bit-map с временным DC
			SelectObject(srcHdc, hBitMap);

			// Копировать содержимое временного DC в DC окна
			BitBlt(
				hdc,                // HDC назначения
				0,                  // Начало вставки по оси X
				0,                  // Начало вставки по оси Y
				image->GetWidth(),  // Ширина
				image->GetHeight(), // Высота
				srcHdc,             // Исходный HDC (из которого будут копироваться данные)
				0,                  // Начало считывания по оси X
				0,                  // Начало считывания по оси Y
				SRCCOPY             // Копировать
			);

			// Уничтожить bit-map
			DeleteObject(hBitMap);
			// Уничтодить временный DC
			DeleteDC(srcHdc);
			// Уничтодить DC
			DeleteDC(hdc);
		}
	}
}
