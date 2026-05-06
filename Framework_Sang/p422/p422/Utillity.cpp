#include "INC_Windows.h"
#include "Utillity.h"
#include <iostream>

namespace learning
{
	DebugConsole::DebugConsole()
	{
		AllocConsole();

		FILE* stream;
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONIN$", "r", stdin);
	}

	DebugConsole::~DebugConsole()
	{
		fclose(stdout);
		fclose(stdin);

		FreeConsole();
	}

	// 콘솔 창을 보여줍니다.
	void DebugConsole::Show()
	{
		::ShowWindow(GetConsoleWindow(), SW_SHOW);
	}

	// 콘솔 창을 숨깁니다.
	void DebugConsole::Hide()
	{
		::ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	// 콘솔 창의 제목을 설정합니다.
	void DebugConsole::SetTitle(const wchar_t* title)
	{
		::SetConsoleTitle(title);
	}


	void DebugConsole::ToggleShow()
	{
		HWND hwnd = GetConsoleWindow();
		if (hwnd)
		{
			if (::IsWindowVisible(hwnd))
			{
				Hide();
			}
			else
			{
				Show();
			}
		}
	}

	DebugConsole g_debugConsole;

	int g_width = 800;
	int g_height = 600;

	void SetScreenSize(int width, int height)
	{
		g_width = width;
		g_height = height;
	}

	void GetScreenSize(int& width, int& height)
	{
		width = g_width;
		height = g_height;
	}
}