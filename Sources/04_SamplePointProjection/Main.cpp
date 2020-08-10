#include <iostream>
#include <Windows.h>

#include <Math.hpp>
#include <Gfx.hpp>

/**
 * Коды ошибок
 */
enum ErrorCode
{
    NO_ERRORS,
    CLASS_REGISTRATION_ERROR,
    WINDOW_CREATION_ERROR,
};

/// Дескриптор исполняемого модуля программы
HINSTANCE _hInstance = nullptr;
/// Дескриптор осноного окна отрисовки
HWND _hwnd = nullptr;
/// Дескриптор контекста отрисовки
HDC _hdc = nullptr;
/// Наименование класса
const char* _strClassName = "MainWindowClass";
/// Заголовок окна
const char* _strWindowCaption = "DemoApp";
/// Код последней ошибки
ErrorCode _lastError = ErrorCode::NO_ERRORS;

/**
 * Обработчик оконных сообщений
 * @param hWnd Дескриптор окна
 * @param message Сообщение
 * @param wParam Параметр сообщения
 * @param lParam Параметр сообщения
 * @return Код выполнения
 */
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/**
 * Копирование информации о пикселях изображения в буфер "поверхности" окна
 * @param pixels Массив пикселей
 * @param width Ширина области
 * @param height Высота области
 * @param hWnd Дескриптор окна
 */
void PresentFrame(void *pixels, int width, int height, HWND hWnd);

/**
 * Нарисовать объект
 * @param frameBuffer Кадровый буфер
 * @param vertices Вершины
 * @param indices Индексы
 * @param color Цвет объекта
 * @param projectPerspective Перспективная проекция точек
 */
void DrawObject(gfx::ImageBuffer<RGBQUAD>* frameBuffer,
        const std::vector<math::Vec3<float>>& vertices,
        const std::vector<size_t>& indices,
        const RGBQUAD& color = {0,255,0,0},
        bool projectPerspective = true);

/**
 * Точка входа
 * @param argc Кол-во аргументов
 * @param argv Аргмуенты
 * @return Код исполнения
 */
int main(int argc, char* argv[])
{
    try {
        // Получение дескриптора исполняемого модуля программы
        _hInstance = GetModuleHandle(nullptr);

        // Информация о классе
        WNDCLASSEX classInfo;
        classInfo.cbSize = sizeof(WNDCLASSEX);
        classInfo.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        classInfo.cbClsExtra = 0;
        classInfo.cbWndExtra = 0;
        classInfo.hInstance = _hInstance;
        classInfo.hIcon = LoadIcon(_hInstance, IDI_APPLICATION);
        classInfo.hIconSm = LoadIcon(_hInstance, IDI_APPLICATION);
        classInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
        classInfo.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
        classInfo.lpszMenuName = nullptr;
        classInfo.lpszClassName = _strClassName;
        classInfo.lpfnWndProc = WindowProcedure;

        // Пытаемся зарегистрировать оконный класс
        if (!RegisterClassEx(&classInfo)) {
            _lastError = ErrorCode::CLASS_REGISTRATION_ERROR;
            throw std::runtime_error("ERROR: Can't register window class.");
        }

        // Создание окна
        _hwnd = CreateWindow(
                _strClassName,
                _strWindowCaption,
                WS_OVERLAPPEDWINDOW,
                0, 0,
                800, 600,
                nullptr,
                nullptr,
                _hInstance,
                nullptr);

        // Если не удалось создать окно
        if (!_hwnd) {
            _lastError = ErrorCode::WINDOW_CREATION_ERROR;
            throw std::runtime_error("ERROR: Can't create main application window.");
        }

        // Показать окно
        ShowWindow(_hwnd, SW_SHOWNORMAL);

        // Получение контекста отрисовки
        _hdc = GetDC(_hwnd);

        // Размеры клиентской области окна
        RECT clientRect;
        GetClientRect(_hwnd, &clientRect);

        // Создать буффер кадра
        auto frameBuffer = gfx::ImageBuffer<RGBQUAD>(clientRect.right, clientRect.bottom, {0, 0, 0, 0});
        std::cout << "INFO: Frame-buffer initialized  (resolution : " << frameBuffer.getWidth() << "x" << frameBuffer.getHeight() << ", size : " << frameBuffer.getSize() << " bytes)" << std::endl;

        // Пропорции области вида
        float aspectRatio = static_cast<float>(clientRect.right) / static_cast<float>(clientRect.bottom);

        // Положения вершин куба
        std::vector<math::Vec3<float>> vertices {
                {-1.0f,1.0f,1.0f},
                {1.0f,1.0f,1.0f},
                {1.0f,-1.0f,1.0f},
                {-1.0f,-1.0f,1.0f},

                {-1.0f,1.0f,-1.0f},
                {1.0f,1.0f,-1.0f},
                {1.0f,-1.0f,-1.0f},
                {-1.0f,-1.0f,-1.0f}
        };

        // Индексы (тройки вершин)
        std::vector<size_t> indices {
                0,1,2, 2,3,0,
                1,5,6, 6,2,1,
                5,4,7, 7,6,5,
                4,0,3, 3,7,4,
                4,5,1, 1,0,4,
                3,2,6, 6,7,3
        };

        // Пройти по всем индексам (шаг - 3 индекса)
        for(size_t i = 3; i <= indices.size(); i+=3)
        {
            // Пройтись по тройке индексов (по 2 чтобы игнорировать диагональные соединения)
            for(size_t j = 0; j < 2; j++)
            {
                // Получить положение двух точек
                auto p0 = vertices[indices[(i-3)+j]];
                auto p1 = vertices[indices[(i-3)+((j + 1) % 3)]];

                // Сдвигаем точки
                p0 = p0 + math::Vec3<float>(0.0f,0.0f,-4.0f);
                p1 = p1 + math::Vec3<float>(0.0f,0.0f,-4.0f);

                // Проекция положений двух точек
//                auto pp0 = math::ProjectOrthogonal(p0,-2.0f,2.0f,-2.0f,2.0f,0.1f,100.0f,aspectRatio);
//                auto pp1 = math::ProjectOrthogonal(p1,-2.0f,2.0f,-2.0f,2.0f,0.1f,100.0f,aspectRatio);

                auto pp0 = math::ProjectPerspective(p0,90.0f,0.1f,100.0f,aspectRatio);
                auto pp1 = math::ProjectPerspective(p1,90.0f,0.1f,100.0f,aspectRatio);

                // Положение в пространстве экрана
                auto sp0 = math::NdcToScreen({pp0.x, pp0.y}, frameBuffer.getWidth(), frameBuffer.getHeight());
                auto sp1 = math::NdcToScreen({pp1.x, pp1.y}, frameBuffer.getWidth(), frameBuffer.getHeight());

                // Написовать линию
                gfx::SetLine(&frameBuffer,sp0.x,sp0.y,sp1.x,sp1.y,{0,255,0,0},gfx::SAFE_CHECK_ALL_POINTS);
            }
        }

        /** MAIN LOOP **/

        // Оконное сообщение
        MSG msg = {};

        // Запуск цикла
        while (true)
        {
            // Обработка оконных сообщений
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) {
                    break;
                }
            }

            // Показ кадра
            PresentFrame(frameBuffer.getData(), static_cast<int>(frameBuffer.getWidth()), static_cast<int>(frameBuffer.getHeight()),_hwnd);
        }
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    // Уничтожение окна
    DestroyWindow(_hwnd);
    // Вырегистрировать класс окна
    UnregisterClass(_strClassName,_hInstance);

    // Код выполнения/ошибки
    return static_cast<int>(_lastError);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Нарисовать объект
 * @param frameBuffer Кадровый буфер
 * @param vertices Вершины
 * @param indices Индексы
 * @param color Цвет объекта
 * @param projectPerspective Перспективная проекция точек
 */
