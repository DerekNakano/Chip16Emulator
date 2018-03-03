#pragma once
#include <Windows.h>
#include <d2d1.h>

class graphics
{
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* rendertarget;
	ID2D1SolidColorBrush* brush;


public:
	graphics();
	~graphics();

	bool Init(HWND windhowHandle);

	void BeginDraw() { rendertarget->BeginDraw(); }
	void EndDraw() { rendertarget->EndDraw(); }
	
	void ClearScreen(float r, float g, float b);
	//void DrawCircle(float x, float y, float radius, float r, float g, float b, float a);
	void DrawPixel(float x, float y, float r, float g, float b, float a);
};