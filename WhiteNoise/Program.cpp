#include <iostream>
#include <Windows.h>

#include "../Base/Gfx.h"
#include "../Base/TextureBuffer.h"

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
 * \brief Заполнить пиксели изображения случайными цветами
 * \param image Буфер изображения
 */
void GenerateWhiteNoise(gfx::TextureBuffer* image);

/**
 * \brief Заполнить пиксели изображения случайными цветами (оптимизировано)
 * \param image Буфер изображения
 */
void GenerateWhiteNoiseFast(gfx::TextureBuffer* image);

// Буфер кадра
gfx::TextureBuffer frameBuffer;

/**
* \brief Точка входа
* \param argc Кол-во аргументов
* \param argv Агрументы (строки)
* \return Код выхода
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
			L"WhiteNoise",
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
		frameBuffer = gfx::TextureBuffer(clientRect.right, clientRect.bottom);
		std::cout << "INFO: Frame-buffer initialized  (size : " << frameBuffer.GetSize() << " bytes)" << std::endl;

		// Заполнить изрбражение белым шумом
		GenerateWhiteNoiseFast(&frameBuffer);

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
				// Генерация белого шума
				GenerateWhiteNoiseFast(&frameBuffer);

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
		gfx::PresentFrame(&frameBuffer, hWnd);
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
* \brief Заполнить пиксели изображения случайными цветами
* \param image Буфер изображения
*/
void GenerateWhiteNoise(gfx::TextureBuffer* image)
{
	for (unsigned y = 0; y < image->GetHeight(); y++)
	{
		for (unsigned x = 0; x < image->GetWidth(); x++)
		{
			unsigned char r = rand() % 254 + 1;
			gfx::SetPoint(image, x, y, { r,r,r,0 });
		}
	}
}

/**
* \brief Заполнить пиксели изображения случайными цветами (оптимизировано)
* \param image Буфер изображения
*/
void GenerateWhiteNoiseFast(gfx::TextureBuffer* image)
{
	unsigned pixelsQnt = image->GetWidth() * image->GetHeight();
	for(unsigned i = 0; i < pixelsQnt; i++)
	{
		unsigned char r = rand() % 254 + 1;
		image->GetData()[i] = { r,r,r,0 };
	}
}
