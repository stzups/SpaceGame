#include "Engine.h"
#include <chrono>
#include <thread>
#include <condition_variable>

Engine::Engine()
	:
	window("SpaceGame"),
	renderThread(&Engine::RenderLoop, this),
	updateThread(&Engine::UpdateLoop, this)
{
	start.notify_all();
	
	MSG msg;
	bool result;
	while ((result = GetMessage(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
		{
			OutputDebugString("Error -1 in windows message");
			std::exit((int)msg.wParam);
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	running = false;
	updateThread.join();
	renderThread.join();
	std::exit(msg.wParam);
}

Engine::~Engine()
{
	running = false;
	updateThread.join();
	renderThread.join();
}

void Engine::UpdateLoop()
{
	{
		std::unique_lock<std::mutex> startLock(mutex);
		start.wait(startLock);
	}
	auto last = std::chrono::steady_clock::now();
	while (running)
	{
		auto now = std::chrono::steady_clock::now();
		double dt = std::chrono::duration<double, std::milli>(now - last).count();
		if (dt > tickTime)
		{
			OutputDebugString("TICKING: ");
			OutputDebugString(std::to_string(tick).c_str());
			last = now;
			{
				std::lock_guard<std::mutex> guard(mutex);
				tick++;
				//todo tick here
			}
		}
	}
}

void Engine::RenderLoop()
{
	{
		std::unique_lock<std::mutex> startLock(mutex);
		start.wait(startLock);
	}
	auto last = std::chrono::steady_clock::now();
	while (running)
	{
		auto now = std::chrono::steady_clock::now();
		double dt = std::chrono::duration<double, std::milli>(now - last).count();//delta time since last frame
		double time = tick + ((double)dt / tickTime);//interpolated tick
		if (1000 / dt <= maxFrameRate)
		{
			last = now;
			int t;
			{
				std::lock_guard<std::mutex> guard(mutex);
				frame++;//unused
				t = tick;
				//todo get all variables and stuff needed for draw
			}
			OutputDebugString("DRAWING: ");
			OutputDebugString(std::to_string(t).c_str());
			//OutputDebugString(", animate:");
			//OutputDebugString(std::to_string(time / 60).c_str());
			OutputDebugString(",");
			OutputDebugString(std::to_string(1000 / dt).c_str());
			OutputDebugString("\n");
			try {
				window.Graphics().ClearBuffer(0.0f, 1.0f, 0.0f);
				window.Graphics().drawTriangle(time / 60);//60 is a random constant i think
				window.Graphics().FlipBuffer();//this waits for vsync, for some reason flops between taking 18ms and 11ms to complete every other frame
			}
			catch (Graphics::InfoException & e)
			{
				window.SetTitle(e.what());
				OutputDebugString("Nonfatal Engine Error:");
				OutputDebugString(e.what());
			}
			catch (EngineException & e)
			{
				window.SetTitle(e.what());
				OutputDebugString("Fatal Engine Error: ");
				OutputDebugString(e.what());
				PostQuitMessage(-1);
			}
		}
	}
}
