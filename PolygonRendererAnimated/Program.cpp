#include <iostream>
#include <Windows.h>
#include <chrono>
#include <iomanip>

#include "../Base/Gfx.h"
#include "../Base/TextureBuffer.h"
#include "../Base/VertexBuffer.h"

/**
* \brief ������� ��������� (����������)
* \param hWnd ����� ����
* \param message ������������� ���������
* \param wParam �������� ���������
* \param lParam �������� ���������
* \return ��� ����������
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/**
* \brief ��������� ������������� ������
* \param image ����� �����������
* \param model ����� ������
* \param rotAngle ���� ��������
*/
void DrawPolygonModel(gfx::TextureBuffer* image, gfx::VertexBuffer* model, float rotAngle = 0.0f);

/**
* \brief ��������� ������������� ������ c ������ �������
* \param image ����� �����������
* \param zBuffer ����� �������
* \param model ����� ������
* \param rotAngle ���� ��������
*/
void DrawPolygonModelZBuffered(gfx::TextureBuffer* image, gfx::ZBuffer* zBuffer, gfx::VertexBuffer* model, float rotAngle = 0.0f);

/**
* \brief ������������ ���� (�������� � ��������� �� 0 �� 360)
* \param angle ����
* \return ��������������� ����
*/
float NormalizeAngle(float angle);

// ����� �����
gfx::TextureBuffer frameBuffer;

// ����� �������
gfx::ZBuffer zBuffer;

// ������ ������
gfx::VertexBuffer vertexBuffer;

// ����������� ��������� ����� (������)
gfx::Vector3D<float> lightDirection(0.0f, 0.0f, -1.0f);

// ����� ���������� �����
std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;

