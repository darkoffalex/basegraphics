#include "ZBuffer.h"
#include <algorithm>


namespace gfx
{
	/**
	* \brief ����������� �� ��������. ������� ������ �����
	*/
	ZBuffer::ZBuffer() :width_(0), height_(0), data_(nullptr) {}

	/**
	* \brief �����������. ������� ����� ������ �������� � �����
	* \param width ������
	* \param height ������
	* \param clear �������� �� ���������
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
	* \brief ����������� �����������
	* \param other ������ �� ������ ������
	* \details ����������, ����� �������������� ������ ������ (���������) �������� ���� ����������� ������ ������ �� ����� ��������
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
	* \brief ����������� ����������� (������ �������)
	* \param other R-value ������ �� ������ ������
	* \details ����������, ��������, ����� ��� �������� ����������� �� ��� ���������� ������� (R-value)
	*/
	ZBuffer::ZBuffer(ZBuffer&& other) noexcept
	{
		std::swap(this->width_, other.width_);
		std::swap(this->height_, other.height_);
		std::swap(this->data_, other.data_);
	}

	/**
	* \brief �������� ����������-�����������
	* \param other ������ �� ������ ������
	* \details ���������� ��� ���������� ������ ���������� ������� ������� ���������� �������
	* \return ������ �� ������� ������
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
	* \brief �������� ����������-�����������
	* \param other R-value ������ �� ������ ������
	* \details ���������� ����� ����������� ������� R-value �������� (�������� ������������ ��������)
	* \return ������ �� ������� ������
	*/
	ZBuffer& ZBuffer::operator=(ZBuffer&& other) noexcept
	{
		std::swap(this->width_, other.width_);
		std::swap(this->height_, other.height_);
		std::swap(this->data_, other.data_);
		return *this;
	}

	/**
	* \brief �������� [] ��� ��������� � ������ ��� � ���������� �������
	* \param y ����� ���� �������
	* \return  ��������� (��� �������)
	*/
	float* ZBuffer::operator[](int y)
	{
		return this->data_ + this->width_ * y;
	}

	/**
	* \brief ����������
	*/
	ZBuffer::~ZBuffer()
	{
		delete[] this->data_;
	}

	/**
	* \brief �������� ������ ������ (� ������)
	* \return ����� ����
	*/
	unsigned ZBuffer::GetSize() const
	{
		return this->width_ * this->height_ * sizeof(float);
	}

	/**
	* \brief ������� ������ ������
	* \param value ��������
	*/
	void ZBuffer::Clear(float value)
	{
		if ((this->width_ * this->height_) > 0 && this->data_)
		{
			std::fill_n(this->data_, this->width_ * this->height_, value);
		}
	}

	/**
	* \brief ��������� ������
	* \return ��������� �� ������ �������
	*/
	float* ZBuffer::GetData() const
	{
		return this->data_;
	}

	/**
	* \brief ��������� ������
	* \return ������
	*/
	unsigned ZBuffer::GetWidth() const
	{
		return this->width_;
	}

	/**
	* \brief ��������� ������
	* \return ������
	*/
	unsigned ZBuffer::GetHeight() const
	{
		return this->height_;
	}

	/**
	* \brief ��������� �� ����� � �������� ���������� �������
	* \param x ���������� X
	* \param y ���������� Y
	* \return �� ��� ���
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
