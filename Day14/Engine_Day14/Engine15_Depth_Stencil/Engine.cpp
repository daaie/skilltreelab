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

// 회전 각도 저장용.
float rot = 0.01f;
float pi = 3.141592f;
float doublePI = pi * 2.0f;
void Engine::Update(double deltaTime)
{
	// 물체 회전시키기.
	RotateObject(deltaTime);
}

void Engine::RotateObject(double deltaTime)
{
	// 회전 각도 변경 (프레임 당 0.003도의 속도로 회전).
	//rot += XMConvertToRadians(0.003f);

	// 초당 30도의 회전 속도로 회전.
	rot += XMConvertToRadians(30.0f) * deltaTime;
	if (rot >= doublePI)
		rot = 0.0f;

	// 회전 행렬.
	XMMATRIX rotationX = XMMatrixRotationX(XMConvertToRadians(-90.0f));
	XMMATRIX rotationY = XMMatrixRotationY(rot);
	// 이동 행렬.
	//XMMATRIX translation = XMMatrixTranslation(3.0f, 1.0f, 0.0f);
	// 스케일 행렬.
	//XMMATRIX scale = XMMatrixScaling(1.5f, 1.5f, 1.0f);

	// 월드 행렬 갱신.
	world = XMMatrixIdentity();
	//world = translation * rotation;
	//world = scale * rotation * translation;
	world *= rotationX * rotationY;

	// 셰이더 상수버퍼에 전달할 데이터 설정.
	CBPerObject cbData;
	cbData.world = XMMatrixTranspose(world);
	cbData.view = XMMatrixTranspose(view);
	cbData.projection = XMMatrixTranspose(projection);

	//// 상수 버퍼 갱신.
	pDeviceContext->UpdateSubresource(cbBuffer, 0, 0, &cbData, 0, 0);

	// 파이프라인 바인딩.
	//pDeviceContext->VSSetConstantBuffers(0, 1, &cbBuffer);
}

void Engine::Render()
{
	// 배경 지우기. (백버퍼 지우기).
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// 뎁스/스텐실 지우기.
	pDeviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 정점 그리기.
	pDeviceContext->DrawIndexed(nIndices, 0, 0);
	//pDeviceContext->Draw(nVertices, 0);

	// 버퍼 교체. (백버퍼 <-> 프론트버퍼).
	pSwapChain->Present(0, 0);
}