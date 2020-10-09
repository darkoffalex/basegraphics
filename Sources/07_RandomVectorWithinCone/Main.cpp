#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

#include <random>

#include <Math.hpp>
#include <Gfx.hpp>

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
 * Нарисовать линейные примитивы
 * @param imageBuffer Указатель на кадровый буфер
 * @param projectedToNdcPoints Точки спроецированные в NDC пространство
 * @param pointsPerPrimitive Количество точек на один примитив
 */
void DrawLinePrimitives(gfx::ImageBuffer<RGBQUAD>* imageBuffer, const std::vector<math::Vec2<float>>& projectedToNdcPoints, uint32_t pointsPerPrimitive);

/**
 * Спроецировать точки в NDC
 * @param mProjection Матрица проекции
 * @param points Массив точек в пространстве мира
 * @return Массив спроецированных точек (без учета глубины)
 */
std::vector<math::Vec2<float>> ProjectPoints(const math::Mat4<float>& mProjection, const std::vector<math::Vec3<float>>& points);

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

        // Матрица проекции
        math::Mat4<float> mProjection = math::GetProjectionMatPerspective(90.0f,aspectRatio,0.0f,100.0f);

        // Точки линий
        std::vector<math::Vec3<float>> linePoints {
        };

        // Точки квадрата (квадрат впереди на -4 единицы)
        std::vector<math::Vec3<float>> quadPoints = {
                {-1.0f,1.0f,-2.0f},
                {1.0f,1.0f,-2.0f},
                {1.0f,-1.0f,-2.0f},
                {-1.0f,-1.0f,-2.0f}
        };

        // Положение источника света
        math::Vec3<float> lightPosition = {0.0f,0.0f,-2.0f};
        // Положение точки из которой в сторону источника будут сгенерированы линии
        math::Vec3<float> pointPosition = {0.0f,0.0f,-0.1f};
        // Радиус источника света
        float lightRadius = 1.0f;
        // Вектор направления к источнику
        auto toLight = math::Normalize(lightPosition - pointPosition);

        // Генератор случайных чисел
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        std::default_random_engine rndEngine(static_cast<unsigned>(ms.count()));

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

            // Очистить точки
            linePoints.clear();

            // Рандомизация
            ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            rndEngine.seed(static_cast<unsigned>(ms.count()));
            std::uniform_real_distribution<float> vectorDist(-1.0f,1.0f);
            std::uniform_real_distribution<float> radiusDist(0.0f,1.0f);

            for(uint32_t i = 0; i < 100; i++)
            {
                // Случайные значения компонентов вектора для генерации перпендикуряра
                float xR = vectorDist(rndEngine);
                float yR = vectorDist(rndEngine);
                float zR = vectorDist(rndEngine);

                // Случайное значение множителя радиуса
                float rR = radiusDist(rndEngine);

                // Получить случайную точку диске
                auto randomPerpendicular = math::Normalize(math::Cross(toLight, toLight + math::Vec3<float>(xR, yR, zR)));
                auto edgePoint = lightPosition + (randomPerpendicular * lightRadius * rR);

                linePoints.push_back(lightPosition);
                linePoints.push_back(edgePoint);
            }


            // Точки после преобразований (2D)
            std::vector<math::Vec2<float>> quadPointsTransformed = ProjectPoints(mProjection, quadPoints);
            std::vector<math::Vec2<float>> linePointsTransformed = ProjectPoints(mProjection, linePoints);
            // Нарисовать спроецированные примитивы
            DrawLinePrimitives(&frameBuffer,quadPointsTransformed,4);
            DrawLinePrimitives(&frameBuffer,linePointsTransformed,2);

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

/**
 * Нарисовать линейные примитивы
 * @param imageBuffer Указатель на кадровый буфер
 * @param projectedToNdcPoints Точки спроецированные в NDC пространство
 * @param pointsPerPrimitive Количество точек на один примитив
 */
void DrawLinePrimitives(gfx::ImageBuffer<RGBQUAD>* imageBuffer, const std::vector<math::Vec2<float>> &projectedToNdcPoints, uint32_t pointsPerPrimitive)
{
    for(size_t i = 0; i < projectedToNdcPoints.size(); i+=pointsPerPrimitive)
    {
        for(size_t j = i; j < i + pointsPerPrimitive; j++)
        {
            // Индексы двух точек
            size_t i0 = j;
            size_t i1 = ((j+1) > (i+pointsPerPrimitive-1) ? i : j+1);

            // Перевести точки в координаты экрана
            auto ps0 = math::NdcToScreen(projectedToNdcPoints[i0],imageBuffer->getWidth(),imageBuffer->getHeight());
            auto ps1 = math::NdcToScreen(projectedToNdcPoints[i1],imageBuffer->getWidth(),imageBuffer->getHeight());

            // Нарисовать линию соединяющую точки
            gfx::SetLine(imageBuffer,ps0.x,ps0.y,ps1.x,ps1.y,{0,255,0});
        }
    }
}

/**
 * Спроецировать точки в NDC
 * @param mProjection Матрица проекции
 * @param points Массив точек в пространстве мира
 * @return Массив спроецированных точек (без учета глубины)
 */
std::vector<math::Vec2<float>> ProjectPoints(const math::Mat4<float> &mProjection, const std::vector<math::Vec3<float>> &points)
{
    std::vector<math::Vec2<float>> result{};

    for(const auto& point : points)
    {
        // Проекция
        auto vt = mProjection * math::Vec4<float>({point.x,point.y,point.z,1.0f});
        // Перспективное деление
        vt = vt / vt.w;
        // Добавить в итоговый массив
        result.emplace_back(vt.x,vt.y);
    }

    return result;
}
