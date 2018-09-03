#pragma once

namespace gfx
{
	/**
	* \brief ����� ������ ������� (z-�����). ���������
	* \details ��� �������� ������� ������� ������ � ������ ���������� ����������� ���-�� ����, ��������� data_ ������ ����� ����� �������
	*/
	class ZBuffer
	{

	private:
		unsigned int width_;         // ������ ������
		unsigned int height_;        // ������ ������
		float * data_;               // ��������� �� ������ �������� (������)

	public:
		/**
		* \brief ����������� �� ��������. ������� ������ �����
		*/
		ZBuffer();

		/**
		* \brief �����������. ������� ����� ������ �������� � �����
		* \param width ������
		* \param height ������
		* \param clear �������� �� ���������
		*/
		ZBuffer(const unsigned width, const unsigned height, float clear);

		/**
		* \brief ����������� �����������
		* \param other ������ �� ������ ������
		* \details ����������, ����� �������������� ������ ������ (���������) �������� ���� ����������� ������ ������ �� ����� ��������
		*/
		ZBuffer(const ZBuffer& other);

		/**
		* \brief ����������� ����������� (������ �������)
		* \param other R-value ������ �� ������ ������
		* \details ����������, ��������, ����� ��� �������� ����������� �� ��� ���������� ������� (R-value)
		*/
		ZBuffer(ZBuffer&& other) noexcept;

		/**
		* \brief �������� ����������-�����������
		* \param other ������ �� ������ ������
		* \details ���������� ��� ���������� ������ ���������� ������� ������� ���������� �������
		* \return ������ �� ������� ������
		*/
		ZBuffer& operator=(const ZBuffer& other);

		/**
		* \brief �������� ����������-�����������
		* \param other R-value ������ �� ������ ������
		* \details ���������� ����� ����������� ������� R-value �������� (�������� ������������ ��������)
		* \return ������ �� ������� ������
		*/
		ZBuffer& operator=(ZBuffer&& other) noexcept;

		/**
		* \brief �������� [] ��� ��������� � ������ ��� � ���������� �������
		* \param y ����� ���� �������
		* \return  ��������� (��� �������)
		*/
		float* operator[](int y);

		/**
		* \brief ����������
		*/
		~ZBuffer();

		/**
		* \brief �������� ������ ������ (� ������)
		* \return ����� ����
		*/
		unsigned int GetSize() const;

		/**
		* \brief ������� ������ ������
		* \param value ��������
		*/
		void Clear(float value);

		/**
		* \brief ��������� ������
		* \return ��������� �� ������ �������
		*/
		float * GetData() const;

		/**
		* \brief ��������� ������
		* \return ������
		*/
		unsigned int GetWidth() const;

		/**
		* \brief ��������� ������
		* \return ������
		*/
		unsigned int GetHeight() const;

		/**
		* \brief ��������� �� ����� � �������� ���������� �������
		* \param x ���������� X
		* \param y ���������� Y
		* \return �� ��� ���
		*/
		bool IsPointInBounds(int x, int y) const;
	};
}
