#pragma once

namespace gfx
{
	/**
	* \brief Класс буфера глубины (z-буфер). Интерфейс
	* \details При создании объекта данного класса в памяти выделяется необходимое кол-во байт, указатель data_ хранит адрес этого массива
	*/
	class ZBuffer
	{

	private:
		unsigned int width_;         // Ширина буфера
		unsigned int height_;        // Высота буфера
		float * data_;               // Указатель на массив пикселей (цветов)

	public:
		/**
		* \brief Конструктор по умоланию. Создает пустой буфер
		*/
		ZBuffer();

		/**
		* \brief Конструктор. Создает буфер нужных размеров и цвета
		* \param width Ширина
		* \param height Высота
		* \param clear Значение по умолчанию
		*/
		ZBuffer(const unsigned width, const unsigned height, float clear);

		/**
		* \brief Конструктор копирования
		* \param other Ссылка на другой объект
		* \details Вызывается, когда инициализируем объект другим (созданным) объектом либо присваевыем другой объект во время создания
		*/
		ZBuffer(const ZBuffer& other);

		/**
		* \brief Конструктор перемещения (обмена данными)
		* \param other R-value ссылка на другой объект
		* \details Вызывается, например, когда при создании присваеваем то что возвращает функция (R-value)
		*/
		ZBuffer(ZBuffer&& other) noexcept;

		/**
		* \brief Оператор присвоения-копирования
		* \param other Ссылка на другой объект
		* \details Вызывается при присовении одного созданного объекта другому созданному объекту
		* \return Ссылка на текущий объект
		*/
		ZBuffer& operator=(const ZBuffer& other);

		/**
		* \brief Оператор присвоения-перемещения
		* \param other R-value ссылка на другой объект
		* \details Вызывается когда присваевыем объекту R-value значение (например возвращаемое функцией)
		* \return Ссылка на текущий объект
		*/
		ZBuffer& operator=(ZBuffer&& other) noexcept;

		/**
		* \brief Оператор [] для обращения к данным как к двумерному массиву
		* \param y Номер ряда пикслей
		* \return  Подмассив (ряд пикслей)
		*/
		float* operator[](int y);

		/**
		* \brief Деструктор
		*/
		~ZBuffer();

		/**
		* \brief Получить размер буфера (в байтах)
		* \return Число байт
		*/
		unsigned int GetSize() const;

		/**
		* \brief Заливка буфера цветом
		* \param value Значение
		*/
		void Clear(float value);

		/**
		* \brief Получение данных
		* \return Указатель на массив пикслей
		*/
		float * GetData() const;

		/**
		* \brief Получение ширины
		* \return Ширина
		*/
		unsigned int GetWidth() const;

		/**
		* \brief Получение высоты
		* \return Высота
		*/
		unsigned int GetHeight() const;

		/**
		* \brief Находится ли точка в пределах допустимой области
		* \param x Координата X
		* \param y Координата Y
		* \return Да или нет
		*/
		bool IsPointInBounds(int x, int y) const;
	};
}
