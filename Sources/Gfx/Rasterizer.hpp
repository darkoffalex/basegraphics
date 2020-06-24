#pragma once

#include "ImageBuffer.hpp"

#include <functional>
#include <vector>

namespace gfx
{

    template <typename VERTEX, typename COLOR, typename DEPTH>
    class Rasterizer
    {
    public:
        enum class FrontFace
        {
            eClockWise,
            eCounterClockWise
        };

        struct Vec4
        {
            float x;
            float y;
            float z;
            float w;
        };

    private:
        /// Указатель на буфер цвета
        ImageBuffer<COLOR>* pColorBuffer_;
        /// Указатель на буфер глубины
        ImageBuffer<DEPTH>* pDepthBuffer_;
        /// Как описывается передняя грань
        FrontFace frontFace_;
        /// Отсечение задних граней
        bool backFaceCooling_;

        /// Функция - вершинный шейдер
        std::function<VERTEX(const VERTEX& vertex, Vec4* outPosition)> vertexShaderFn_;

        /// Функция - фрагментный шейдер
        std::function<COLOR(const VERTEX& interpolatedVertexInfo)> fragmentShaderFn_;
    public:
        /**
         * Конструктор по умолчанию
         */
        Rasterizer():
                pColorBuffer_(nullptr),
                pDepthBuffer_(nullptr),
                frontFace_(FrontFace::eClockWise),
                backFaceCooling_(true)
        {}

        /**
         * Основной конструктор
         * @param pColorBuffer Указатель на буфер цвета
         * @param pDepthBuffer Указатель на буфер глубины
         * @param frontFace Как описывается передняя грань
         * @param backFaceCooling Отсечение задних граней
         */
        Rasterizer(ImageBuffer<COLOR>* pColorBuffer,
                   ImageBuffer<DEPTH>* pDepthBuffer,
                   FrontFace frontFace = FrontFace::eClockWise,
                   bool backFaceCooling = true):
                pColorBuffer_(pColorBuffer),
                pDepthBuffer_(pDepthBuffer),
                frontFace_(frontFace),
                backFaceCooling_(backFaceCooling)
        {}

        void DrawTriangle(const VERTEX& v0, const VERTEX& v1, const VERTEX& v2)
        {
        }
    };
}

