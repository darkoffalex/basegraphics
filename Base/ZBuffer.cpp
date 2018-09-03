#include "ZBuffer.h"
#include <algorithm>


namespace gfx
{
	/**
	* \brief Конструктор по умоланию. Создает пустой буфер
	*/
	ZBuffer::ZBuffer() :width_(0), height_(0), data_(nullptr) {}

	/**
	* \brief Конструктор. Создает буфер нужных размеров и цвета
	* \param width Ширина
	* \param height Высота
	* \param clear Значение по умолчанию
	*/
	ZBuffer::ZBuffer(const unsigned width, const unsigned height, float clear) :
		width_(width),
		height_(height),
		data_((width * height) > 0 ? new float[width*height] : nullptr)
	{
		if (this->data_)
		{
			std::fill_n(this->data_, this->width_ * this->height_, clear);
		}
	}

	/**
	* \brief Конструктор копирования
	* \param other Ссылка на другой объект
	* \details Вызывается, когда инициализируем объект другим (созданным) объектом либо присваевыем другой объект во время создания
	*/
	ZBuffer::ZBuffer(const ZBuffer& other) :
		width_(other.width_),
		height_(other.height_),
		data_(other.data_ ? new float[other.width_ * other.height_] : nullptr)
	{
		if (other.data_)
		{
			memcpy(this->data_, other.data_, other.width_ * other.height_ * sizeof(float));
		}
	}

	/**
	* \brief Конструктор перемещения (обмена данными)
	* \param other R-value ссылка на другой объект
	* \details Вызывается, например, когда при создании присваеваем то что возвращает функция (R-value)
	*/
	ZBuffer::ZBuffer(ZBuffer&& other) noexcept
	{
		std::swap(this->width_, other.width_);
		std::swap(this->height_, other.height_);
		std::swap(this->data_, other.data_);
	}

	/**
	* \brief Оператор присвоения-копирования
	* \param other Ссылка на другой объект
	* \details Вызывается при присовении одного созданного объекта другому созданному объекту
	* \return Ссылка на текущий объект
	*/
	ZBuffer& ZBuffer::operator=(const ZBuffer& other)
	{
		delete[] this->data_;

		this->width_ = other.width_;
		this->height_ = other.height_;

		if (other.data_ && (other.width_ * other.height_) > 0)
		{
			memcpy(this->data_, other.data_, other.width_ * other.height_ * sizeof(float));
		}

		return *this;
	}

	/**
	* \brief Оператор присвоения-перемещения
	* \param other R-value ссылка на другой объект
	* \details Вызывается когда присваевыем объекту R-value значение (например возвращаемое функцией)
	* \return Ссылка на текущий объект
	*/
	ZBuffer& ZBuffer::operator=(ZBuffer&& other) noexcept
	{
		std::swap(this->width_, other.width_);
		std::swap(this->height_, other.height_);
		std::swap(this->data_, other.data_);
		return *this;
	}

	/**
	* \brief Оператор [] для обращения к данным как к двумерному массиву
	* \param y Номер ряда пикслей
	* \return  Подмассив (ряд пикслей)
	*/
	float* ZBuffer::operator[](int y)
	{
		return this->data_ + this->width_ * y;
	}

	/**
	* \brief Деструктор
	*/
	ZBuffer::~ZBuffer()
	{
		delete[] this->data_;
	}

	/**
	* \brief Получить размер буфера (в байтах)
	* \return Число байт
	*/
	unsigned ZBuffer::GetSize() const
	{
		return this->width_ * this->height_ * sizeof(float);
	}

	/**
	* \brief Заливка буфера цветом
	* \param value Значение
	*/
	void ZBuffer::Clear(float value)
	{
		if ((this->width_ * this->height_) > 0 && this->data_)
		{
			std::fill_n(this->data_, this->width_ * this->height_, value);
		}
	}

	/**
	* \brief Получение данных
	* \return Указатель на массив пикслей
	*/
	float* ZBuffer::GetData() const
	{
		return this->data_;
	}

	/**
	* \brief Получение ширины
	* \return Ширина
	*/
	unsigned ZBuffer::GetWidth() const
	{
		return this->width_;
	}

	/**
	* \brief Получение высоты
	* \return Высота
	*/
	unsigned ZBuffer::GetHeight() const
	{
		return this->height_;
	}

	/**
	* \brief Находится ли точка в пределах допустимой области
	* \param x Координата X
	* \param y Координата Y
	* \return Да или нет
	*/
	bool ZBuffer::IsPointInBounds(int x, int y) const
	{
		if (this->GetSize() == 0) {
			return false;
		}

		return x <= static_cast<int>(this->GetWidth() - 1) &&
			x >= 0 &&
			y <= static_cast<int>(this->GetHeight() - 1) &&
			y >= 0;
	}
}
