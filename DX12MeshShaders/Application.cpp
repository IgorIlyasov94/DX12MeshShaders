#include "Application.h"

Common::Application::Application(HINSTANCE instance, int cmdShow)
	: _instance(instance), _cmdShow(cmdShow)
{
	
}

Common::Application::~Application()
{

}

int Common::Application::Run()
{
	try
	{
		Initialize(_instance, _cmdShow);

		MSG message{};

		while (message.message != WM_QUIT)
		{
			PeekMessage(&message, nullptr, 0, 0, PM_REMOVE);

			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		return static_cast<int>(message.wParam);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);

		renderer.reset();
	}

	return 0;
}

void Common::Application::Initialize(HINSTANCE instance, int cmdShow)
{
	WNDCLASSEX windowClass{};

	auto windowClassName = L"Mesh Shaders";
	auto windowTitleName = L"Mesh Shaders";

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = instance;
	windowClass.hIcon = LoadIcon(instance, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = LoadIcon(windowClass.hInstance, IDI_APPLICATION);

	RegisterClassExW(&windowClass);

	int initialX = 100;
	int initialY = 100;
	int initialWidth = 640;
	int initialHeight = 480;

	HWND windowHandler = CreateWindowExW(WS_EX_APPWINDOW, windowClassName, windowTitleName, WS_OVERLAPPEDWINDOW,
		initialX, initialY, initialWidth, initialHeight, nullptr, nullptr, instance, nullptr);

	ShowWindow(windowHandler, SW_SHOWDEFAULT);

	SetForegroundWindow(windowHandler);
	SetFocus(windowHandler);

	ShowCursor(true);

	renderer = std::make_unique<Graphics::DirectX12Renderer>(windowHandler, initialWidth, initialHeight, isFullscreen);
	scene = std::make_unique<Scene>();

	isFullscreen = false;
}

LRESULT Common::Application::WindowProc(HWND windowHandler, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(windowHandler, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));

			return 0;
		}
		case WM_KEYDOWN:
		{
			auto keyData = static_cast<uint8_t>(wParam);

			return 0;
		}
		case WM_KEYUP:
		{
			auto keyData = static_cast<uint8_t>(wParam);

			return 0;
		}
		case WM_SYSKEYDOWN:
		{
			if ((wParam == VK_RETURN) && (lParam & (static_cast<int64_t>(1) << 29)))
			{
				if (renderer != nullptr)
				{
					isFullscreen = !isFullscreen;
					renderer->SwitchFullscreenMode(isFullscreen);

					return 0;
				}
			}

			break;
		}
		case WM_DISPLAYCHANGE:
		{
			return 0;
		}
		case WM_PAINT:
		{
			if (renderer != nullptr && scene != nullptr)
			{
				auto commandList = renderer->FrameStart();
				scene->OnFrameRender(commandList);
				renderer->FrameEnd();
			}

			return 0;
		}
		case WM_SIZE:
		{
			if (renderer != nullptr)
			{
				RECT windowSize;
				GetClientRect(windowHandler, &windowSize);

				renderer->OnResize(windowSize.right - windowSize.left, windowSize.bottom - windowSize.top);
			}

			return 0;
		}
		case WM_SETFOCUS:
		{
			if (renderer != nullptr)
				renderer->OnSetFocus();

			return 0;
		}
		case WM_KILLFOCUS:
		{
			if (renderer != nullptr)
				renderer->OnLostFocus();

			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);

			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);

			return 1;
		}
		default:
		{
			return DefWindowProc(windowHandler, message, wParam, lParam);
		}
	}

	return 0;
}

std::unique_ptr<Graphics::DirectX12Renderer> Common::Application::renderer = nullptr;
std::unique_ptr<Common::Scene> Common::Application::scene = nullptr;
bool Common::Application::isFullscreen = false;
