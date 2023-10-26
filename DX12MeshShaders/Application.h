#pragma once

#include "Includes.h"
#include "Graphics/DirectX12Renderer.h"

namespace Common
{
	class Application final
	{
	public:
		Application(HINSTANCE instance, int cmdShow);
		~Application();

		int Run();

	private:
		Application() = delete;
		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		static LRESULT CALLBACK WindowProc(HWND windowHandler, UINT message, WPARAM wParam, LPARAM lParam);

		HINSTANCE _instance;
		int _cmdShow;

		static std::unique_ptr<Graphics::DirectX12Renderer> renderer;

		static bool isFullscreen;
	};
}
