#include <iostream>
#include <Windows.h>
#include <chrono>

#include "BitmapBuffer.h"

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
* \brief Установка пикселя
* \param buffer Буфер кадра (указатель объект)
* \param x Положение по оси X
* \param y Положение по ост Y
* \param color Цвет очистки
*/
void SetPoint(BitmapBuffer * buffer, int x, int y, BitmapRGB color = { 0,0,0 });

/**
* \brief Рисование линии (медленный вариант, с использованием чисел с плавающей точкой)
* \param buffer Буфер кадра (указатель объект)
* \param x0 Начальная точка (компонента X)
* \param y0 Начальная точка (компонента Y)
* \param x1 Конечная точка (компонента X)
* \param y1 Конечная точка (компонента Y)
* \param color Цвет очистки
*/
void SetLine(BitmapBuffer * buffer, int x0, int y0, int x1, int y1, BitmapRGB color = { 0,0,0 });

/**
* \brief Рисование линии (алгоритм предшествующий классическому алгоритму Брэзенхема)
* \param buffer Буфер кадра (указатель объект)
* \param x0 Начальная точка (компонента X)
* \param y0 Начальная точка (компонента Y)
* \param x1 Конечная точка (компонента X)
* \param y1 Конечная точка (компонента Y)
* \param color Цвет очистки
*/
void SetLinePreBresenham(BitmapBuffer * buffer, int x0, int y0, int x1, int y1, BitmapRGB color = { 0,0,0 });

/**
* \brief Рисование линии (быстрый вариант, алгоритм Брэзенхема)
* \param buffer Буфер кадра (указатель объект)
* \param x0 Начальная точка (компонента X)
* \param y0 Начальная точка (компонента Y)
* \param x1 Конечная точка (компонента X)
* \param y1 Конечная точка (компонента Y)
* \param color Цвет очистки
*/
void SetLineBresenham(BitmapBuffer * buffer, int x0, int y0, int x1, int y1, BitmapRGB color = { 0,0,0 });

/**
 * \brief Рисование квадрата
 * \param buffer Буфер кадра (указатель объект)
 * \param x Положение левого верхнего угла по оси X
 * \param y Положение левого верхнего угла по оси Y
 * \param size Размер стороны
 * \param color Цвет
 */
void SetSquare(BitmapBuffer * buffer, int x, int y, int size, BitmapRGB color = { 0,0,0 });

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

// Время последнего кадра
std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;

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
		classInfo.lpszClassName = L"BrasenhamLinesWindow";
		classInfo.lpfnWndProc = WndProc;

		// Если не удалось зарегистрировать класс
		if (!RegisterClassEx(&classInfo)) {
			throw std::runtime_error("ERROR: Can't register window class.");
		}

		// Создание окна
		HWND mainWindow = CreateWindow(
			classInfo.lpszClassName,
			L"Brasenham Lines",
			WS_OVERLAPPEDWINDOW,
			0, 0,
			640, 480,
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

		// Параметры положения/скорости квадрата
		struct
		{
			//Положение квадрата
			float posX = 10.0f;
			float posY = 10.0f;

			//Множитель скорости
			const float speedMul = 0.3f;

			//Вектор скорости (направление)
			float speedX = 1;
			float speedY = 1;

			//Размер стороны квадрата
			float size = 100.0f;
		} squareSettings;


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
				// Время текущего кадра (текущей итерации)
				const std::chrono::time_point<std::chrono::high_resolution_clock> currentFrameTime = std::chrono::high_resolution_clock::now();

				// Сколько микросекунд прошло с последней итерации
				// 1 миллисекунда = 1000 микросекунд = 1000000 наносекунд
				const int64_t delta = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - lastFrameTime).count();

				// Перевести в миллисекунды
				float deltaMs = static_cast<float>(delta) / 1000.0f;

				// Приращение к положению (с учетом скорости и времени прошедшего с прошлого кадра)
				// Во избежании резкого рывка при старте дельта времени ограничивается максимальным значением 5 миллисекунд
				if(deltaMs <= 5.0f)
				{
					squareSettings.posX += deltaMs * squareSettings.speedX * squareSettings.speedMul;
					squareSettings.posY += deltaMs * squareSettings.speedY * squareSettings.speedMul;
				}


				// Если столкнулось с левой стеной - развернуть
				if(squareSettings.posX <= 0){
					squareSettings.posX = 0;
					squareSettings.speedX *= -1;
				}

				// Если столкнулось с верхом - развернуть
				if(squareSettings.posY <= 0){
					squareSettings.posY = 0;
					squareSettings.speedY *= -1;
				}

				// Если столкнулось с правой стеной - развернуть
				if ((squareSettings.posX + squareSettings.size) >= frameBuffer.GetWidth() - 1)
				{
					squareSettings.posX = frameBuffer.GetWidth() - squareSettings.size - 1;
					squareSettings.speedX *= -1;
				}

				// Если столкнулось с низом - развернуть
				if ((squareSettings.posY + squareSettings.size) >= frameBuffer.GetHeight() - 1)
				{
					squareSettings.posY = frameBuffer.GetHeight() - squareSettings.size - 1;
					squareSettings.speedY *= -1;
				}

				// Очистить кадр 
				frameBuffer.Clear({ 0,0,0 });

				// Нарисовать квадрат
				SetSquare(
					&frameBuffer,
					static_cast<int>(squareSettings.posX), 
					static_cast<int>(squareSettings.posY), 
					static_cast<int>(squareSettings.size), 
					{ 0,255,0 });


				// Сообщение "перерисовать", чтобы показать обновленный кадр
				SendMessage(mainWindow, WM_PAINT, NULL, NULL);

				// Обновить "время последнего кадра"
				lastFrameTime = currentFrameTime;
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
* \brief Установка пикселя
* \param buffer Буфер кадра (указатель объект)
* \param x Положение по оси X
* \param y Положение по ост Y
* \param color Цвет очистки
*/
void SetPoint(BitmapBuffer * buffer, int x, int y, BitmapRGB color)
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
* \brief Рисование линии (медленный вариант, с использованием чисел с плавающей точкой)
* \param buffer Буфер кадра (указатель объект)
* \param x0 Начальная точка (компонента X)
* \param y0 Начальная точка (компонента Y)
* \param x1 Конечная точка (компонента X)
* \param y1 Конечная точка (компонента Y)
* \param color Цвет очистки
*/
void SetLine(BitmapBuffer * buffer, int x0, int y0, int x1, int y1, BitmapRGB color)
{
	int const deltaX = x1 - x0;
	int const deltaY = y1 - y0;

	if (std::abs(deltaX) >= std::abs(deltaY))
	{
		double const k = static_cast<double>(deltaY) / static_cast<double>(deltaX);
		for (int i = 0; deltaX > 0 ? i <= deltaX : i >= deltaX; deltaX > 0 ? i++ : i--)
		{
			int x = i + x0;
			int y = static_cast<int>(i * k) + y0;
			SetPoint(buffer, x, y, color);
		}
	}
	else
	{
		double const k = static_cast<double>(deltaX) / static_cast<double>(deltaY);
		for (int i = 0; deltaY > 0 ? i <= deltaY : i >= deltaY; deltaY > 0 ? i++ : i--)
		{
			int x = static_cast<int>(i * k) + x0;
			int y = i + y0;
			SetPoint(buffer, x, y, color);
		}
	}
}

