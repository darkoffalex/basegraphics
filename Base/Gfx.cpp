#include "Gfx.h"
#include <iostream>

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
		// Значения начальной и конечной точки
		int x0, y0, x1, y1;
		x0 = pointSrc.x; y0 = pointSrc.y;
		x1 = pointDst.x; y1 = pointDst.y;

		// Была ли произведена смена осей
		bool axisSwapped = false;

		// Если линия имеет наклон более 45 градусов (дельта по иксу меньше дельты по игреку)
		// Нужно поменять оси местами
		if(std::abs(x0 - x1) < std::abs(y0 - y1))
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
			axisSwapped = true;
		}

		// Если начальная точка дальше конечной по X - поменять точки местами
		if(x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		// Дельты для X и Y
		int dx = x1 - x0;
		int dy = y1 - y0;

		// Накапливаемое отклонение (превышение сигнализирует о том что нужно сдвигать Y)
		float accretion = 0;
		// Значение добавляемое к отклонению на каждом шаге
		float deltaAccretion = std::abs(static_cast<float>(dy) / static_cast<float>(dx));
		// Начальный Y
		int y = y0;

		// Пройти по всем X'ам
		for(int x = x0; x <= x1; x++)
		{
			// Была ли смена осей
			if(!axisSwapped)
			{
				if (safePoints)
					SetPointSafe(image, x, y, color);
				else
					SetPoint(image, x, y, color);
			}
			else
			{
				if (safePoints)
					SetPointSafe(image, y, x, color);
				else
					SetPoint(image, y, x, color);
			}

			// Добавить одно отношений дельты к отклонению
			accretion += deltaAccretion;

			// Если отклонение за пройденные шаги начало превосходить 0.5 (считаем что у пикселя есть центр)
			// значит наастало время менять Y (вверх или вниз в зависимости от дельты Y)
			if(accretion > 0.5f)
			{
				// Изменить Y на единицу
				y += (dy > 0 ? 1 : -1);
				// Отнять от приращения единицу, вернув переменную к "состоянию готовности" к следующему шагу
				accretion -= 1.0f;
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
	* \details Данный подход является развитем SetLinePreBresenham. В нужных местах происходит умножение на dx (дельту X),
	* благодаря чему удается избавиться от чисел с плавающей точкой
	*/
	void SetLine(TextureBuffer* image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color, bool safePoints)
	{
		// Значения начальной и конечной точки
		int x0, y0, x1, y1;
		x0 = pointSrc.x; y0 = pointSrc.y;
		x1 = pointDst.x; y1 = pointDst.y;

		// Была ли произведена смена осей
		bool axisSwapped = false;

		// Если линия имеет наклон более 45 градусов (дельта по иксу меньше дельты по игреку)
		// Нужно поменять оси местами
		if (std::abs(x0 - x1) < std::abs(y0 - y1))
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
			axisSwapped = true;
		}

		// Если начальная точка дальше конечной по X - поменять точки местами
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		// Дельты для X и Y
		int dx = x1 - x0;
		int dy = y1 - y0;

		// Накапливаемое отклонение (превышение сигнализирует о том что нужно сдвигать Y)
		float accretion = 0;
		// Значение добавляемое к отклонению на каждом шаге
		float deltaAccretion = std::abs(static_cast<float>(dy));
		// Начальный Y
		int y = y0;

		// Пройти по всем X'ам
		for (int x = x0; x <= x1; x++)
		{
			// Была ли смена осей
			if (!axisSwapped)
			{
				if (safePoints)
					SetPointSafe(image, x, y, color);
				else
					SetPoint(image, x, y, color);
			}
			else
			{
				if (safePoints)
					SetPointSafe(image, y, x, color);
				else
					SetPoint(image, y, x, color);
			}

			// Добавить одно отношение дельты к отклонению
			accretion += deltaAccretion;

			// Если двукратное отклонение за пройденные шаги начало превосходить дельту по X
			// значит настало время менять Y (вверх или вниз в зависимости от дельты Y)
			if (accretion * 2 > dx)
			{
				// Изменить Y на единицу
				y += (dy > 0 ? 1 : -1);
				// Отнять от приращения дельту по X, вернув переменную к "состоянию готовности" к следующему шагу
				accretion -= dx;
			}
		}
	}

	/**
	* \brief Рисование линии (c градиентным цветом)
	* \param image Буфер изображения
	* \param pointSrc Начальная точка
	* \param pointDst Конечная точка
	* \param colorSrc Цвет начальной точки
	* \param colorDst Цвет конечной точки
	* \param safePoints Использовать функцию SetPointSafe для точек
	*/
	void SetLineInterpolated(TextureBuffer* image, Vector2D<int> pointSrc, Vector2D<int> pointDst, Color4f colorSrc, Color4f colorDst, bool safePoints)
	{
		// Значения начальной и конечной точки
		int x0, y0, x1, y1;
		x0 = pointSrc.x; y0 = pointSrc.y;
		x1 = pointDst.x; y1 = pointDst.y;

		// Была ли произведена смена осей
		bool axisSwapped = false;

		// Если линия имеет наклон более 45 градусов (дельта по иксу меньше дельты по игреку)
		// Нужно поменять оси местами
		if (std::abs(x0 - x1) < std::abs(y0 - y1))
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
			axisSwapped = true;
		}

		// Если начальная точка дальше конечной по X - поменять точки местами
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
			std::swap(colorSrc, colorDst);
		}

		// Дельты для X и Y
		int dx = x1 - x0;
		int dy = y1 - y0;

		// Накапливаемое отклонение (превышение сигнализирует о том что нужно сдвигать Y)
		float accretion = 0;
		// Значение добавляемое к отклонению на каждом шаге
		float deltaAccretion = std::abs(static_cast<float>(dy));
		// Цветовое значение на которое будет увеличен (уменьшен) цвет старотовой точки на каждом шаге
		Color4f stepColorValue = (colorDst - colorSrc) / static_cast<float>(std::abs(dx));
		// Начальный Y
		int y = y0;
		// Начальный цвет
		Color4f color = colorSrc;

		// Пройти по всем X'ам
		for (int x = x0; x <= x1; x++)
		{
			// Была ли смена осей
			if (!axisSwapped)
			{
				if (safePoints)
					SetPointSafe(image, x, y, color.GetBgr());
				else
					SetPoint(image, x, y, color.GetBgr());
			}
			else
			{
				if (safePoints)
					SetPointSafe(image, y, x, color.GetBgr());
				else
					SetPoint(image, y, x, color.GetBgr());
			}

			// Добавить одно отношение дельты к отклонению
			accretion += deltaAccretion;

			// Если двукратное отклонение за пройденные шаги начало превосходить дельту по X
			// значит настало время менять Y (вверх или вниз в зависимости от дельты Y)
			if (accretion * 2 > dx)
			{
				// Изменить Y на единицу
				y += (dy > 0 ? 1 : -1);
				// Отнять от приращения дельту по X, вернув переменную к "состоянию готовности" к следующему шагу
				accretion -= dx;
			}

			// Изменить цвет
			color += stepColorValue;
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
	* \brief Рисование линии (с проверкой выхода начальной и конечной точки за пределы)
	* \param image Буфер изображения
	* \param pointSrc Начальная точка
	* \param pointDst Конечная точка
	* \param colorSrc Цвет начальной точки
	* \param colorDst Цвет конечной точки
	*/
	void SetLineSafeInterpolated(TextureBuffer* image, Vector2D<int> pointSrc, Vector2D<int> pointDst, Color4f colorSrc, Color4f colorDst)
	{
		if (image->IsPointInBounds(pointSrc.x, pointSrc.y) && image->IsPointInBounds(pointDst.x, pointDst.y))
		{
			SetLineInterpolated(image, pointSrc, pointDst, colorSrc, colorDst);
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
		// Если все точки находятся в допустимых пределах (не выходят за границы текстурного буфера)
		if(image->IsPointInBounds(p0.x,p0.y) && image->IsPointInBounds(p1.x,p1.y) && image->IsPointInBounds(p2.x,p2.y))
		{
			// Нарисовать линии
			SetLine(image, p0, p1, color);
			SetLine(image, p1, p2, color);
			SetLine(image, p2, p0, color);

			// Найти описывающий прямоугольник
			Box2D<int> bbox = FindTriangleBoundingBox2D(p0, p1, p2);

			// Пройтись по точкам прямугольника, и если точни принадлежат треугольнику - закрасить их
			for (int y = bbox.topLeft.y; y <= bbox.bottomRight.y; y++)
			{
				for (int x = bbox.topLeft.x; x <= bbox.bottomRight.x; x++)
				{
					if(IsInTriangleVector({x,y},p0,p1,p2)){
						SetPoint(image, x, y, color);
					}
				}
			}
		}
	}

	/**
	* \brief Рисование треугольника (полигон)
	* \param image Буфер изображения
	* \param p0 Точка 1
	* \param p1 Точка 2
	* \param p2 Точка 3
	* \param col0 Цвет точки 1
	* \param col1 Цвет точки 2
	* \param col2 Цвет точки 3
	*/
	void SetPolygonInterpolated(TextureBuffer * image, Vector2D<int> p0, Vector2D<int> p1, Vector2D<int> p2, Color4f col0, Color4f col1, Color4f col2)
	{
		// Исключить из обработки кривые треугольники
		if (p0.y == p1.y && p0.y == p2.y) return;

		// Если все точки находятся в допустимых пределах (не выходят за границы текстурного буфера)
		if (image->IsPointInBounds(p0.x, p0.y) && image->IsPointInBounds(p1.x, p1.y) && image->IsPointInBounds(p2.x, p2.y))
		{
			// Нарисовать линии
			SetLineInterpolated(image, p0, p1, col0, col1);
			SetLineInterpolated(image, p1, p2, col1, col2);
			SetLineInterpolated(image, p2, p0, col2, col0);
			
			
			// Храним значение первой точки (на вектор которой сдвигаем треугольник)
			Vector2D<int> oldP0 = p0;

			// Сдвигаем треугольник в начало координат
			p1 = p1 - p0;
			p2 = p2 - p0;
			p0 = { 0,0 };

			// Если y 3-ей точки оказался нулем - меняем точку 3 и 2 местами
			if (p2.y == 0) {
				std::swap(p2, p1);
			}

			// Получить цветовую "дельту" для двух сторон треугольника
			Color4f deltaCol0 = col1 - col0;
			Color4f deltaCol1 = col2 - col0;

			// Найти описывающий прямоугольник
			Box2D<int> bbox = FindTriangleBoundingBox2D(p0, p1, p2);

			// Пройтись по точкам прямугольника, и если точни принадлежат треугольнику - закрасить их
			for (int y = bbox.topLeft.y; y <= bbox.bottomRight.y; y++)
			{
				for (int x = bbox.topLeft.x; x <= bbox.bottomRight.x; x++)
				{
					// От начальной точки треугольника в сторону двух других идут 2 вектора, и еще один вектор
					// в сторону проверяемой точкр (вектор P). Вектор P можно представить в виде суммы векторов, котороые
					// являются результатом произведения 2-ух основных векторов треугольника (сторон) на некие коэфициенты 

					// Коэфициент-множитель для одного вектора (стороны) треугольника.
					float w1 = static_cast<float>(y * p2.x - x * p2.y) / static_cast<float>(p1.y*p2.x - p1.x*p2.y);

					// Если коэфициент в пределах единицы - точка все еще может находится в пределах треугольника
					if(w1 >= 0 && w1 <= 1)
					{
						// Коэфициент для второй стороны
						float w2 = static_cast<float>(y - w1*p1.y) / static_cast<float>(p2.y);

						// Если сумма коэфициентов в пределах единицы - точка в треугольнике, можно рисовать
						if(w2 >= 0 && (w1 + w2) <= 1)
						{
							// Вычислить цвет точки сложив "цветовые вектора"
							Color4f resultColor = col0 + (deltaCol0 * w2) + (deltaCol1 * w1);

							// Установить точку
							SetPoint(image, x + oldP0.x, y + oldP0.y, resultColor.GetBgr());
						}
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
	* \brief Находится ли точка внутри треугольника (векторный метод)
	* \param p Проверяемая точка
	* \param a Точка треугольника A
	* \param b Точка треугольника B
	* \param c Точка треугольника C
	* \return Да или нет
	*/
	bool IsInTriangleVector(Vector2D<int> p, Vector2D<int> a, Vector2D<int> b, Vector2D<int> c)
	{
		// Сдвигаем треугольник в начало координат
		b = b - a;
		c = c - a;
		p = p - a;
		a = { 0,0 };

		// Если c.y оказался нулем - меняем точку "с" и "b" местами
		if(c.y == 0){
			std::swap(c, b);
		}

		// Получаем коэфициенты-множетели двух векторов, сумма которых равна вектору к проверяемой точке
		float w1 = static_cast<float>(p.y * c.x - p.x * c.y) / static_cast<float>(b.y*c.x - b.x*c.y);
		float w2 = static_cast<float>(p.y - w1*b.y) / static_cast<float>(c.y);

		// Проверяем выходят ли коэфициенты за пределы
		return w1 >= 0 && w2 >= 0 && (w1 + w2) <= 1;
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
