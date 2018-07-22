#include <iostream>
#include <Windows.h>
#include <chrono>

#include "BitmapBuffer.h"
#include "VertexBuffer.h"

/**
* \brief Оконная процедура (объявление)
* \param hWnd Хендл окна
* \param message Идентификатор сообщения
* \param wParam Параметр сообщения
* \param lParam Парамтер сообщения
* \return Код выполнения
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/**
* \brief Установка пикселя (безопасный вариант, с проверкой границ)
* \param buffer Буфер кадра (указатель объект)
* \param x Положение по оси X
* \param y Положение по ост Y
* \param color Цвет
*/
void SetPointSafe(BitmapBuffer * buffer, int x, int y, BitmapRGB color = { 0,0,0 });

/**
 * \brief Установка пикселя
 * \param buffer 
 * \param x Положение по оси X
 * \param y Положение по ост Y
 * \param color Цвет
 */
void SetPoint(BitmapBuffer * buffer, int x, int y, BitmapRGB color = { 0,0,0 });

/**
* \brief Рисование линии (быстрый вариант, алгоритм Брэзенхема)
* \param buffer Буфер кадра (указатель на объект)
* \param x0 Начальная точка (компонента X)
* \param y0 Начальная точка (компонента Y)
* \param x1 Конечная точка (компонента X)
* \param y1 Конечная точка (компонента Y)
* \param color Цвет
*/
void SetLine(BitmapBuffer * buffer, int x0, int y0, int x1, int y1, BitmapRGB color = { 0,0,0 });

/**
 * \brief Рисование сеточной модели
 * \param bBuffer Буфер кадра (указатель на объект)
 * \param vBuffer Буфер геометрии (указатель на объект)
 */
void DrawWireModel(BitmapBuffer * bBuffer, VertexBuffer * vBuffer);

/**
* \brief Отрисовка кадра
* \param width Ширина
* \param height Высота
* \param pixels Массив пикселов
* \param hWnd Хендл окна, device context которого будет использован
*/
void PresentFrame(uint32_t width, uint32_t height, void* pixels, HWND hWnd);

// Буфер кадра
BitmapBuffer frameBuffer;

// Буфер вершин
VertexBuffer vertexBuffer;

