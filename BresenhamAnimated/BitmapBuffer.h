#pragma once

/**
* \brief Структура описывающая цвет. Используется в хранении и представлении изображения, т.е
* массив пикслей, который используется для показа средствами WinAPI, представляет
* из себя массив данных структур. Порядок следования полей (цветов) в структуре важен.
*/
struct BitmapDataRGB
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char reserved;
};

/**
* \brief Структура описывающая цвет. Используется при инициализации объекта класса и во время вызовов
* различных методов класса в качестве их аргументов. От BitmapDataRGB отличается, по сути, только
* форматом (порядком цветов и отсутсивем 4-го члена). Суть данной структуры в удобстве интерфейса (RGB - более привычно)
*/
struct BitmapRGB
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

/**
* \brief Класс буфера данных изображения
*/
class BitmapBuffer
{
private:
	unsigned int width_;           // Ширина изображения
	unsigned int height_;          // Высота изображения
	BitmapDataRGB * data_;         // Данные о цветах пикселей
	BitmapDataRGB ** pixelAccess_; // Массив указателей на "под-массивы" в data_ (доступ как в 2-мерном массиве)

public:

	/**
	* \brief Конструктор по умолчанию (инициализирует пустой буфер, без данных);
	*/
	BitmapBuffer();

	/**
	* \brief Конструктор. Инициализирует буфер заданных размеров заполняя его заданным цветом
	* \param width Ширина нового изображения
	* \param height Высота нового изображения
	* \param clearColor Цвет по умолчанию
	*/
	BitmapBuffer(const unsigned int width, const unsigned int height, BitmapRGB clearColor = { 0,0,0 });

	/**
	* \brief Конструктор копирования. Осуществляет копирование данных data_ и pixelAccess_ для нового объекта
	* \param other Копируемый объект
	*/
	BitmapBuffer(const BitmapBuffer& other);

	/**
	* \brief Конструктор перемещения. Осуществляет обмен данными с перемещаемым объектом
	* \param other R-value ссылка на объект с которым происходит обмен данными
	*/
	BitmapBuffer(BitmapBuffer&& other) noexcept;

	/**
	* \brief Оператор присвоения-копирования. Осуществляет копирование данных data_ и pixelAccess_ для нового объекта
	* \param other Присваеваемый объект
	* \return Ссылка на текущий объект
	*/
	BitmapBuffer& operator=(const BitmapBuffer& other);

	/**
	* \brief Оператор присвоения-перемещения. Осуществляет обмен данными с присваеваемым объектом
	* \param other Присваеваемый объект
	* \return Ссылка на текущий объект
	*/
	BitmapBuffer& operator=(BitmapBuffer&& other) noexcept;

	/**
	* \brief Оператор обращения к массиву
	* \param y Индекс ряда
	* \return Массив пикселей ряда
	*/
	BitmapDataRGB* operator[](int y);

	/**
	* \brief Очистка данных data_ и освобождение массива pixelAccess_
	*/
	~BitmapBuffer();

	/**
	* \brief Получить размер буфера
	* \return Размер в байтах
	*/
	unsigned int GetSize() const;

	/**
	* \brief Заполнить буфер заданным цветом
	* \param color Звет заливки
	*/
	void Clear(BitmapRGB color);

	/**
	* \brief Получить данные буфера
	* \return Указатель на массив пикслей
	*/
	BitmapDataRGB * GetData() const;

	/**
	* \brief Доступ к пикселям буфера
	* \return Указатель на массив указателей на массивы BitmapDataRGB
	*/
	BitmapDataRGB ** Pixels();

	/**
	* \brief Получить ширину
	* \return Ширина (uint)
	*/
	unsigned int GetWidth() const;

	/**
	* \brief Получить высоту
	* \return Высота (uint)
	*/
	unsigned int GetHeight() const;
};