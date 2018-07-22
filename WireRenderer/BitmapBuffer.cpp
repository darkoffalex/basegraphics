#include "BitmapBuffer.h"
#include <algorithm>

/**
* \brief Конструктор по умолчанию (инициализирует пустой объект, без данных);
*/
BitmapBuffer::BitmapBuffer() :width_(0), height_(0), data_(nullptr), pixelAccess_(nullptr) {}

/**
* \brief Конструктор. Инициализирует буфер заданных размеров заполняя его заданным цветом
* \param width Ширина нового изображения
* \param height Высота нового изображения
* \param clearColor Цвет по умолчанию
*/
BitmapBuffer::BitmapBuffer(const unsigned int width, const unsigned int height, BitmapRGB clearColor) :
	width_(width),
	height_(height),
	data_((width * height) > 0 ? new BitmapDataRGB[width*height] : nullptr),
	pixelAccess_(height > 0 ? new BitmapDataRGB*[height] : nullptr)
{
	if (this->data_)
	{
		const BitmapDataRGB clearColorData = { clearColor.blue,clearColor.green,clearColor.red,0 };
		std::fill_n(this->data_, this->width_ * this->height_, clearColorData);

		for (unsigned int i = 0; i < this->height_; i++) {
			this->pixelAccess_[i] = this->data_ + this->width_ * i;
		}
	}
}

/**
* \brief Конструктор копирования. Осуществляет копирование данных data_ и pixelAccess_ для нового объекта
* \param other Копируемый объект
*/
BitmapBuffer::BitmapBuffer(const BitmapBuffer& other) :
	width_(other.width_),
	height_(other.height_),
	data_(other.data_ ? new BitmapDataRGB[other.width_ * other.height_] : nullptr),
	pixelAccess_(other.pixelAccess_ ? new BitmapDataRGB*[other.height_] : nullptr)
{
	if (other.data_)
	{
		memcpy(this->data_, other.data_, other.width_ * other.height_ * sizeof(BitmapDataRGB));
	}

	if (other.pixelAccess_)
	{
		memcpy(this->pixelAccess_, other.pixelAccess_, other.height_ * sizeof(BitmapDataRGB*));
	}
}

/**
* \brief Конструктор перемещения. Осуществляет обмен данными с перемещаемым объектом
* \param other R-value ссылка на объект с которым происходит обмен данными
*/
BitmapBuffer::BitmapBuffer(BitmapBuffer&& other) noexcept :BitmapBuffer()
{
	std::swap(this->width_, other.width_);
	std::swap(this->height_, other.height_);
	std::swap(this->data_, other.data_);
	std::swap(this->pixelAccess_, other.pixelAccess_);
}

/**
* \brief Оператор присвоения-копирования. Осуществляет копирование данных data_ и pixelAccess_ для нового объекта
* \param other Присваеваемый объект
* \return Ссылка на текущий объект
*/
BitmapBuffer& BitmapBuffer::operator=(const BitmapBuffer& other)
{
	delete[] this->data_;
	delete[] this->pixelAccess_;

	this->width_ = other.width_;
	this->height_ = other.height_;

	if (other.data_ && (other.width_ * other.height_) > 0)
	{
		memcpy(this->data_, other.data_, other.width_ * other.height_ * sizeof(BitmapDataRGB));
		memcpy(this->pixelAccess_, other.pixelAccess_, other.height_ * sizeof(BitmapDataRGB*));
	}

	return *this;
}

/**
* \brief Оператор присвоения-перемещения. Осуществляет обмен данными с присваеваемым объектом
* \param other Присваеваемый объект (r-value сылка)
* \return Ссылка на текущий объект
*/
BitmapBuffer& BitmapBuffer::operator=(BitmapBuffer&& other) noexcept
{
	std::swap(this->width_, other.width_);
	std::swap(this->height_, other.height_);
	std::swap(this->data_, other.data_);
	std::swap(this->pixelAccess_, other.pixelAccess_);
	return *this;
}

/**
* \brief Оператор обращения к массиву
* \param y Индекс ряда
* \return Массив пикселей ряда
*/
BitmapDataRGB* BitmapBuffer::operator[](int y)
{
	return this->pixelAccess_[y];
}

/**
* \brief Очистка данных data_ и освобождение массива pixelAccess_
*/
BitmapBuffer::~BitmapBuffer()
{
	delete[] this->data_;
	delete[] this->pixelAccess_;
}

/**
* \brief Получить размер буфера
* \return Размер в байтах
*/
unsigned BitmapBuffer::GetSize() const
{
	return sizeof(BitmapDataRGB) * this->width_ * this->height_;
}

/**
* \brief Заполнить буфер заданным цветом
* \param color Звет заливки
*/
void BitmapBuffer::Clear(BitmapRGB color)
{
	if (this->data_) {
		const BitmapDataRGB clearColorData = { color.blue,color.green,color.red,0 };
		std::fill_n(this->data_, this->width_ * this->height_, clearColorData);
	}
}

/**
* \brief Получить данные буфера
* \return Указатель на массив пикслей
*/
BitmapDataRGB* BitmapBuffer::GetData() const
{
	return this->data_;
}

/**
* \brief Доступ к пикселям буфера
* \return Указатель на массив указателей на массивы BitmapDataRGB
*/
BitmapDataRGB** BitmapBuffer::Pixels()
{
	return this->pixelAccess_;
}

/**
* \brief Получить ширину
* \return Ширина (uint)
*/
unsigned BitmapBuffer::GetWidth() const
{
	return this->width_;
}

/**
* \brief Получить высоту
* \return Высота (uint)
*/
unsigned BitmapBuffer::GetHeight() const
{
	return this->height_;
}