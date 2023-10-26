#include "Application.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
	Common::Application application(instance, cmdShow);
	return application.Run();
}