/**
* \brief ����� �����
* \param argc ���-�� ����������
* \param argv ��������� (������)
* \return ��� ������
*/
int main(int argc, char* argv[])
{
	try
	{
		// ��������� ������ ������������ ������
		HINSTANCE hInstance = GetModuleHandle(nullptr);

		// ����������� ������ ����
		WNDCLASSEX classInfo;
		classInfo.cbSize = sizeof(WNDCLASSEX);
		classInfo.style = CS_HREDRAW | CS_VREDRAW;
		classInfo.cbClsExtra = 0;
		classInfo.cbWndExtra = 0;
		classInfo.hInstance = hInstance;
		classInfo.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		classInfo.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
		classInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
		classInfo.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
		classInfo.lpszMenuName = nullptr;
		classInfo.lpszClassName = L"WireRendererWindow";
		classInfo.lpfnWndProc = WndProc;

		// ���� �� ������� ���������������� �����
		if (!RegisterClassEx(&classInfo)) {
			throw std::runtime_error("ERROR: Can't register window class.");
		}

		// �������� ����
		HWND mainWindow = CreateWindow(
			classInfo.lpszClassName,
			L"WireRendererAnimated",
			WS_OVERLAPPEDWINDOW,
			0, 0,
			800, 600,
			NULL,
			NULL,
			hInstance,
			NULL);

		// ���� �� ������� ������� ����
		if (!mainWindow) {
			throw std::runtime_error("ERROR: Can't create main application window.");
		}

		// �������� ����
		ShowWindow(mainWindow, SW_SHOWNORMAL);

		// �������� ������� ���������� ������� ����
		RECT clientRect;
		GetClientRect(mainWindow, &clientRect);
		std::cout << "INFO: Client area sizes : " << clientRect.right << "x" << clientRect.bottom << std::endl;

		// ������� ����� ����� �� �������� ���������� �������
		frameBuffer = gfx::TextureBuffer(clientRect.right, clientRect.bottom);
		std::cout << "INFO: Frame-buffer initialized  (size : " << frameBuffer.GetSize() << " bytes)" << std::endl;

		// ������� ����� ������� �� �������� ���������� ������� (���������� �������� ���������)
		zBuffer = gfx::ZBuffer(clientRect.right, clientRect.bottom, 1.0f);
		std::cout << "INFO: Z-buffer initialized (size : " << zBuffer.GetSize() << " bytes)" << std::endl;

		// ��������� ������� � ����� ������ �� �����
		vertexBuffer.LoadFromFile("models/african_head.obj");
		std::cout << "INFO: Vertex-buffer initialized (size: " << vertexBuffer.GetSize() << " bytes, " << vertexBuffer.GetVertices().size() << " vertices, " << vertexBuffer.GetFaces().size() << " faces)" << std::endl;

		// ���� ��������
		float angle = 0.0f;
		float angleSpeed = 0.05f;

		//DrawPolygonModelZBuffered(&frameBuffer, &zBuffer, &vertexBuffer, angle);

		// ������� ��������� (������ ���������)
		MSG msg = {};

		// ������ ���� (�������� ���� �� ������ ��������� WM_QUIT)
		while (true)
		{
			// ���� �������� ���������
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT) {
					break;
				}
			}

			// ���� ����� ���� �� ����
			if (mainWindow)
			{
				// ����� �������� ����� (������� ��������)
				const std::chrono::time_point<std::chrono::high_resolution_clock> currentFrameTime = std::chrono::high_resolution_clock::now();

				// ������� ����������� ������ � ��������� ��������
				// 1 ������������ = 1000 ����������� = 1000000 ����������
				const int64_t delta = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - lastFrameTime).count();

				// ��������� � ������������
				float deltaMs = static_cast<float>(delta) / 1000.0f;

				// ���������� ����
				angle += deltaMs*angleSpeed;
				angle = NormalizeAngle(angle);

				//������������ ����
				frameBuffer.Clear({ 0,0,0 });
				zBuffer.Clear(1.0f);

				DrawPolygonModelZBuffered(&frameBuffer, &zBuffer, &vertexBuffer, angle);
				//DrawPolygonModel(&frameBuffer, &vertexBuffer, angle);

				// ��������� "������������", ����� �������� ����������� ����
				SendMessage(mainWindow, WM_PAINT, NULL, NULL);

				// �������� "����� ���������� �����"
				lastFrameTime = currentFrameTime;
			}

		}
	}
	catch (std::exception const &ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

/**
* \brief ������� ��������� (����������)
* \param hWnd ����� ����
* \param message ������������� ���������
* \param wParam �������� ���������
* \param lParam �������� ���������
* \return ��� ����������
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		gfx::PresentFrame(&frameBuffer, hWnd);
		return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/**
* \brief ��������� ������������� ������
* \param image ����� �����������
* \param model ����� ������
* \param rotAngle ���� ��������
*/
void DrawPolygonModel(gfx::TextureBuffer* image, gfx::VertexBuffer* model, float rotAngle)
{
	// ������ �� ���� ��������� ������ (������� - ������ �������� ������, ������ 3 �������)
	for (unsigned int i = 0; i < model->GetFaces().size(); i++)
	{
		// ��������� �������� �������� ��������
		std::vector<int> faceIndices = model->GetFaces()[i];

		// 2D ����� ������������ (� ����������� ������)
		std::vector<gfx::Vector2D<int>> points2D;
		// 3D ����� ������������ (� ����������� �����/����)
		std::vector<gfx::Vector3D<float>> points3D;

		// ������ �� ���� ������ �������� (���� ������� ��� �� 3)
		for (unsigned int j = 0; j < 3; j++)
		{
			// 3D ����� ������������
			gfx::Vector3D<float> point3D = model->GetVertices()[faceIndices[j]];
			// ��������
			point3D.RotateAroundY(rotAngle);

			// ��������������� �� ����������� ����������� 2D �����
			gfx::Vector2D<int> point2D = {
				static_cast<int>((point3D.x + 1.0f)*(image->GetWidth() / 2.0f)),
				static_cast<int>((point3D.y*-1 + 1.0f)*(image->GetHeight() / 2.0f))
			};

			// �������� � ������ �����
			points2D.push_back(point2D);
			points3D.push_back(point3D);
		}

		// ������� � �������� (����������� ��� ��������� ������������ ���� ����� ������������)
		gfx::Vector3D<float> normal = (points3D[2] - points3D[0]) ^ (points3D[1] - points3D[0]);
		// �������� � ��������� �����
		normal.Normalize(1.0f);
		// ��������� ������������� ��������� (��������� �� �������� �� ������� ��� ���� ������ �� �����������)
		// ������������ ��������� ������������ ������� � ������� ����������� ���������
		float intensity = normal*lightDirection;

		// ���� ������������� ����� ���� (����������� �������� �� ������� �� ������ ����) � ���� 3 �����, ���������� ����������� ����������� (�������)
		if (intensity > 0 && points2D.size() > 2) {

			// �������� ������� ������ �������� ������������� ������������ ������� ��������
			gfx::ColorBGR color = {
				static_cast<unsigned char>(255 * intensity),
				static_cast<unsigned char>(255 * intensity),
				static_cast<unsigned char>(255 * intensity),
				0 };


			// ���������� �������
			gfx::SetPolygon(image, points2D[0], points2D[1], points2D[2], color);
		}
	}
}

/**
* \brief ��������� ������������� ������ c ������ �������
* \param image ����� �����������
* \param zBuffer ����� �������
* \param model ����� ������
* \param rotAngle ���� ��������
*/
void DrawPolygonModelZBuffered(gfx::TextureBuffer* image, gfx::ZBuffer* zBuffer, gfx::VertexBuffer* model, float rotAngle)
{
	// ������ �� ���� ��������� ������ (������� - ������ �������� ������, ������ 3 �������)
	for (unsigned int i = 0; i < model->GetFaces().size(); i++)
	{
		// ��������� �������� �������� ��������
		std::vector<int> faceIndices = model->GetFaces()[i];

		// 3D ����� ������������ (� ����������� �����/����)
		std::vector<gfx::Vector3D<float>> points3D;

		// ������ �� ���� ������ �������� (���� ������� ��� �� 3)
		for (unsigned int j = 0; j < 3; j++)
		{
			// 3D ����� ������������
			gfx::Vector3D<float> point3D = model->GetVertices()[faceIndices[j]];
			// ������� ������ ��� Y
			point3D.RotateAroundY(rotAngle);
			// �������� ��������������� ������� � ������
			points3D.push_back(point3D);
		}

		// ������� � �������� (����������� ��� ��������� ������������ ���� ����� ������������)
		gfx::Vector3D<float> normal = (points3D[2] - points3D[0]) ^ (points3D[1] - points3D[0]);
		// �������� ��������� �����
		normal.Normalize(1.0f);
		// ��������� ������������� ��������� (��������� �� �������� �� ������� ��� ���� ������ �� �����������)
		// ������������ ��������� ������������ ������� � ������� ����������� ���������
		float intensity = normal*lightDirection;

		// ���� ������������� ����� ���� (����������� �������� �� ������� �� ������ ����) � ���� 3 �����, ���������� ����������� ����������� (�������)
		if (intensity > 0) {

			// �������� ������� ������ �������� ������������� ������������ ������� ��������
			gfx::Color4f color = { intensity,intensity,intensity};

			// ������������� ������� �������� ��� �������
			gfx::RaterizePolygon(image, zBuffer, 2.0, -10.0f, points3D[0], points3D[1], points3D[2], color, color, color);
		}
	}
}

/**
* \brief ������������ ���� (�������� � ��������� �� 0 �� 360)
* \param angle ����
* \return ��������������� ����
*/
float NormalizeAngle(float angle)
{
	// �������� ������� �� ������� ������� �� ���������������� ���� (����. 45481.0f) �� 360
	angle = static_cast<float>(fmod(angle, 360));
	// � ������ �������������� ������� - ��������� 360
	if (angle < 0) angle += 360;
	// ������� ��������������� ����
	return angle;
}