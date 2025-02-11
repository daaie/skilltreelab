#include "Engine.h"
#include <d3d11.h>

Engine::Engine(HINSTANCE hinstance) 
	: DXApp(hinstance)
{

}

Engine::~Engine()
{
	
}

bool Engine::Init()
{
	if (DXApp::Init() == false)
		return false;

	return true;
}

void Engine::Update()
{

}

void Engine::Render()
{
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	pDeviceContext->Draw(3, 0);

	pSwapChain->Present(0, 0);
}
