#include <iostream>
#include <Windows.h>
#include <string>

#include <Math.hpp>
#include <Gfx.hpp>
#include <Timer.hpp>

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
/// Таймер
tools::Timer* _timer = nullptr;

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
 * Нарисовать полигональный меш
 * @param frameBuffer Указатель на кадровый буфер
 * @param vertices Массив вершин
 * @param indices Массив индексов
 * @param position Положение меша
 * @param orientation Ориентация меша
 * @param color Цвет (RGB в диапозоне от 0 до 1)
 * @param projectPerspective Использовать перспективную проекцию
 * @param backFaceCulling Отбрасывать задние грани
 * @param fillFaces Заливка граней (false для сеточной отрисовки)
 */
void DrawMesh(gfx::ImageBuffer<RGBQUAD>* frameBuffer,
        const std::vector<math::Vec3<float>>& vertices,
        const std::vector<size_t>& indices,
        const math::Vec3<float>& position,
        const math::Vec3<float>& orientation,
        const math::Vec3<float>& color = {0.0f,1.0f,0.0f},
        bool projectPerspective = true,
        bool backFaceCulling = false,
        bool fillFaces = false);

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

        // Текущий угол поворота
        float rotationAngle = 0.0f;

        // Скорость вращения
        float angleSpeed = 0.02f;

        /** MAIN LOOP **/

        // Таймер основного цикла (для выяснения временной дельты и FPS)
        _timer = new tools::Timer();

        // Оконное сообщение
        MSG msg = {};

        // Запуск цикла
        while (true)
        {
            // Обновить таймер
            _timer->updateTimer();

            // Обработка оконных сообщений
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) {
                    break;
                }
            }

            // Поскольку показ FPS на окне уменьшает FPS - делаем это только тогда когда счетчик готов (примерно 1 раз в секунду)
            if (_timer->isFpsCounterReady()){
                std::string fps = std::string(_strWindowCaption).append(" (").append(std::to_string(_timer->getFps())).append(" FPS)");
                SetWindowTextA(_hwnd, fps.c_str());
            }

            // Приращение угла поворота
            rotationAngle += (angleSpeed * _timer->getDelta());

            // Нарисовать полигональный меш
            DrawMesh(
                    &frameBuffer,
                    vertices,
                    indices,
                    {0.0f,0.0f,-4.0f},
                    {rotationAngle,rotationAngle,0.0f},
                    {0.0f,1.0f,0.0f},
                    true,
                    true,
                    true);

            // Показ кадра
            PresentFrame(frameBuffer.getData(), static_cast<int>(frameBuffer.getWidth()), static_cast<int>(frameBuffer.getHeight()),_hwnd);

            // Очистка кадра
            frameBuffer.clear({0,0,0,0});
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

/**
 * Нарисовать полигональный меш
 * @param frameBuffer Указатель на кадровый буфер
 * @param vertices Массив вершин
 * @param indices Массив индексов
 * @param position Положение меша
 * @param orientation Ориентация меша
 * @param color Цвет (RGB в диапозоне от 0 до 1)
 * @param projectPerspective Использовать перспективную проекцию
 * @param backFaceCulling Отбрасывать задние грани
 * @param fillFaces Заливка граней (false для сеточной отрисовки)
 */
void DrawMesh(gfx::ImageBuffer<RGBQUAD>* frameBuffer,
              const std::vector<math::Vec3<float>>& vertices,
              const std::vector<size_t>& indices,
              const math::Vec3<float>& position,
              const math::Vec3<float>& orientation,
              const math::Vec3<float>& color,
              bool projectPerspective,
              bool backFaceCulling,
              bool fillFaces)
{
    // Пропорции области вида
    float aspectRatio = static_cast<float>(frameBuffer->getWidth()) / static_cast<float>(frameBuffer->getHeight());

    // Пройти по всем индексам (шаг - 3 индекса)
    for(size_t i = 3; i <= indices.size(); i+=3)
    {
        // Точки треугольника (в координатаъ экрана)
        std::vector<math::Vec2<int>> triangle;
        // Точки в NDC-координатах
        std::vector<math::Vec3<float>> triangleNdc;

        // Произвести необходимые преобразования кординат точек, для отображения на экране
        for(size_t j = 0; j < 3; j++)
        {
            // Получить точку
            auto p = vertices[indices[(i-3)+j]];

            // Вращение точки
            p = math::RotateAroundX(p,orientation.x);
            p = math::RotateAroundY(p,orientation.y);
            p = math::RotateAroundZ(p,orientation.z);

            // Смещение (перемещение) точки
            p = p + position;

            // Проекция точки
            auto pp = projectPerspective ?
                    math::ProjectPerspective(p,45.0f,0.1f,100.0f,aspectRatio) :
                    math::ProjectOrthogonal(p,-2.0f,2.0f,-2.0f,2.0f,0.1f,100.0f,aspectRatio);

            // Перевод в координаты экрана
            auto sp = math::NdcToScreen({pp.x, pp.y}, frameBuffer->getWidth(), frameBuffer->getHeight());

            // Добавить в треугольник (2D)
            triangle.push_back(sp);
            triangleNdc.push_back(p);
        }

        // Получить нормаль для отбрасывания задних граней (инвертируем, поскольку ось Y в координатах экрана инвертирована)
        auto normalForCulling = -math::Normalize(math::Cross(
                math::Normalize(math::Vec3<float>(triangle[2].x - triangle[0].x,triangle[2].y - triangle[0].y,0.0f)),
                math::Normalize(math::Vec3<float>(triangle[1].x - triangle[0].x,triangle[1].y - triangle[0].y,0.0f))
                ));

        // Скалярное произведения вектора к зрителю и нормали треугольника (показывает насколько сильно треугольник повернут к зрителю)
        auto dotForCulling = math::Dot(normalForCulling,{0.0f, 0.0f, 1.0f});

        // Если нужно отрисовывать треугольник (если включен backFaceCulling, то отрисовывается в случае если повернут к зрителю)
        if(!backFaceCulling || dotForCulling > 0)
        {
            // Яркость цвета
            float brightness = 1.0f;

            // Если нужна заливка граней
            if(fillFaces)
            {
                // Нормаль для вычисления освещенности
                auto normal = math::Normalize(math::Cross(
                        math::Normalize(triangleNdc[2] - triangleNdc[0]),
                        math::Normalize(triangleNdc[1] - triangleNdc[0])
                ));

                // Яркость тем сильнее, чем больше грань обернута к свету (считаем что свет исходит от зрителя)
                brightness = math::Dot(normal,{0.0f, 0.0f, 1.0f});
            }

            // Написовать треугольник
            gfx::SetTriangle(
                    frameBuffer,
                    triangle[0].x,triangle[0].y,
                    triangle[1].x,triangle[1].y,
                    triangle[2].x,triangle[2].y,
                    {
                        static_cast<unsigned char>(color.b * brightness * 255.0f),
                        static_cast<unsigned char>(color.g * brightness * 255.0f),
                        static_cast<unsigned char>(color.r * brightness * 255.0f),
                        0
                        },
                    fillFaces);
        }


    }
}
