#include "graphics.h"

graphics::graphics() 
{
	factory = NULL;
	rendertarget = NULL;
	brush = NULL;
}

graphics::~graphics()
{
	if (factory)
		factory->Release();
	if(rendertarget)
		rendertarget->Release();
	if (brush)
		brush->Release();
}

bool graphics::Init(HWND windowHandle)
{
	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);

	if (res != S_OK)	//been a problem with factory
		return false;

	RECT rect;
	GetClientRect(windowHandle, &rect);

	res = factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(windowHandle, D2D1::SizeU(rect.right, rect.bottom)),
			&rendertarget);

	if (res != S_OK)	//been a problem with rendertarget?
		return false;

	res = rendertarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);

	if (res != S_OK)	//been a problem with brush
		return false;

	return true;
}

void graphics::ClearScreen(float r, float g, float b)
{
	rendertarget->Clear(D2D1::ColorF(r, g, b));
}

void graphics::DrawPixel(float x, float y, float r, float g, float b, float a)
{
	brush->SetColor(D2D1::ColorF(r, g, b, a));

	rendertarget->DrawRectangle(D2D1::Rect(x,y,x+2.0f,y+2.0f), brush, 3.0f);
}