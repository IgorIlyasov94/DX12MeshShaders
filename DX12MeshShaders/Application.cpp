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
	return 0;
}
