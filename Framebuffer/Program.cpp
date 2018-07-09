#include <iostream>
#include <Windows.h>

// Размеры сторон кадра (буфера)
const unsigned int FRAME_W = 320;
const unsigned int FRAME_H = 240;

// Буфер кадра
RGBQUAD frameBuffer[FRAME_H][FRAME_W];

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
* \brief Показ кадра (объявление)
* \param width Ширина
* \param height Высота
* \param pixels Массив пикселов
* \param hWnd Хендл окна, device context которого будет использован
*/
void PresentFrame(unsigned int width, unsigned int height, void* pixels, HWND hWnd);

/**
* \brief Установка пикселя (объявление)
* \param buffer Буфер кадра (указатель объект)
* \param x Положение по оси X
* \param y Положение по ост Y
* \param color Цвет очистки
*/
void SetPoint(void * buffer, int x, int y, RGBQUAD color = { 0, 0, 0, 0 });

/**
 * \brief Точка входа
 * \param argc Кол-во аргументов запуска
 * \param argv Аргументы (строки)
 * \return Код завершения
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
		classInfo.lpszClassName = L"FramebufferWindow";
		classInfo.lpfnWndProc = WndProc;

		// Если не удалось зарегистрировать класс
		if (!RegisterClassEx(&classInfo)) {
			throw std::runtime_error("ERROR: Can't register window class.");
		}

		// Создание окна
		HWND mainWindow = CreateWindow(
			classInfo.lpszClassName,
			L"Framebuffer",
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

		// Установить точку (через прямое обрещение к буферу)
		frameBuffer[10][200] = { 0,255,0,0 };

		// Установить точку (через метод)
		SetPoint(frameBuffer, 200, 9, { 0,0,255,0 });

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
		PresentFrame(FRAME_W, FRAME_H, frameBuffer, hWnd);
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
* \brief Показ кадра (реализация)
* \param width Ширина
* \param height Высота
* \param pixels Массив пикселов
* \param hWnd Хендл окна, device context которого будет использован
*/
void PresentFrame(unsigned int width, unsigned int height, void* pixels, HWND hWnd)
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

/**
* \brief Установка пикселя (Реализация)
* \param buffer Буфер кадра (указатель объект)
* \param x Положение по оси X
* \param y Положение по ост Y
* \param color Цвет очистки
*/
void SetPoint(void * buffer, int x, int y, RGBQUAD color)
{
	if(x >= 0 && x <= static_cast<int>(FRAME_W - 1) && y >= 0 && y <= static_cast<int>(FRAME_H - 1))
	{
		reinterpret_cast<RGBQUAD*>(buffer)[y * FRAME_W + x] = color;
	}
}
