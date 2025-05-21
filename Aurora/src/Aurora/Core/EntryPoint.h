#pragma once

#ifdef AR_PLATFORM_WINDOWS
extern Aurora::Application* Aurora::CreateApplication();

int main(int argc,char** argv)
{
	Aurora::Log::Init();
	AR_CORE_WARN("Initialized Log£¡");
	AR_INFO("Hello£¡");


	auto app = Aurora::CreateApplication();
	app->Run();
	delete app;
}
#endif