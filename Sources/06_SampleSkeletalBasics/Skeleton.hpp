/**
 * Класс скелета. Используется для скелетной анимации одиночного меша
 * Copyright (C) 2020 by Alex "DarkWolf" Nem - https://github.com/darkoffalex
 */

#pragma once

#include <vector>
#include <functional>
#include <memory>

#include <Math.hpp>

/**
 * Класс скелета
 */
class Skeleton
{
public:

    /**
     * Класс кости скелета (внутренний класс скелета)
     */
    class Bone
    {
    public:
        /// Флаги вычисления матриц
        enum CalcFlags
        {
            eNone = (0),
            eFullTransform = (1u << 0u),
            eBindTransform = (1u << 1u),
            eInverseBindTransform = (1u << 2u),
        };

    private:
        /// Открыть доступ для дружественных классов
        friend class Skeleton;

        /// Указатель на скелет
        Skeleton* pSkeleton_;
        /// Индекс кости в линейном массиве
        size_t index_;

        /// Указатель на родительскую кость
        Skeleton::Bone* pParentBone_;
        /// Массив указателей на дочерние кости
        std::vector<std::shared_ptr<Skeleton::Bone>> childrenBones_;

        /// Смещение (расположение) относительно родительской кости (можно считать это initial-положением)
        math::Mat4<float> localBindTransform_;
        /// Локальная трансформация относительно bind (та трансформация, которая может назначаться во время анимации)
        math::Mat4<float> localTransform_;

        /// Результирующая трансформация кости с учетом локальной трансформации и результирующий трансформаций родительских костей
        /// Данная трансформация может быть применена к точкам находящимся В ПРОСТРАНСТВЕ КОСТИ
        math::Mat4<float> totalTransform_;
        /// Результирующая трансформация кости БЕЗ учета задаваемой, но с учетом bind-трансформаций родительских костей
        math::Mat4<float> totalBindTransform_;
        /// Инвертированная bind матрица может быть использована для перехода в пространство кости ИЗ ПРОСТРАНСТВА МОДЕЛИ
        math::Mat4<float> totalBindTransformInverse_;

