#pragma once

#include <vector>
#include <Math.hpp>

class Skeleton
{
    class Bone
    {
    private:
        /// Указатель на скелет
        Skeleton* pSkeleton_;
        /// Индекс кости в линейном массиве
        size_t index_;
        /// Указатель на родительскую кость
        Bone* pParentBone_;
        /// Массив дочерних костей
        std::vector<Bone> childrenBones_;

        /// Смещение (расположение) относительно родитеской кости (можно считать это initial-положением)
        math::Mat4<float> localBindTransform_;
        /// Локальная трансформация (та трансформация, которая может назначаться, например "поворот руки на n градусов")
        math::Mat4<float> localTransform_;

        /// Результируюшая трансформация кости с учетом локальной и учетом трансформаций родительских костей
        /// Служит для перевода точек из пространства кости в пространство модели (с учетом локальной трансформации)
        math::Mat4<float> totalTransform_;

        /// Результирующая трансформация кости БЕЗ учета локальной, но с учетом трансформаций родительских костей
        /// В последстивии инвертируется для перевода точек из пространства модели в пространство кости (без учета локальной трансформации)
        math::Mat4<float> totalBindTransform_;

        /**
         * Подсчет итоговым матриц трансформации для ветки начиная с текущей кости
         */
        void calculateBranch()
        {
            // Если у кости есть родительская кость
            if(pParentBone_ != nullptr)
            {
                // Общее смещщение кости - точка смещается на локальное смещение, затем на общее родитеское смещение
                totalBindTransform_ = pParentBone_->totalBindTransform_ * this->localBindTransform_;
                // Общая трансформация кости - точка смещается на локальную (анимированную) трансформацию, затем на начальнон смещение кости, затем на родительскую общую трансформацию
                totalTransform_ = pParentBone_->totalTransform_ * this->localBindTransform_ * this->localTransform_;
            }
            // Если нет родительской кости - считать кость корневой
            else
            {
                totalBindTransform_ = this->localBindTransform_;
                totalTransform_ = this->localBindTransform_ * this->localTransform_;
            }

            // Если есть указатель на объект скелета и индекс валиден
            if(pSkeleton_ != nullptr && index_ < pSkeleton_->finalTransformations_.size())
            {
                // Итоговая матрица трансформации для точек в простнастве модели
                pSkeleton_->finalTransformations_[index_] = totalTransform_ * math::Inverse(totalBindTransform_);

                // Итоговая матрица трансформации для точек в пространстве кости
                pSkeleton_->finalTransformationFromBoneSpace_[index_] = totalTransform_;
            }

            // Рекрсивно выполнить для дочерних элементов (если они есть)
            if(!this->childrenBones_.empty()){
                for(auto& childBone : this->childrenBones_){
                    childBone.calculateBranch();
                }
            }
        }

    public:
        /**
         * Конструктор по умолчанию
         */
        Bone():
                pSkeleton_(nullptr),
                index_(0),
                pParentBone_(nullptr),
                localBindTransform_(math::Mat4<float>(1.0f)),
                localTransform_(math::Mat4<float>(1.0f)),
                totalTransform_(math::Mat4<float>(1.0f)),
                totalBindTransform_(math::Mat4<float>(1.0f)){};

        /**
         * Основной конструктор кости
         * @param pSkeleton Указатель на объект скелета
         * @param index Индекс кости в линейном массиве трансформаций
         * @param parentBone Указатель на родительскую кость
         * @param localBindTransform Смещение (расположение) относительно родитеской кости
         * @param localTransform  Локальная трансформация (та трансформация, которая может назначаться, например "поворот руки на n градусов")
         */
        explicit Bone(Skeleton* pSkeleton,
                      size_t index, Bone* parentBone = nullptr,
                      const math::Mat4<float>& localBindTransform = math::Mat4<float>(1.0f),
                      const math::Mat4<float>& localTransform = math::Mat4<float>(1.0f)):
                index_(index),
                pSkeleton_(pSkeleton),
                pParentBone_(parentBone),
                localBindTransform_(localBindTransform),
                localTransform_(localTransform)
        {
            calculateBranch();
        }

        /**
         * Конструктор перемещения
         * @param other R-value ссылка на другой объект
         * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
         */
        Bone(Bone&& other) noexcept : Bone()
        {
            std::swap(pSkeleton_,other.pSkeleton_);
            std::swap(index_,other.index_);
            std::swap(pParentBone_,other.pParentBone_);
            std::swap(localBindTransform_,other.localBindTransform_);
            std::swap(localTransform_,other.localTransform_);
            std::swap(totalTransform_,other.totalTransform_);
            std::swap(totalBindTransform_,other.totalBindTransform_);
            childrenBones_ = std::move(other.childrenBones_);
        }

