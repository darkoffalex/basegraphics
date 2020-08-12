#include <iostream>
#include <Windows.h>
#include <string>

#include <Math.hpp>
#include <Gfx.hpp>
#include <Timer.hpp>

#include "Skeleton.hpp"


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
 * Простое описание вершины
 * В отличии от реальных примеров, здесь отсутствуют веса и вершина может принадлежать только одной кости
 */
struct Vertex
{
    /// Положение в пространстве
    math::Vec3<float> position;

    /// Индек кости, к которой привязана вершины
    size_t boneId = 0;
};

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

        /// И Н И Ц И А Л И З А Ц И Я  С Ц Е Н Ы

        // Текущий угол поворота
        float rotationAngle = 0.0f;

        // Скорость вращения
        float angleSpeed = 0.03f;

        // Матрица проекции
        math::Mat4<float> mProjection = math::GetProjectionMatOrthogonal(-8.0f,8.0f,-8.0f,8.0f,0.1f,100.0f,aspectRatio);

        // Набор вершин
        std::vector<Vertex> vertices = {
                // Первый квдрат принадлежит корневой кости
                {{-1.0f,-1.0f,0.0f},0},
                {{-1.0f,1.0f,0.0f},0},
                {{1.0f,1.0f,0.0f},0},
                {{1.0f,-1.0f,0.0f},0},

                // 2-й квадрат принадлежит кости 1
                {{-1.0f,1.5f,0.0f},1},
                {{-1.0f,3.5f,0.0f},1},
                {{1.0f,3.5f,0.0f},1},
                {{1.0f,1.5f,0.0f},1},

                // 3-й квадрат принадлежит кости 2
                {{-1.0f,4.0f,0.0f},2},
                {{-1.0f,6.0f,0.0f},2},
                {{1.0f,6.0f,0.0f},2},
                {{1.0f,4.0f,0.0f},2},
        };

        // Инициализация скелета (скелет из 3 суставов/костей)
        Skeleton skeleton(3);
        skeleton.getRootBone()
        ->addChildBone(1,math::GetTranslationMat4({0.0f,2.5f,0.0f}))
        ->addChildBone(2,math::GetTranslationMat4({0.0f,2.5f,0.0f}));


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

            /// А Н И М А Ц И Я

            // Приращение угла поворота
            rotationAngle += (angleSpeed * _timer->getDelta());

            // Повороты суставов скелета
            skeleton.getRootBone()->setTransformation(math::GetRotationMat4({0.0f,0.0f,rotationAngle}));
            skeleton.getRootBone()->getChildrenBones()->back().setTransformation(math::GetRotationMat4({0.0f,0.0f,rotationAngle}));
            skeleton.getRootBone()->getChildrenBones()->back().getChildrenBones()->back().setTransformation(math::GetRotationMat4({0.0f,0.0f,rotationAngle}));

            /// D R A W

            // Точки после преобразований (2D)
            std::vector<math::Vec2<float>> pointsTransformed;

            // Дополнительные точки (для отметки костей)
            std::vector<math::Vec2<float>> bonesPointsTransformed;

            // Трансформация вершин
            // В данном цикле проходим по вершинам и применям к ним трансформации соответствующих костей
            for(const auto& v : vertices)
            {
                auto vt = mProjection * skeleton.getFinalBoneTransforms()[v.boneId] * math::Vec4<float>({v.position.x,v.position.y,v.position.z,1.0f});
                pointsTransformed.emplace_back(vt.x,vt.y);
            }

            // В данном цикле просто добавляем точки для дальнейшей индикации костей
            // Для этого используем нулевые точки в пространстве кости, и соостветствующие матрицы (fromBoneSpace - true)
            for(const auto& t : skeleton.getFinalBoneTransforms(true))
            {
                auto vt = mProjection * t * math::Vec4<float>({0.0f,0.0f,0.0f,1.0f});
                bonesPointsTransformed.emplace_back(vt.x,vt.y);
            }


            // Рисование примитивов по преобразовнным точкам
            size_t pointsPerPrimitive = 4;
            for(size_t i = 0; i < pointsTransformed.size(); i+=pointsPerPrimitive)
            {
                for(size_t j = i; j < i + pointsPerPrimitive; j++)
                {
                    // Индексы двух точек
                    size_t i0 = j;
                    size_t i1 = ((j+1) > (i+pointsPerPrimitive-1) ? i : j+1);

                    // Перевести точки в координаты экрана
                    auto ps0 = math::NdcToScreen(pointsTransformed[i0],frameBuffer.getWidth(),frameBuffer.getHeight());
                    auto ps1 = math::NdcToScreen(pointsTransformed[i1],frameBuffer.getWidth(),frameBuffer.getHeight());

                    // Нарисовать линию соединяющую точки
                    gfx::SetLine(&frameBuffer,ps0.x,ps0.y,ps1.x,ps1.y,{0,255,0});
                }
            }

            // Индикация костей
            for(size_t i = 0; i < bonesPointsTransformed.size(); i++)
            {
                // Индексы двух точек
                size_t i0 = i;
                size_t i1 = i+1;

                if(i1 < bonesPointsTransformed.size())
                {
                    // Перевести точки в координаты экрана
                    auto ps0 = math::NdcToScreen(bonesPointsTransformed[i0],frameBuffer.getWidth(),frameBuffer.getHeight());
                    auto ps1 = math::NdcToScreen(bonesPointsTransformed[i1],frameBuffer.getWidth(),frameBuffer.getHeight());

                    // Круги в местах точек
                    if(i == 0) gfx::SetCircle(&frameBuffer,ps0.x,ps0.y,5,{0,0,255});
                    gfx::SetCircle(&frameBuffer,ps1.x,ps1.y,5,{0,0,255});

                    // Нарисовать линию соединяющую точки
                    gfx::SetLine(&frameBuffer,ps0.x,ps0.y,ps1.x,ps1.y,{0,0,255});
                }
            }

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