/**
* \brief Рисование линии (алгоритм предшествующий классическому алгоритму Брэзенхема)
* \param buffer Буфер кадра (указатель объект)
* \param x0 Начальная точка (компонента X)
* \param y0 Начальная точка (компонента Y)
* \param x1 Конечная точка (компонента X)
* \param y1 Конечная точка (компонента Y)
* \param color Цвет очистки
*/
void SetLinePreBresenham(BitmapBuffer* buffer, int x0, int y0, int x1, int y1, BitmapRGB color)
{
	int const deltaX = x1 - x0;
	int const deltaY = y1 - y0;
	int const absDeltaX = std::abs(deltaX);
	int const absDeltaY = std::abs(deltaY);

	float accretion = 0;

	if (absDeltaX >= absDeltaY)
	{
		int y = y0;
		const int direction = deltaY != 0 ? (deltaY > 0 ? 1 : -1) : 0;
		for (int x = x0; deltaX > 0 ? x <= x1 : x >= x1; deltaX > 0 ? x++ : x--)
		{
			SetPoint(buffer, x, y, color);
			accretion += static_cast<float>(absDeltaY) / static_cast<float>(absDeltaX);

			if (accretion >= 1.0f)
			{
				accretion -= 1.0f;
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
			accretion += static_cast<float>(absDeltaX) / static_cast<float>(absDeltaY);

			if (accretion >= 1.0f)
			{
				accretion -= 1.0f;
				x += direction;
			}
		}
	}
}

/**
* \brief Рисование линии (быстрый вариант, алгоритм Брэзенхема)
* \param buffer Буфер кадра (указатель объект)
* \param x0 Начальная точка (компонента X)
* \param y0 Начальная точка (компонента Y)
* \param x1 Конечная точка (компонента X)
* \param y1 Конечная точка (компонента Y)
* \param color Цвет очистки
*/
void SetLineBresenham(BitmapBuffer * buffer, int x0, int y0, int x1, int y1, BitmapRGB color)
{
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

			if (accretion >= absDeltaX)
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

			if (accretion >= absDeltaY)
			{
				accretion -= absDeltaY;
				x += direction;
			}
		}
	}
}

/**
* \brief Рисование квадрата
* \param buffer Буфер кадра (указатель объект)
* \param x Положение левого верхнего угла по оси X
* \param y Положение левого верхнего угла по оси Y
* \param size Размер стороны
* \param color Цвет
*/
void SetSquare(BitmapBuffer* buffer, int x, int y, int size, BitmapRGB color)
{
	SetLineBresenham(buffer, x, y, x + size - 1, y, color);
	SetLineBresenham(buffer, x, y, x, y + size - 1, color);
	SetLineBresenham(buffer, x + (size - 1), y, x + (size - 1), y + (size - 1), color);
	SetLineBresenham(buffer, x, y + (size - 1), x + (size - 1), y + (size - 1), color);
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