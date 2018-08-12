#include <iostream>
#include <Windows.h>

#include "../Base/Gfx.h"
#include "../Base/TextureBuffer.h"
#include "../Base/VertexBuffer.h"

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
* \brief Рисование полигональной модели
* \param image Буфер изображения
* \param model Буфер вершин
*/
void DrawPolygonModel(gfx::TextureBuffer* image, gfx::VertexBuffer* model);

// Буфер кадра
gfx::TextureBuffer frameBuffer;

// Буффер вершин
gfx::VertexBuffer vertexBuffer;

// Направление источника света (вперед)
gfx::Vector3D<float> lightDIrection(0.0f,0.0f,-1.0f);

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
		classInfo.lpszClassName = L"PolygonRendererWindow";
		classInfo.lpfnWndProc = WndProc;

		// Если не удалось зарегистрировать класс
		if (!RegisterClassEx(&classInfo)) {
			throw std::runtime_error("ERROR: Can't register window class.");
		}

		// Создание окна
		HWND mainWindow = CreateWindow(
			classInfo.lpszClassName,
			L"PolygonRenderer",
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

		// Загрузить вершины в буфер вершин из файла
		vertexBuffer.LoadFromFile("models/african_head.obj");
		std::cout << "INFO: Vertex-buffer initialized (size: " << vertexBuffer.GetSize() << " bytes, " << vertexBuffer.GetVertices().size() << " model, " << vertexBuffer.GetFaces().size() << " faces)" << std::endl;

		// Построить изображение модели
		DrawPolygonModel(&frameBuffer, &vertexBuffer);

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
* \brief Рисование полигональной модели
* \param image Буфер изображения
* \param model Буфер вершин
*/
void DrawPolygonModel(gfx::TextureBuffer* image, gfx::VertexBuffer* model)
{
	// Проход по всем полигонам модели (полигон - массив индексов вершин, обычно 3 индекса)
	for (unsigned int i = 0; i < model->GetFaces().size(); i++)
	{
		// Получение индексов текущего полигона
		std::vector<int> faceIndices = model->GetFaces()[i];

		// 2D точки треугольника (в координатах экрана)
		std::vector<gfx::Vector2D<int>> points2D;
		// 3D точки треугольника (в координатах сцены/мира)
		std::vector<gfx::Vector3D<float>> points3D;

		// Проход по всем точкам полигона (если считать что их 3)
		for (unsigned int j = 0; j < 3; j++)
		{
			// 3D точка треуглльника
			gfx::Vector3D<float> point3D = model->GetVertices()[faceIndices[j]];

			// Спроецированная на поверхность отображения 2D точка
			gfx::Vector2D<int> point2D = {
				static_cast<int>((point3D.x + 1.0f)*(image->GetWidth() / 2.0f)),
				static_cast<int>((point3D.y*-1 + 1.0f)*(image->GetHeight() / 2.0f))
			};
			
			// Добавить в массив точек
			points2D.push_back(point2D);
			points3D.push_back(point3D);
		}

		// Нормаль к полигону (вычисляется как векторное произведение двух ребер треугольника)
		gfx::Vector3D<float> normal = (points3D[2] - points3D[0]) ^ (points3D[1] - points3D[0]);
		// Привести единичной длине
		normal.Normalize(1.0f);
		// Вычислить интенсивность освещения (направлен ли источник на полигон или свет проход по касательной)
		// Используется скалярное произведение нормали и вектора направления освещения
		float intensity = normal*lightDIrection;

		// Если интенсивность более ноля (отбрасывать полигоны на которые не падает свет) и есть 3 точки, нарисовать закрашенный треугольник (полигон)
		if(intensity > 0 && points2D.size() > 2){
			// Получить оттенок серово учитывая интенсивность освещенности каждого полигона
			gfx::ColorBGR color = { 
				static_cast<unsigned char>(255 * intensity), 
				static_cast<unsigned char>(255 * intensity),
				static_cast<unsigned char>(255 * intensity),
				0 };

			// Нарисовать полигон
			gfx::SetPolygon(image, points2D[0], points2D[1], points2D[2], color);
		}
	}
}