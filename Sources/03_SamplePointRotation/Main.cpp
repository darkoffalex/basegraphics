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
    eNoErrors,
    eClassRegistrationError,
    eWindowCreationError,
};

/// Дескриптор исполняемого модуля программы
HINSTANCE g_hInstance = nullptr;
/// Дескриптор осноного окна отрисовки
HWND g_hwnd = nullptr;
/// Дескриптор контекста отрисовки
HDC g_hdc = nullptr;
/// Наименование класса
const char* g_strClassName = "MainWindowClass";
/// Заголовок окна
const char* g_strWindowCaption = "DemoApp";
/// Код последней ошибки
ErrorCode g_lastError = ErrorCode::eNoErrors;
/// Таймер
tools::Timer* g_pTimer = nullptr;

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
 * Точка входа
 * @param argc Кол-во аргументов
 * @param argv Аргмуенты
 * @return Код исполнения
 */
int main(int argc, char* argv[])
{
    try {
        // Получение дескриптора исполняемого модуля программы
        g_hInstance = GetModuleHandle(nullptr);

        // Информация о классе
        WNDCLASSEX classInfo;
        classInfo.cbSize = sizeof(WNDCLASSEX);
        classInfo.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        classInfo.cbClsExtra = 0;
        classInfo.cbWndExtra = 0;
        classInfo.hInstance = g_hInstance;
        classInfo.hIcon = LoadIcon(g_hInstance, IDI_APPLICATION);
        classInfo.hIconSm = LoadIcon(g_hInstance, IDI_APPLICATION);
        classInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
        classInfo.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
        classInfo.lpszMenuName = nullptr;
        classInfo.lpszClassName = g_strClassName;
        classInfo.lpfnWndProc = WindowProcedure;

        // Пытаемся зарегистрировать оконный класс
        if (!RegisterClassEx(&classInfo)) {
            g_lastError = ErrorCode::eClassRegistrationError;
            throw std::runtime_error("ERROR: Can't register window class.");
        }

        // Создание окна
        g_hwnd = CreateWindow(
                g_strClassName,
                g_strWindowCaption,
                WS_OVERLAPPEDWINDOW,
                0, 0,
                800, 600,
                nullptr,
                nullptr,
                g_hInstance,
                nullptr);

        // Если не удалось создать окно
        if (!g_hwnd) {
            g_lastError = ErrorCode::eWindowCreationError;
            throw std::runtime_error("ERROR: Can't create main application window.");
        }

        // Показать окно
        ShowWindow(g_hwnd, SW_SHOWNORMAL);

        // Получение контекста отрисовки
        g_hdc = GetDC(g_hwnd);

        // Размеры клиентской области окна
        RECT clientRect;
        GetClientRect(g_hwnd, &clientRect);

        // Создать буффер кадра
        auto frameBuffer = gfx::ImageBuffer<RGBQUAD>(clientRect.right, clientRect.bottom, {0, 0, 0, 0});
        std::cout << "INFO: Frame-buffer initialized  (resolution : " << frameBuffer.getWidth() << "x" << frameBuffer.getHeight() << ", size : " << frameBuffer.getSize() << " bytes)" << std::endl;

        // Пропорции области вида
        float aspectRatio = static_cast<float>(clientRect.right) / static_cast<float>(clientRect.bottom);

        // Центральная точка
        math::Vec2<float> center = {0.0f,0.0f};

        // Точки квадрата
        std::vector<math::Vec2<float>> quadPoints = {
                {-0.5f,-0.5f},
                {-0.5f,0.5f},
                {0.5f,0.5f},
                {0.5f,-0.5f}
        };

        // Текущий угол поворота
        float rotationAngle = 0.0f;

        // Скорость вращения
        float angleSpeed = 0.03f;

        /** MAIN LOOP **/

        // Таймер основного цикла (для выяснения временной дельты и FPS)
        g_pTimer = new tools::Timer();

        // Оконное сообщение
        MSG msg = {};

        // Запуск цикла
        while (true)
        {
            // Обновить таймер
            g_pTimer->updateTimer();

            // Обработка оконных сообщений
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT) {
                    break;
                }
            }

            // Поскольку показ FPS на окне уменьшает FPS - делаем это только тогда когда счетчик готов (примерно 1 раз в секунду)
            if (g_pTimer->isFpsCounterReady()){
                std::string fps = std::string(g_strWindowCaption).append(" (").append(std::to_string(g_pTimer->getFps())).append(" FPS)");
                SetWindowTextA(g_hwnd, fps.c_str());
            }

            // Точки после преобразований
            std::vector<math::Vec2<float>> pointsTransformed;

            // Обновление сцены
            {
                // Приращение угла поворота
                rotationAngle += (angleSpeed * g_pTimer->getDelta());

                // Пройтись по точкам квадрата
                for(const auto& point : quadPoints)
                {
                    // Повернуть точку на заданный угол
                    auto p = math::Rotate2D(point,rotationAngle);
                    // Корректировать положение точки с учетом пропорций жкрана (чтобы избежать растяжения)
                    p.x /= aspectRatio;
                    // Добавить в массив обработанных точек
                    pointsTransformed.push_back(p);
                }
            }

            // Рисование примитивов
            {
                // Нарисовать окружность в центре
                auto centerScreen = math::NdcToScreen(center,frameBuffer.getWidth(),frameBuffer.getHeight());
                gfx::SetCircle(&frameBuffer,centerScreen.x,centerScreen.y,60,{0,0,255});

                // Пройтись по всем трансформированным точкам
                for(size_t i = 0; i < pointsTransformed.size(); i++)
                {
                    // Индексы двух точек
                    size_t i0 = i;
                    size_t i1 = ((i+1) > (pointsTransformed.size()-1) ? 0 : i+1);

                    // Перевести точки в координаты экрана
                    auto ps0 = math::NdcToScreen(pointsTransformed[i0],frameBuffer.getWidth(),frameBuffer.getHeight());
                    auto ps1 = math::NdcToScreen(pointsTransformed[i1],frameBuffer.getWidth(),frameBuffer.getHeight());

                    // Нарисовать линию соединяющую точки
                    gfx::SetLine(&frameBuffer,ps0.x,ps0.y,ps1.x,ps1.y,{0,255,0});
                }
            }

            // Показ кадра
            PresentFrame(frameBuffer.getData(), static_cast<int>(frameBuffer.getWidth()), static_cast<int>(frameBuffer.getHeight()), g_hwnd);

            // Очистка кадра
            frameBuffer.clear({0,0,0,0});
        }
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    // Уничтожение окна
    DestroyWindow(g_hwnd);
    // Вырегистрировать класс окна
    UnregisterClass(g_strClassName, g_hInstance);

    // Код выполнения/ошибки
    return static_cast<int>(g_lastError);
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