/**
* \brief Точка входа
* \param argc Кол-во аргументов на входе
* \param argv Агрументы (массив C-строк)
* \return Код выполнения
*/
int main(int argc, char* argv[])
{
	try
	{
		// Получение хендла исполняемого модуля
		HINSTANCE hInstance = GetModuleHandle(nullptr);

		// Регистрация класса окна
		WNDCLASSEX classInfo;
		classInfo.cbSize = sizeof(WNDCLASSEX);
		classInfo.style = CS_HREDRAW | CS_VREDRAW;
		classInfo.cbClsExtra = 0;
		classInfo.cbWndExtra = 0;
		classInfo.hInstance = hInstance;
		classInfo.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		classInfo.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
		classInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
		classInfo.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
		classInfo.lpszMenuName = nullptr;
		classInfo.lpszClassName = L"WireRendererWindow";
		classInfo.lpfnWndProc = WndProc;

		// Если не удалось зарегистрировать класс
		if (!RegisterClassEx(&classInfo)) {
			throw std::runtime_error("ERROR: Can't register window class.");
		}

		// Создание окна
		HWND mainWindow = CreateWindow(
			classInfo.lpszClassName,
			L"Wire Renderer",
			WS_OVERLAPPEDWINDOW,
			0, 0,
			800, 600,
			NULL,
			NULL,
			hInstance,
			NULL);

		// Если не удалось создать окно
		if (!mainWindow) {
			throw std::runtime_error("ERROR: Can't create main application window.");
		}

		// Показать окно
		ShowWindow(mainWindow, SW_SHOWNORMAL);

		// Получить размеры клиентской области окна
		RECT clientRect;
		GetClientRect(mainWindow, &clientRect);
		std::cout << "INFO: Client area sizes : " << clientRect.right << "x" << clientRect.bottom << std::endl;

		// Создать буфер кадра по размерам клиенсткой области
		frameBuffer = BitmapBuffer(clientRect.right, clientRect.bottom);
		std::cout << "INFO: Frame-buffer initialized  (size : " << frameBuffer.GetSize() << " bytes)" << std::endl;

		// Загрузить вершины в буфер вершин из файла
		vertexBuffer.LoadFromFile("models/african_head.obj");
		std::cout << "INFO: Vertex-buffer initialized (size: " << vertexBuffer.GetSize() << " bytes, " << vertexBuffer.GetVertices().size() << " vertices, " << vertexBuffer.GetFaces().size() << " faces)" << std::endl;

		// Оконное сообщение (пустая структура)
		MSG msg = {};

		// Вечный цикл (работает пока не пришло сообщение WM_QUIT)
		while (true)
		{
			// Если получено сообщение
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT) {
					break;
				}
			}

			// Если хендл окна не пуст
			if (mainWindow)
			{
				// Очистить кадр 
				frameBuffer.Clear({ 0,0,0 });

				DrawWireModel(&frameBuffer, &vertexBuffer);

				// Сообщение "перерисовать", чтобы показать обновленный кадр
				SendMessage(mainWindow, WM_PAINT, NULL, NULL);
			}

		}
	}
	catch (std::exception const &ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

/**
* \brief Оконная процедура (реализация)
* \param hWnd Хендл окна
* \param message Идентификатор сообщения
* \param wParam Параметр сообщения
* \param lParam Парамтер сообщения
* \return Код выполнения
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		PresentFrame(frameBuffer.GetWidth(), frameBuffer.GetHeight(), frameBuffer.GetData(), hWnd);
		return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/**
* \brief Установка пикселя (безопасный вриант)
* \param buffer Буфер кадра (указатель на объект)
* \param x Положение по оси X
* \param y Положение по ост Y
* \param color Цвет
*/
void SetPointSafe(BitmapBuffer * buffer, int x, int y, BitmapRGB color)
{
	if (y <= static_cast<int>(buffer->GetHeight() - 1) &&
		y >= 0 && x <= static_cast<int>(buffer->GetWidth() - 1) && x >= 0)
	{
		(*buffer)[y][x].red = color.red;
		(*buffer)[y][x].green = color.green;
		(*buffer)[y][x].blue = color.blue;
	}
}

/**
 * \brief Установка пикселя
 * \param buffer Буфер кадра (указатель на объект)
 * \param x Положение по оси X
 * \param y Положение по ост Y
 * \param color Цвет
 */
void SetPoint(BitmapBuffer * buffer, int x, int y, BitmapRGB color)
{
	(*buffer)[y][x].red = color.red;
	(*buffer)[y][x].green = color.green;
	(*buffer)[y][x].blue = color.blue;
}

/**
* \brief Рисование линии (быстрый вариант, алгоритм Брэзенхема)
* \param buffer Буфер кадра (указатель объект)
* \param x0 Начальная точка (компонента X)
* \param y0 Начальная точка (компонента Y)
* \param x1 Конечная точка (компонента X)
* \param y1 Конечная точка (компонента Y)
* \param color Цвет
*/
void SetLine(BitmapBuffer * buffer, int x0, int y0, int x1, int y1, BitmapRGB color)
{
	if(x0 > static_cast<int>(buffer->GetWidth()-1) ||
		x1 > static_cast<int>(buffer->GetWidth()-1) ||
		y0 > static_cast<int>(buffer->GetHeight()-1) ||
		y1 > static_cast<int>(buffer->GetHeight()-1))
	{
		return;
	}

	int const deltaX = x1 - x0;
	int const deltaY = y1 - y0;
	int const absDeltaX = std::abs(deltaX);
	int const absDeltaY = std::abs(deltaY);

	int accretion = 0;

	if (absDeltaX >= absDeltaY)
	{
		int y = y0;
		const int direction = deltaY != 0 ? (deltaY > 0 ? 1 : -1) : 0;
		for (int x = x0; deltaX > 0 ? x <= x1 : x >= x1; deltaX > 0 ? x++ : x--)
		{
			SetPoint(buffer, x, y, color);
			accretion += absDeltaY;

			if (accretion * 2 >= absDeltaX)
			{
				accretion -= absDeltaX;
				y += direction;
			}
		}
	}
	else
	{
		int x = x0;
		const int direction = deltaX != 0 ? (deltaX > 0 ? 1 : -1) : 0;
		for (int y = y0; deltaY > 0 ? y <= y1 : y >= y1; deltaY > 0 ? y++ : y--)
		{
			SetPoint(buffer, x, y, color);
			accretion += absDeltaX;

			if (accretion * 2 >= absDeltaY)
			{
				accretion -= absDeltaY;
				x += direction;
			}
		}
	}
}

/**
* \brief Рисование сеточной модели
* \param bBuffer Буфер кадра (указатель на объект)
* \param vBuffer Буфер геометрии (указатель на объект)
*/
void DrawWireModel(BitmapBuffer * bBuffer, VertexBuffer * vBuffer)
{
	// Проход по всем полигонам модели (полигон - массив индексов вершин, обычно 3 индекса)
	for(unsigned int i = 0; i < vBuffer->GetFaces().size(); i++)
	{
		// Получение индексов текущего полигона
		std::vector<int> faceIndices = vBuffer->GetFaces()[i];

		// Проход по всем точкам полигона (если считать что их 3)
		for(unsigned int j = 0; j < 3; j++)
		{
			// Получаем текущую точку и следующую (следующая может оказаться нулевой, завершающей)
			Vector3D v0 = vBuffer->GetVertices()[faceIndices[j]];
			Vector3D v1 = vBuffer->GetVertices()[faceIndices[(j + 1) % 3]];

			// Преобразование однородных координат [-1,1] вершин в оконные координаты [0,ширина]
			// Используем только 2 ости (X и Y). Ось Z (глубина) не задействуется, в рузультате
			// получаем что-то вроде изометрической проекции (без перспективного искажения).
			// Если заменить оси X и Z - получим вид модели с иной стороны
			int x0 = static_cast<int>((v0.X + 1.0f)*(bBuffer->GetWidth() / 2.0f));
			int y0 = static_cast<int>((v0.Y*-1 + 1.0f)*(bBuffer->GetHeight() /2.0f)); //Учитываем инверсию оси Y окна, умнажаем на -1
			int x1 = static_cast<int>((v1.X + 1.0f)*(bBuffer->GetWidth() / 2.0f));
			int y1 = static_cast<int>((v1.Y*-1 + 1.0f)*(bBuffer->GetHeight() /2.0f)); //Учитываем инверсию оси Y окна, умнажаем на -1

			SetLine(bBuffer, x0, y0, x1, y1, {0,255,0});
		}
	}
}

/**
* \brief Отрисовка кадра
* \param width Ширина
* \param height Высота
* \param pixels Массив пикселов
* \param hWnd Хендл окна, device context которого будет использован
*/
void PresentFrame(uint32_t width, uint32_t height, void* pixels, HWND hWnd)
{
	// Получить хендл на временный bit-map (4 байта на пиксель)
	HBITMAP hBitMap = CreateBitmap(width, height, 1, 8 * 4, pixels);

	// Получить device context окна
	HDC hdc = GetDC(hWnd);

	// Временный DC для переноса bit-map'а
	HDC srcHdc = CreateCompatibleDC(hdc);

	// Связать bit-map с временным DC
	SelectObject(srcHdc, hBitMap);

	// Копировать содержимое временного DC в DC окна
	BitBlt(
		hdc,    // HDC назначения
		0,      // Начало вставки по оси X
		0,      // Начало вставки по оси Y
		width,  // Ширина
		height, // Высота
		srcHdc, // Исходный HDC (из которого будут копироваться данные)
		0,      // Начало считывания по оси X
		0,      // Начало считывания по оси Y
		SRCCOPY // Копировать
	);

	// Уничтожить bit-map
	DeleteObject(hBitMap);
	// Уничтодить временный DC
	DeleteDC(srcHdc);
	// Уничтодить DC
	DeleteDC(hdc);
}