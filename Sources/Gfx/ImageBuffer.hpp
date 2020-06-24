#pragma once
#include <algorithm>

namespace gfx
{
    /**
     * Буфер данных двумерного изображения
     * @tparam T Тип или класс описывающий цвет одного элемента (текселя) текстуры
     */
    template<typename T>
    class ImageBuffer
    {
    private:
        unsigned width_ = 0;
        unsigned height_ = 0;
        T* data_;

    public:
        /**
         * Конструктор по умолчанию (инициализация пустого буфера)
         */
        ImageBuffer(): width_(0), height_(0), data_(nullptr){};

        /**
         * Конструктор
         * @param width Ширина изображения
         * @param height Высота изображения
         * @param clear Значение для очистки
         */
        ImageBuffer(const unsigned width, const unsigned height, const T& clear):
                width_(width),
                height_(height),
                data_((width * height) > 0 ? new T[width * height] : nullptr)
        {
            if(data_ != nullptr){
                std::fill_n(this->data_, this->width_ * this->height_, clear);
            }
        }

        /**
         * Конструктор копирования
         * Вызывается при инициализации объекта другим объектом (присвоение во веремя создания - этот же случай)
         * @param other Копируемый объекь
         */
        ImageBuffer(const ImageBuffer& other): ImageBuffer(other.width_, other.height_)
        {
            if (other.data_)
            {
                memcpy(this->data_, other.data_, other.width_ * other.height_ * sizeof(T));
            }
        }

        /**
         * Конструктор перемещения
         * Когда объект инициализируется из R-value ссылки (напр. объект возвращен функцией) и копировать нет смысла
         * @param other Перемещаемый объект
         */
        ImageBuffer(ImageBuffer&& other) noexcept : ImageBuffer()
        {
            // Омеенять ресурсы объектов
            std::swap(data_,other.data_);
            std::swap(width_,other.width_);
            std::swap(height_,other.height_);
        }

        /**
         * Оператор присвоения через копирование
         * Вызывается когда значение одного объекта присваивается другому
         * @param other Копируемый объекь
         * @return Ссылка на текущий объект
         */
        ImageBuffer& operator=(const ImageBuffer& other)
        {
            // Если присвоение самому себе
            if(this == &other)
                return *this;

            // Очистить ресурс текущего объекта
            if(data_) delete[] data_;
            data_ = nullptr;
            width_ = 0;
            height_ = 0;

            // Установить размеры
            this->width_ = other.width_;
            this->height_ = other.height_;

            // Если предпологается что буфер не пуст - выделить память и скопировать в нее данные
            if(width_ * height_ > 0){
                this->data_ = new T[other.width_ * other.height_];
                memcpy(this->data_, other.data_, other.width_ * other.height_ * sizeof(T));
            }

            // Вернуть текущий объект (ссылку)
            return *this;
        }

        /**
         * Оператор присвоения через перемещение по R-value ссылке
         * Вызывается когда объекту присваивается R-value значение (например возвращаемое функцией)
         * @param other Перемещаемый объект
         * @return Ссылка на текущий объект
         */
        ImageBuffer& operator=(ImageBuffer&& other) noexcept
        {
            // Если присваивание самому себе - просто вернуть ссылку на этот объект
            if (&other == this) return *this;

            // Очистить ресурс текущего объекта
            if(data_) delete[] data_;
            data_ = nullptr;
            width_ = 0;
            height_ = 0;

            // Омеенять ресурсы объектов
            std::swap(data_,other.data_);
            std::swap(width_,other.width_);
            std::swap(height_,other.height_);

            // Вернуть текущий объект (ссылку)
            return *this;
        }

        /**
         * Оператор для работы с буфером как с двумерным массивом
         * @param y Номер ряда
         * @return Указатель на часть массива данных
         */
        T* operator[](int y)
        {
            return this->data_ + this->width_ * y;
        }

        /**
         * Очистка ресурса
         */
        ~ImageBuffer()
        {
            delete[] data_;
        }

        /**
         * Получить размер в байтах
         * @return
         */
        [[nodiscard]] unsigned int getSize() const {
            return this->width_ * this->height_ * sizeof(T);
        }

        /**
         * Очистка буфера
         * @param clearValue
         */
        void clear(const T& clearValue){
            if((this->width_ * this->height_) > 0 && this->data_){
                std::fill_n(this->data_, this->width_ * this->height_, clearValue);
                //memset(this->data_,0,this->width_ * this->height_ * sizeof(T));
            }
        }

        /**
         * Получить данные
         * @return
         */
        T* getData(){
            return this->data_;
        }

        /**
         * Получить ширину
         * @return
         */
        [[nodiscard]] unsigned int getWidth() const
        {
            return this->width_;
        }

        /**
         * Получить высоту
         * @return
         */
        [[nodiscard]] unsigned int getHeight() const
        {
            return this->height_;
        }

        /**
         * Проверка попадания точки в границы буфера кадра
         * @param x Координаты точки по X
         * @param y Координаты точки по Y
         * @return Да или нет
         */
        [[nodiscard]] bool isPointIn(int x, int y) const
        {
            if(this->getSize() == 0) return false;

            return
                static_cast<unsigned>(x) <= (this->getWidth()-1) && static_cast<unsigned>(x) >= 0 &&
                static_cast<unsigned>(y) <= (this->getHeight()-1) && static_cast<unsigned>(y) >= 0;
        }
    };
}

