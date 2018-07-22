#include "VertexBuffer.h"
#include <sstream>

/**
* \brief Загрузка данныз из .obj файла модели
* \param path Путь к файлу
*/
void VertexBuffer::LoadFromFile(std::string path)
{
	std::ifstream in;
	in.open(path, std::ifstream::in);
	if (in.fail()) return;

	std::string line;

	while (!in.eof()) {
		// Получить строку
		std::getline(in, line);

		// Получить строковй поток
		std::istringstream iss(line);

		// Переменная для символьного мусора
		char trash;

		// Если строчка начинается с "v " (нет разницы первых 2 символов строки и "v ")
		if (!line.compare(0, 2, "v ")) {
			// Вписать символ первое значение строки (это символ "v") в строковую "мусорку"
			iss >> trash;
			// Далее идут координаты вершин (float), вписать их по очереди
			Vector3D v;
			iss >> v.X;
			iss >> v.Y;
			iss >> v.Z;
			// Добавить вершину
			this->vertices_.push_back(v);
		}
		// Если строка начинается с "f "
		else if (!line.compare(0, 2, "f ")) {
			// Массив индексов полигона
			std::vector<int> faceIndices;
			// Переменные для мусора (целочисленного) и для искомого индекса
			int itrash, idx;
			// Вписать символ первое значение строки (это символ "f") в строковую "мусорку"
			iss >> trash;
			// Строка содержит значения "24/1/24 25/2/25 26/3/26", индексами вершин являются только первые
			// числа в группе из 3 числел разделенных слешем. Поэтому вначале считываем в idx значение индекса,
			// а затем остальное вписываем в "мусорки". Так проделываем до оконачния строки
			while (iss >> idx >> trash >> itrash >> trash >> itrash) {
				idx--; // Отнимаем 1, поскольку в файле индексы идут с 1 а в массиве vertices_ нумерация с ноля
				faceIndices.push_back(idx);
			}
			// Добавляем полигон
			faces_.push_back(faceIndices);
		}
	}
}

/**
* \brief Получить вершины
* \return Ссылка на массив вершин
*/
const std::vector<Vector3D>& VertexBuffer::GetVertices() const
{
	return this->vertices_;
}

/**
* \brief Получить полигоны (индексы соответствующих вершин)
* \return Ссылка на массив полигонов
*/
const std::vector<std::vector<int>>& VertexBuffer::GetFaces() const
{
	return this->faces_;
}

/**
* \brief Получить размер буфера
* \return Кол-во  байт
*/
unsigned VertexBuffer::GetSize() const
{
	return (sizeof(Vector3D) * this->vertices_.size()) + (sizeof(int)*3*this->faces_.size());
}

/**
* \brief Конструктор загружающий данные из .obj файла модели
* \param path Путь к файлу
*/
VertexBuffer::VertexBuffer(std::string path)
{
	this->LoadFromFile(path);
}
