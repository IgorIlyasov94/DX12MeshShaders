#pragma once

#include "Includes.h"

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

		HINSTANCE _instance;
		int _cmdShow;
	};
}
