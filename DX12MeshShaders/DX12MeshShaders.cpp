#include "Application.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
	Common::Application application(instance, cmdShow);
	return application.Run();
}