        /**
         * Рекурсивное вычисление матриц для текущей кости и всех дочерних её костей
         * @param callUpdateCallbackFunction Вызывать функцию обратного вызова установленную к скелета
         * @param calcFlags Опции вычисления матриц (какие матрицы считать)
         */
        void calculateBranch(bool callUpdateCallbackFunction = true, unsigned calcFlags = CalcFlags::eFullTransform | CalcFlags::eBindTransform | CalcFlags::eInverseBindTransform)
        {
            // Если у кости есть родительская кость
            if(pParentBone_ != nullptr)
            {
                // Общая initial (bind) трансформация для кости учитывает текущую и родительскую (что в свою очередь справедливо и для родительской)
                if(calcFlags & CalcFlags::eBindTransform)
                    totalBindTransform_ = pParentBone_->totalBindTransform_ * this->localBindTransform_;

                // Общая полная (с учетом задаваемой) трансформация кости (смещаем на localTransform_, затем на initial, затем на общую родительскую трансформацию)
                if(calcFlags & CalcFlags::eFullTransform)
                    totalTransform_ = pParentBone_->totalTransform_ * this->localBindTransform_ * this->localTransform_;
            }
                // Если нет родительской кости - считать кость корневой
            else
            {
                if(calcFlags & CalcFlags::eBindTransform)
                    totalBindTransform_ = this->localBindTransform_;

                if(calcFlags & CalcFlags::eFullTransform)
                    totalTransform_ = this->localBindTransform_ * this->localTransform_;
            }

            // Инвертированная матрица bind трансформации
            if(calcFlags & CalcFlags::eInverseBindTransform)
                totalBindTransformInverse_ = math::Inverse(totalBindTransform_);

            // Если есть указатель на объект скелета и индекс корректный
            if(pSkeleton_ != nullptr && index_ < pSkeleton_->modelSpaceFinalTransforms_.size())
            {
                // Итоговая матрица трансформации для точек находящихся в пространстве модели
                // Поскольку общая трансформация кости работает с вершинами находящимися в пространстве модели,
                // они в начале должны быть переведены в пространство кости.
                pSkeleton_->modelSpaceFinalTransforms_[index_] = pSkeleton_->globalInverseTransform_ * totalTransform_ * totalBindTransformInverse_;

                // Для ситуаций, если вершины задаются сразу в пространстве кости
                pSkeleton_->boneSpaceFinalTransforms_[index_] = pSkeleton_->globalInverseTransform_ * totalTransform_;
            }

            // Рекурсивно выполнить для дочерних элементов (если они есть)
            if(!this->childrenBones_.empty()){
                for(auto& childBone : this->childrenBones_){
                    childBone->calculateBranch(false, calcFlags);
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
                totalBindTransform_(math::Mat4<float>(1.0f)),
                totalBindTransformInverse_(math::Mat4<float>(1.0f)){}

        /**
         * Основной конструктор кости
         * @param pSkeleton Указатель на объект скелета
         * @param index Индекс кости в линейном массиве трансформаций
         * @param parentBone Указатель на родительскую кость
         * @param localBindTransform Смещение (расположение) относительно родительской кости
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
                localTransform_(localTransform),
                totalTransform_(math::Mat4<float>(1.0f)),
                totalBindTransform_(math::Mat4<float>(1.0f)),
                totalBindTransformInverse_(math::Mat4<float>(1.0f))
        {
            // Вычисление матриц кости
            calculateBranch(CalcFlags::eFullTransform|CalcFlags::eBindTransform|CalcFlags::eInverseBindTransform);
        }

        /**
         * Деструктор по умолчанию
         */
        ~Bone() = default;

        /**
         * Добавление дочерней кости
         * @param index Индекс
         * @param localBindTransform Изначальная трансформация
         * @param localTransform Задаваемая трансформация
         * @return Указатель на добавленную кость
         */
        std::shared_ptr<Bone> addChildBone(size_t index,
                                           const math::Mat4<float>& localBindTransform,
                                           const math::Mat4<float>& localTransform = math::Mat4<float>(1.0f))
        {
            // Создать дочернюю кость
            std::shared_ptr<Bone> child(new Bone(this->pSkeleton_,index,this,localBindTransform,localTransform));
            // Добавить в массив дочерних костей
            this->childrenBones_.push_back(child);
            // Добавить в общий линейный массив по указанному индексу
            if(this->pSkeleton_ != nullptr) this->pSkeleton_->bones_[index] = child;
            // Вернуть указатель
            return child;
        }

        /**
         * Установить локальную (анимируемую) трансформацию
         * @param transform Матрица 4*4
         * @param recalculateBranch Пересчитать ветвь
         */
        void setLocalTransform(const math::Mat4<float>& transform, bool recalculateBranch = true)
        {
            this->localTransform_ = transform;
            if(recalculateBranch) this->calculateBranch(true, CalcFlags::eFullTransform);
        }

        /**
         * Установить изначальную (initial) трансформацию кости относительно родителя
         * @param transform Матрица 4*4
         * @param recalculateBranch Пересчитать ветвь
         */
        void setLocalBindTransform(const math::Mat4<float>& transform, bool recalculateBranch = true)
        {
            this->localBindTransform_ = transform;
            if(recalculateBranch) this->calculateBranch(true, CalcFlags::eBindTransform|CalcFlags::eInverseBindTransform);
        }

        /**
         * Установить изначальную (initial, bind) и добавочную (animated) трансформацию
         * @param localBind Матрица 4*4
         * @param local Матрица 4*4
         * @param recalculateBranch Пересчитать ветвь
         */
        void setTransformations(const math::Mat4<float>& localBind, const math::Mat4<float>& local, bool recalculateBranch = true)
        {
            this->localBindTransform_ = localBind;
            this->localTransform_ = local;
            if(recalculateBranch) this->calculateBranch(true, CalcFlags::eFullTransform|CalcFlags::eBindTransform|CalcFlags::eInverseBindTransform);
        }

        /**
         * Получить массив дочерних костей
         * @return Ссылка на массив указателей
         */
        std::vector<std::shared_ptr<Bone>>& getChildrenBones()
        {
            return this->childrenBones_;
        }

        /**
         * Получить указатель на родительскую кость
         * @return Указатель
         */
        Bone* getParentBone()
        {
            return this->pParentBone_;
        }

        /**
         * Получить индекс кости
         * @return Целое положительное число
         */
        size_t getIndex()
        {
            return index_;
        }
    };

    /**
     * Smart-pointer объекта скелетной кости
     */
    typedef std::shared_ptr<Bone> BonePtr;

    /**
     * Состояние анимации
     */
    enum struct AnimationState
    {
        eStopped,
        ePlaying
    };

private:
    /// Открыть доступ для класса Mesh
    friend class Mesh;

    /// Массив итоговых трансформаций для вершин в пространстве модели
    std::vector<math::Mat4<float>> modelSpaceFinalTransforms_;
    /// Массив итоговых трансформаций для вершин в пространстве костей
    std::vector<math::Mat4<float>> boneSpaceFinalTransforms_;
    /// Матрица глобальной инверсии (на случай если в программе для моделирования объекту задавалась глобальная трансформация)
    math::Mat4<float> globalInverseTransform_;

    /// Массив указателей на кости для доступа по индексам
    std::vector<BonePtr> bones_;
    /// Корневая кость
    BonePtr rootBone_;

public:
    /**
     * Конструктор по умолчанию
     * Изначально у скелета всегда есть одна кость
     */
    Skeleton():
            modelSpaceFinalTransforms_(1),
            boneSpaceFinalTransforms_(1),
            globalInverseTransform_(math::Mat4<float>(1.0f)),
            bones_(1)
    {
        // Создать корневую кость
        rootBone_ = std::make_shared<Bone>(this,0,nullptr,math::Mat4<float>(1),math::Mat4<float>(1));
        // Нулевая кость в линейном массиве указателей
        bones_[0] = rootBone_;
    }

    /**
     * Основной конструктор
     * @param boneTotalCount Общее количество костей
     * @param updateCallback Функция обратного вызова при пересчете матриц
     */
    explicit Skeleton(size_t boneTotalCount):
            globalInverseTransform_(math::Mat4<float>(1.0f))
    {
        // Изначально у скелета есть как минимум 1 кость
        modelSpaceFinalTransforms_.resize(std::max<size_t>(1,boneTotalCount));
        boneSpaceFinalTransforms_.resize(std::max<size_t>(1,boneTotalCount));
        bones_.resize(std::max<size_t>(1,boneTotalCount));

        // Создать корневую кость
        rootBone_ = std::make_shared<Bone>(this,0,nullptr,math::Mat4<float>(1),math::Mat4<float>(1));
        // Нулевая кость в линейном массиве указателей
        bones_[0] = rootBone_;
    }

    /**
     * Деструктор по умолчанию
     */
    ~Skeleton() = default;

    /**
     * Получить корневую кость
     * @return Указатель на объект кости
     */
    BonePtr getRootBone()
    {
        return rootBone_;
    }

    void setGlobalInverseTransform(const math::Mat4<float>& m)
    {
        this->globalInverseTransform_ = m;
        this->getRootBone()->calculateBranch(true,Bone::CalcFlags::eNone);
    }

    /**
     * Получить массив итоговых трансформаций костей
     * @param fromBoneSpace Если точки заданы в пространстве кости
     * @return Ссылка на массив матриц
     */
    const std::vector<math::Mat4<float>>& getFinalBoneTransforms(bool fromBoneSpace = false) const
    {
        return fromBoneSpace ? boneSpaceFinalTransforms_ : modelSpaceFinalTransforms_;
    }

    /**
     * Получить общее кол-во костей
     * @return Целое положительное число
     */
    size_t getBonesCount() const
    {
        return bones_.size();
    }

    /**
     * Получить размер массива трансформаций в байтах
     * @return Целое положительное число
     */
    size_t getTransformsDataSize() const
    {
        return sizeof(math::Mat4<float>) * this->modelSpaceFinalTransforms_.size();
    }

    /**
     * Получить линейный массив костей
     * @return Массив указателей на кости
     */
    const std::vector<BonePtr>& getBones()
    {
        return bones_;
    }

    /**
     * Получить указатель на кость по индексу
     * @param index Индекс
     * @return Smart-pointer кости
     */
    BonePtr getBoneByIndex(size_t index)
    {
        return bones_[index];
    }
};

/**
 * Smart-unique-pointer объекта скелета
 */
typedef std::unique_ptr<Skeleton> UniqueSkeleton;