        /**
         * Перемещение через присваивание
         * @param other R-value ссылка на другой объект
         * @return Ссылка на текущий объект
         */
        Bone& operator=(Bone&& other) noexcept
        {
            if (this == &other) return *this;

            index_ = 0;
            pSkeleton_ = nullptr;
            pParentBone_ = nullptr;
            childrenBones_.clear();

            std::swap(pSkeleton_,other.pSkeleton_);
            std::swap(index_,other.index_);
            std::swap(pParentBone_,other.pParentBone_);
            std::swap(localBindTransform_,other.localBindTransform_);
            std::swap(localTransform_,other.localTransform_);
            std::swap(totalTransform_,other.totalTransform_);
            std::swap(totalBindTransform_,other.totalBindTransform_);
            childrenBones_ = std::move(other.childrenBones_);

            return *this;
        }

        /**
         * Добавление дочерней кости
         * @param index Индекс
         * @param localBindTransform Изначальная трансформация
         * @param localTransform Задаваемая трансформация
         */
        Bone* addChildBone(size_t index,
                const math::Mat4<float>& localBindTransform = math::Mat4<float>(1.0f),
                const math::Mat4<float>& localTransform = math::Mat4<float>(1.0f))
        {
            this->childrenBones_.emplace_back(this->pSkeleton_,index,this,localBindTransform,localTransform);
            return &(this->childrenBones_.back());
        }

        /**
         * Задать трансформацию текущей кости (и всей дальнейшей ветви)
         * @param localTransform Локальная трансформация
         */
        void setTransformation(const math::Mat4<float>& localTransform)
        {
            this->localTransform_ = localTransform;
            this->calculateBranch();
        }

        /**
         * Задать локальную изначальную трансформацию кости (и всей дальнейшей вевти)
         * @param localBindTransform Смещение (расположение) относительно родитеской кости
         */
        void setBindTransformation(const math::Mat4<float>& localBindTransform)
        {
            this->localBindTransform_ = localBindTransform;
            this->calculateBranch();
        }

        /**
         * Задать изначальную локальную трансформацию и дополнитульную (анимированную) локальную трансформацию
         * @param localBindTransform Смещение (расположение) относительно родитеской кости
         * @param localTransform Локальная добавочная трансформация
         */
        void setTransformations(const math::Mat4<float>& localBindTransform, const math::Mat4<float>& localTransform)
        {
            this->localBindTransform_ = localBindTransform;
            this->localTransform_ = localTransform;
            this->calculateBranch();
        }

        /**
         * Получить указатель на массив дочерних костей
         * @return Указатель на массив ксотей
         */
        std::vector<Bone>* getChildrenBones()
        {
            return &childrenBones_;
        }
    };

private:
    /// Массив итоговых трансформаций
    std::vector<math::Mat4<float>> finalTransformations_;

    /// Массив трансформаций без инвертированной bind матрицы
    std::vector<math::Mat4<float>> finalTransformationFromBoneSpace_;

    /// Корневая кость
    Bone rootBone_;

public:
    /**
     * Конструктор по умолчанию
     */
    Skeleton() = default;

    /**
     * Основной конструктор
     * @param boneTotalCount Общее количество костей
     */
    explicit Skeleton(size_t boneTotalCount)
    {
        // Резервируем массив нужным количеством элементов
        finalTransformations_.resize(boneTotalCount);
        finalTransformationFromBoneSpace_.resize(boneTotalCount);

        // Создать корневую кость скелета
        rootBone_ = Bone(this,0, nullptr);
    }

    /**
     * Конструктор перемещения
     * @param other R-value ссылка на другой объект
     * @details Нельзя копировать объект, но можно обменяться с ним ресурсом
     */
    Skeleton(Skeleton&& other) noexcept : Skeleton()
    {
        finalTransformations_ = std::move(other.finalTransformations_);
        rootBone_ = std::move(other.rootBone_);
    }

    /**
     * Перемещение через присваивание
     * @param other R-value ссылка на другой объект
     * @return Ссылка на текущий объект
     */
    Skeleton& operator=(Skeleton&& other) noexcept
    {
        if (this == &other) return *this;

        //TODO: Желательно очистить данные перед обменом

        finalTransformations_ = std::move(other.finalTransformations_);
        rootBone_ = std::move(other.rootBone_);

        return *this;
    }

    /**
     * Получить корневую кость
     * @return Указатель на объект кости
     */
    Bone* getRootBone()
    {
        return &rootBone_;
    }

    /**
     * Получить массив итоговых трансформаций костей
     * @param fromBoneSpace Если точки заданы в пространстве кости
     * @return Массив матриц
     */
    const std::vector<math::Mat4<float>>& getFinalBoneTransforms(bool fromBoneSpace = false)
    {
        return fromBoneSpace ? finalTransformationFromBoneSpace_ : finalTransformations_;
    }
};