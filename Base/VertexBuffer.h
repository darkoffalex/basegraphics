#pragma once

#include <vector>
#include <string>

#include "GfxTypes.h"

namespace gfx
{
	class VertexBuffer
	{
	private:
		std::vector<Vector3D<float>> vertices_;   // Массив вершин (массив структкр Vector3D)
		std::vector<std::vector<int>> faces_;     // Массив полиногов/треугольников (массив масивов индексов вершин)

	public:
		/**
		 * \brief Загрузка данных о вершинах из .obj файла модели
		 * \param path Путь к файлу
		 */
		void LoadFromFile(std::string path);

		/**
		 * \brief Получить вершины
		 * \return Ссылка на массив вершин
		 */
		const std::vector<Vector3D<float>>& GetVertices() const;

		/**
		 * \brief Получить полигоны (индексы соответствующих вершин)
		 * \return Ссылка на массив полигонов
		 */
		const std::vector<std::vector<int>>& GetFaces() const;

		/**
		 * \brief Получить размер буфера
		 * \return Кол-во  байт
		 */
		unsigned int GetSize() const;

		/**
		 * \brief Конструктор загружающий данные из .obj файла модели
		 * \param path Путь к файлу
		 */
		VertexBuffer(std::string path);

		/**
		 * \brief Конструктор по умолчанию
		 */
		VertexBuffer() = default;

		/**
		 * \brief Деструктор
		 */
		~VertexBuffer() = default;
	};

}

