#include <iostream>
#include <Windows.h>
#include <string>

#include <Math.hpp>
#include <Gfx.hpp>
#include <Timer.hpp>
#include <utility>

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
 * Данный индекс означает отсутствие родителя у сустава
 */
#define JOINT_NO_PARENT 9999

/**
 * Структура описывающаяя "сустав"
 * Каждый сустав обладает своим набором точек и своей трансформацией относительно родительского сустава
 */
struct Joint
{
    /// Набор точек принадлежащих суставу (точки задаются в ПРОСТРАНСТВЕ СУСТАВА)
    std::vector<math::Vec4<float>> points;
    /// Матрица локальной трансформации относительно родителя
    math::Mat4<float> offset = math::Mat4<float>(1.0f);
    /// Матрица полной трансформации сустава
    math::Mat4<float> transformation = math::Mat4<float>(1.0f);
    /// Индекс родительского сустава
    size_t parentId = JOINT_NO_PARENT;

    /**
     * Конструктор
     * @param pointsIn Набор точек прикрепленных к суставу
     * @param offsetIn Трансформация сустава относительно родителя
     * @param parent Индекс родительского сустава
     */
    Joint(std::vector<math::Vec4<float>> pointsIn, const math::Mat4<float>& offsetIn, size_t parent = JOINT_NO_PARENT):
            points(std::move(pointsIn)),offset(offsetIn),parentId(parent)
    {}

    /**
     * Получить указатель на родительский узел (сустав)
     * @param allJoints Массив всех суставов
     * @return Указатель на объект структуры Joint
     */
    const Joint* getParent(const std::vector<Joint>& allJoints) const
    {
        if(parentId == JOINT_NO_PARENT || parentId > allJoints.size() - 1) return nullptr;
        return &(allJoints[parentId]);
    }

    /**
     * Получить массив дочерних узлов (суставов)
     * @param allJoints Массив всех суставов
     * @return Массив дочерних узлов
     */
    std::vector<const Joint*> getChildren(const std::vector<Joint>& allJoints) const
    {
        std::vector<const Joint*> children;

        int currentIndex = -1;

        for(size_t i = 0; i < allJoints.size(); i++)
        {
            if(&(allJoints[i]) == this){
                currentIndex = i;
            }
        }

        if(currentIndex >= 0)
        {
            for(const auto& joint : allJoints)
            {
                if(joint.parentId == static_cast<size_t>(currentIndex)){
                    children.push_back(&joint);
                }
            }
        }

        return children;
    }
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
        math::Mat4<float> mProjection = math::GetProjectionMatOrthogonal(-2.0f,2.0f,-2.0f,2.0f,0.1f,100.0f,aspectRatio);

        // Массив суставов
        std::vector<Joint> joints;

        // Добавить корневой сустав
        joints.push_back({
                {{0.0f,0.0f,0.0f,1.0f},{0.0f,1.0f,0.0f,1.0f}},
                math::GetTranslationMat4({0.0f,0.0f,0.0f}),
                JOINT_NO_PARENT
        });

        // Добавляем дочерний сустав для корневого (со смещением {0.0f,1.0f,0.0f})
        joints.push_back({
                {{0.0f,0.0f,0.0f,1.0f},{0.0f,0.5f,0.0f,1.0f}},
                math::GetTranslationMat4({0.0f,1.0f,0.0f}),
                0
        });

        // Добавляем дочерний сустав для второго (со смещением {0.0f,0.5f,0.0f})
        joints.push_back({
                {{0.0f,0.0f,0.0f,1.0f},{0.0f,0.25f,0.0f,1.0f}},
                math::GetTranslationMat4({0.0f,0.5f,0.0f}),
                1
        });

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

            // Точки после преобразований
            std::vector<math::Vec2<float>> pointsTransformed;

            /// U P D A T E
            {
                // Приращение угла поворота
                rotationAngle += (angleSpeed * _timer->getDelta());

                // Поворот суставов
                joints[0].transformation = joints[0].offset * math::GetRotationMat4({0.0f,0.0f,rotationAngle});
                joints[1].transformation = joints[1].offset * math::GetRotationMat4({0.0f,0.0f,rotationAngle * 2});
                joints[2].transformation = joints[2].offset * math::GetRotationMat4({0.0f,0.0f,rotationAngle * 4});

                // Проход по всем суставам
                for(auto& joint : joints)
                {
                    // Текущая трансформация
                    math::Mat4<float> currentTransform = joint.transformation;

                    // К матрице текущей трансформации применяются трансформации родителей, родителей родителей и т.д (иерархически)
                    const Joint* currentJoint = &joint;
                    while(currentJoint->parentId != JOINT_NO_PARENT){
                        currentJoint = currentJoint->getParent(joints);
                        currentTransform = currentJoint->transformation * currentTransform;
                    }

                    // Применяем трансформации и проекцию ко всем точкам
                    for(const auto& p : joint.points)
                    {
                        auto pTransformed = mProjection * currentTransform * p;
                        pointsTransformed.emplace_back(pTransformed.x,pTransformed.y);
                    }
                }
            }

            /// D R A W
            {
                // Пройтись по всем трансформированным точкам
                for(size_t i = 0; i < pointsTransformed.size(); i++)
                {
                    // Индексы двух точек
                    size_t i0 = i;
                    size_t i1 = i+1;

                    if(i+1 < pointsTransformed.size())
                    {
                        // Перевести точки в координаты экрана
                        auto ps0 = math::NdcToScreen(pointsTransformed[i0],frameBuffer.getWidth(),frameBuffer.getHeight());
                        auto ps1 = math::NdcToScreen(pointsTransformed[i1],frameBuffer.getWidth(),frameBuffer.getHeight());

                        // Нарисовать линию соединяющую точки
                        gfx::SetLine(&frameBuffer,ps0.x,ps0.y,ps1.x,ps1.y,{0,255,0});
                        gfx::SetCircle(&frameBuffer,ps0.x,ps0.y,5,{0,0,255});
                    }
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
