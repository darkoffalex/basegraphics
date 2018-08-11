#include "TextureBuffer.h"
#include <algorithm>


namespace gfx
{
	/**
	* \brief Конструктор по умоланию. Создает пустой буфер
	*/
	TextureBuffer::TextureBuffer() :width_(0), height_(0), data_(nullptr) {}

	/**
	* \brief Конструктор. Создает буфер нужных размеров и цвета
	* \param width Ширина
	* \param height Высота
	* \param clear Цвет (по умолчанию черный)
	*/
	TextureBuffer::TextureBuffer(const unsigned width, const unsigned height, ColorBGR clear) :
		width_(width),
		height_(height),
		data_((width * height) > 0 ? new ColorBGR[width*height] : nullptr)
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
	TextureBuffer::TextureBuffer(const TextureBuffer& other):
		width_(other.width_),
		height_(other.height_),
		data_(other.data_ ? new ColorBGR[other.width_ * other.height_] : nullptr)
	{
		if (other.data_)
		{
			memcpy(this->data_, other.data_, other.width_ * other.height_ * sizeof(ColorBGR));
		}
	}

	/**
	* \brief Конструктор перемещения (обмена данными)
	* \param other R-value ссылка на другой объект
	* \details Вызывается, например, когда при создании присваеваем то что возвращает функция (R-value)
	*/
	TextureBuffer::TextureBuffer(TextureBuffer&& other) noexcept
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
	TextureBuffer& TextureBuffer::operator=(const TextureBuffer& other)
	{
		delete[] this->data_;

		this->width_ = other.width_;
		this->height_ = other.height_;

		if (other.data_ && (other.width_ * other.height_) > 0)
		{
			memcpy(this->data_, other.data_, other.width_ * other.height_ * sizeof(ColorBGR));
		}

		return *this;
	}

	/**
	* \brief Оператор присвоения-перемещения
	* \param other R-value ссылка на другой объект
	* \details Вызывается когда присваевыем объекту R-value значение (например возвращаемое функцией)
	* \return Ссылка на текущий объект
	*/
	TextureBuffer& TextureBuffer::operator=(TextureBuffer&& other) noexcept
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
	ColorBGR* TextureBuffer::operator[](int y)
	{
		return this->data_ + this->width_ * y;
	}

	/**
	* \brief Деструктор
	*/
	TextureBuffer::~TextureBuffer()
	{
		delete[] this->data_;
	}

	/**
	* \brief Получить размер буфера (в байтах)
	* \return Число байт
	*/
	unsigned TextureBuffer::GetSize() const
	{
		return this->width_ * this->height_ * sizeof(ColorBGR);
	}

	/**
	* \brief Заливка буфера цветом
	* \param color Цвет
	*/
	void TextureBuffer::Clear(const ColorBGR& color)
	{
		if((this->width_ * this->height_) > 0 && this->data_)
		{
			std::fill_n(this->data_, this->width_ * this->height_, color);
		}
	}

	/**
	* \brief Получение данных
	* \return Указатель на массив пикслей
	*/
	ColorBGR* TextureBuffer::GetData() const
	{
		return this->data_;
	}

	/**
	* \brief Получение ширины
	* \return Ширина
	*/
	unsigned TextureBuffer::GetWidth() const
	{
		return this->width_;
	}

	/**
	* \brief Получение высоты
	* \return Высота
	*/
	unsigned TextureBuffer::GetHeight() const
	{
		return this->height_;
	}

	/**
	* \brief Находится ли точка в пределах допустимой области
	* \param x Координата X
	* \param y Координата Y
	* \return Да или нет
	*/
	bool TextureBuffer::IsPointInBounds(int x, int y) const
	{
		if(this->GetSize() == 0){
			return false;
		}

		return x <= static_cast<int>(this->GetWidth() - 1) && 
			x >= 0 && 
			y <= static_cast<int>(this->GetHeight() - 1) && 
			y >= 0;
	}
}