void DrawObject(gfx::ImageBuffer<RGBQUAD> *frameBuffer, const std::vector<math::Vec3<float>> &vertices,
                const std::vector<size_t> &indices, const RGBQUAD &color, bool projectPerspective)
{
    // Пропорции области вида
    float aspectRatio = static_cast<float>(frameBuffer->getWidth()) / static_cast<float>(frameBuffer->getHeight());

    // Пройти по всем индексам (шаг - 3 индекса)
    for(size_t i = 3; i <= indices.size(); i+=3)
    {
        // Точки треугольника
        std::vector<math::Vec2<int>> triangle;

        // Произвести необходимые преобразования кординат точек, для отображения на экране
        for(size_t j = 0; j < 3; j++)
        {
            auto p = vertices[indices[(i-3)+j]];
            auto pp = projectPerspective ? math::ProjectPerspective(p,45.0f,0.1f,100.0f,aspectRatio) : math::ProjectOrthogonal(p,-2.0f,2.0f,-2.0f,2.0f,0.1f,100.0f,aspectRatio);
            auto sp = math::NdcToScreen({pp.x, pp.y}, frameBuffer->getWidth(), frameBuffer->getHeight());
            triangle.push_back(sp);
        }

        // Написовать треугольник
        gfx::SetTriangle(
                frameBuffer,
                triangle[0].x,triangle[0].y,
                triangle[1].x,triangle[1].y,
                triangle[2].x,triangle[2].y,
                {0,255,0,0},
                false);
    }
}

/**
 * Обработчик оконных сообщений
 * @param hWnd Дескриптор окна
 * @param message Сообщение
 * @param wParam Параметр сообщения
 * @param lParam Параметр сообщения
 * @return Код выполнения
 */
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
            // При нажатии любой кнопки мыши
            break;

        case WM_MOUSEMOVE:
            // При движении мыши
            // Если зажата левая кнопка мыши
            if (wParam & MK_LBUTTON) {
            }
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

/**
 * Копирование информации о пикселях изображения в буфер "поверхности" окна
 * @param pixels Массив пикселей
 * @param width Ширина области
 * @param height Высота области
 * @param hWnd Дескриптор окна
 */
void PresentFrame(void *pixels, int width, int height, HWND hWnd)
{
    // Получить хендл на временный bit-map (4 байта на пиксель)
    HBITMAP hBitMap = CreateBitmap(
            width,
            height,
            1,
            8 * 4,
            pixels);

    // Получить device context окна
    HDC hdc = GetDC(hWnd);

    // Временный DC для переноса bit-map'а
    HDC srcHdc = CreateCompatibleDC(hdc);

    // Связать bit-map с временным DC
    SelectObject(srcHdc, hBitMap);

    // Копировать содержимое временного DC в DC окна
    BitBlt(
            hdc,                                  // HDC назначения
            0,                                 // Начало вставки по оси X
            0,                                 // Начало вставки по оси Y
            static_cast<int>(width),              // Ширина
            static_cast<int>(height),             // Высота
            srcHdc,                               // Исходный HDC (из которого будут копироваться данные)
            0,                                // Начало считывания по оси X
            0,                                // Начало считывания по оси Y
            SRCCOPY                               // Копировать
    );

    // Уничтожить bit-map
    DeleteObject(hBitMap);
    // Уничтожить временный DC
    DeleteDC(srcHdc);
    // Уничтожить DC
    ReleaseDC(hWnd,hdc);
}