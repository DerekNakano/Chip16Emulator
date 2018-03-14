#include <iostream>
#include <string>
#include <d2d1.h>		//graphic libraries
#include <d2d1_1.h>
#include <Windows.h>
#include <time.h>
#include "chip8.h" 
#include "graphics.h"
#include <ctime>
#include <chrono>

chip8* mychip;
graphics* g;
bool gameChosen = false;

void draw();			//draws on window

void getInput();		//determines what key is pressed

string games();			//displays the game choices

LRESULT CALLBACK WindowProc(_In_ HWND   hwnd, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (uMsg == WM_DESTROY)		//when user closes window
	{
		PostQuitMessage(0);		//return code 0

		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);	//passes it on to default winow
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmd, int nCmdShow)
{
	WNDCLASSEX windowclass;
	ZeroMemory(&windowclass, sizeof(WNDCLASSEX));

	windowclass.cbSize = sizeof(WNDCLASSEX);
	windowclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowclass.hInstance = hInstance;
	windowclass.lpfnWndProc = WindowProc;
	windowclass.lpszClassName = "MainWindow";
	windowclass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&windowclass);

	RECT rect = { 0, 0, 649, 329 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND windowHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Test", WS_OVERLAPPEDWINDOW, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, 0);

	if (!windowHandle)		//fails to create window handle
		return -1;

	g = new graphics();
	if (!g->Init(windowHandle))
	{
		delete g;
		return -1;
	}
	
	ShowWindow(windowHandle, nCmdShow);

	MSG message;		//start of updating display
	message.message = WM_NULL;

	mychip = new chip8();

	while (message.message != WM_QUIT)// && message.message != WM_CLOSE && message.message != WM_DESTROY)
	{

		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))	//if there was a message then we want to send it to window proc
			DispatchMessage(&message);
		else if (!gameChosen)				//runs intro screen of game choices
		{
			string game = games();

			if (game.compare("") != 0)		//chose game
				mychip->loadGame(game);

		}
		else if (mychip)					//runs chip emulator
		{

			//update
			mychip->eCycle();

			//render
			if (mychip->checkDrawFlag())	//if there is a new display
			{
				draw();
			}
		}

		getInput();

		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		float dur = std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();

		if (16.67 > dur)
			Sleep(16.67 - dur);
	}

	delete mychip;
	delete g;

	return 0;
}

string games()
{
	unsigned char num[15] =
	{
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	};

	unsigned char word[15] =
	{
		0x70, 0x20, 0x20, 0x20, 0x20, // T
		0xE0, 0x90, 0xE0, 0x80, 0x80, // P
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	};

	int x, y = 0;

	g->BeginDraw();
	
	for (int i = 0; i < 15; i++)
	{
		if (i % 5 == 0)
			y++;

		for (int j = 0; j < 8; j++)
		{
			if ((num[i] & (0x80 >> j)) != 0)	//bit is 1
			{
				x = 28 + j;
				g->DrawPixel((float)x*10.0f, (float)y*10.0f, 1.0f, 1.0f, 1.0f, 1.0f);
			}
		}

		for (int j = 0; j < 8; j++)
		{
			if ((word[i] & (0x80 >> j)) != 0)	//bit is 1
			{
				x = 36 + j;
				g->DrawPixel((float)x*10.0f, (float)y*10.0f, 1.0f, 1.0f, 1.0f, 1.0f);
			}
		}

		y++;
	}

	g->EndDraw();

	if (GetKeyState('1') & 0x8000)		//1 is pressed
	{
		gameChosen = true;
		return "tetris";
	}
	else if (GetKeyState('2') & 0x8000)		//2 is pressed
	{
		gameChosen = true;
		return "pong";
	}
	else if (GetKeyState('3') & 0x8000)
	{
		gameChosen = true;
		return "brix";
	}
	
	return "";
}

void draw()
{
	int x = -1;
	int y = 0;

	g->BeginDraw();

	for (int i = 0; i < 2048; i++)
	{
		x++;
		if (x >= 64)
		{
			x = 0;
			y++;
		}

		if (mychip->display[i] > 0)
			g->DrawPixel((float)x*10.0f, (float)y*10.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		else
			g->DrawPixel((float)x*10.0f, (float)y*10.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	}
	g->EndDraw();
}

void getInput()
{

		memset(mychip->key, 0, 16 * sizeof(char)); //clear keys

		if (GetKeyState('1') & 0x8000)		//1 is pressed
		{
			mychip->key[1] = 1;
		}
		else if (GetKeyState('2') & 0x8000)		//2 is pressed
		{
			mychip->key[2] = 1;
		}
		else if (GetKeyState('3') & 0x8000)
		{
			mychip->key[3] = 1;
			bool gameChosen = false;
		}
		else if (GetKeyState('4') & 0x8000)
		{
			mychip->key[12] = 1;
		}
		else if (GetKeyState('Q') & 0x8000)
		{
			mychip->key[4] = 1;
		}
		else if (GetKeyState('W') & 0x8000)
		{
			mychip->key[5] = 1;
		}
		else if (GetKeyState('E') & 0x8000)
		{
			mychip->key[6] = 1;
		}
		else if (GetKeyState('R') & 0x8000)
		{
			mychip->key[13] = 1;
		}
		else if (GetKeyState('A') & 0x8000)
		{
			mychip->key[7] = 1;
		}
		else if (GetKeyState('S') & 0x8000)
		{
			mychip->key[8] = 1;
		}
		else if (GetKeyState('D') & 0x8000)
		{
			mychip->key[9] = 1;
		}
		else if (GetKeyState('F') & 0x8000)
		{
			mychip->key[14] = 1;
		}
		else if (GetKeyState('Z') & 0x8000)
		{
			mychip->key[10] = 1;
		}
		else if (GetKeyState('X') & 0x8000)
		{
			mychip->key[0] = 1;
		}
		else if (GetKeyState('C') & 0x8000)
		{
			mychip->key[11] = 1;
		}
		else if (GetKeyState('V') & 0x8000)
		{
			mychip->key[15] = 1;
		}
}