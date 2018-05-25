#pragma once
#include <Windows.h>
#include "DXUtil.h"
#include <xnamath.h>

class DXApp
{
public:

	// 정점 구조체 정의.
	struct Vertex
	{
		/*float x, y, z;*/
		XMFLOAT3 position;

		Vertex() { }
		Vertex(float x, float y, float z) : position(x, y, z) { }
	};

	DXApp(HINSTANCE hinstance);
	virtual ~DXApp();

	// 메인 메시지 루프.
	int Run();

	// 프레임워크 메소드.
	virtual bool Init();
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	// Win32 멤버 변수.
	HWND		hwnd;
	HINSTANCE	hinstance;
	UINT		clientWidth;
	UINT		clientHeight;
	LPCWSTR		appTitle;
	DWORD		wndStyle;

	// DirectX 11 멤버 변수.
	ID3D11Device*		 pDevice;			// 렌더링 외의 다른 업무 전담.
	ID3D11DeviceContext* pDeviceContext;		// 렌더링 전담.
	IDXGISwapChain*		 pSwapChain;			// 스왑 체인.
	ID3D11RenderTargetView* pRenderTargetView;	// 렌더 타겟.

	ID3D11Buffer*		vertexBuffer;			// 정점 버퍼.
	ID3D11VertexShader* vertexShader;			// 정점 쉐이더(셰이더).
	ID3D11PixelShader*  pixelShader;			// 픽셀 쉐이더.
	ID3DBlob*			vertexShaderBuffer;		// 정점 쉐이더 버퍼.
	ID3DBlob*			pixelShaderBuffer;		// 픽셀 쉐이더 버퍼.
	ID3D11InputLayout*	vertexInputLayout;		// 정점 입력 레이아웃.


protected:

	// Win32 메소드. 
	// 윈도우 초기화.
	bool InitWindow();

	// DirectX 초기화.
	bool InitDirect3D();

	// 장면 초기화.
	bool InitScene();
};