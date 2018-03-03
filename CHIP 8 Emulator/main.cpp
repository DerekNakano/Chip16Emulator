#include <iostream>
#include <string>
#include <d2d1.h>		//graphic libraries
#include <d2d1_1.h>
#include <Windows.h>
#include <time.h>
#include "chip8.h" 
#include "graphics.h"
#include <ctime>

chip8* mychip;
graphics* g;

void draw();			//draws on window

void getInput();		//determines what key is pressed

LRESULT CALLBACK WindowProc(_In_ HWND   hwnd, _In_ UINT   uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (uMsg == WM_DESTROY)// || uMsg == WM_CLOSE)		//when user closes window
	{
		PostQuitMessage(0);		//return code 0
		
		/*
		if(g)
			delete g;
		if(mychip)
			delete mychip;*/

		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);	//passes it on to default winow
}

//int main(int argc, char **argv)
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

	mychip->loadGame("brix");

	clock_t t;
	float dur;

	while (message.message != WM_QUIT)// && message.message != WM_CLOSE && message.message != WM_DESTROY)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))	//if there was a message then we want to send it to window proc
			DispatchMessage(&message);
		else if(mychip)
		{
			t = clock();

			//update
			mychip->eCycle();

			//render
			if (mychip->checkDrawFlag())	//if there is a new display
			{
				draw();
			}
			
			getInput();
			
			t = clock() - t;	//get clock duration of cycle

			dur = (float)t * 1000 / CLOCKS_PER_SEC;		//duration in milli

			if(16.67 > dur)
				Sleep(16.67 - (float)dur);
		}
	}

	/*
	if(mychip)
		delete mychip;
	if(g)
		delete g;*/

	delete mychip;
	delete g;

	return 0;
}

void draw()
{
	int x = -1;
	int y = 0;

	g->BeginDraw();
	//g->ClearScreen(0.0f, 0.0f, 0.5f);
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

		for (int i = 0; i < 16; i++)	//clear keys
			mychip->key[i] = 0;

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