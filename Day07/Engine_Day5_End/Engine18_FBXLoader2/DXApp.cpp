#include "DXApp.h"

// 메시지 프로시져 설정을 우회하는데 사용하기 위해 생성.
DXApp* g_pApp = NULL;

LRESULT CALLBACK MainMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// g_pApp 변수가 설정됐으면 MsgProc 사용.
	if (g_pApp) return g_pApp->MsgProc(hwnd, msg, wParam, lParam);

	// 설정되지 않으면 윈도우 기본 프로시저 함수 호출.
	else return DefWindowProc(hwnd, msg, wParam, lParam);
}

DXApp::DXApp(HINSTANCE hinstance)
{
	// 멤버 변수 초기화.
	hwnd = NULL;
	this->hinstance = hinstance;
	clientWidth = 1600;
	clientHeight = 1200;
	appTitle = L"Engine09 - Render State";
	wndStyle = WS_OVERLAPPEDWINDOW;
	g_pApp = this;

	// DX 관련 멤버 변수 초기화.
	pDevice = NULL;
	pDeviceContext = NULL;
	pSwapChain = NULL;
	pRenderTargetView = NULL;

	vertexBuffer = NULL;
	vertexShader = NULL;
	pixelShader = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;
	vertexInputLayout = NULL;

	indexBuffer = NULL;

	depthStencilView = NULL;
	depthStencilBuffer = NULL;
}

DXApp::~DXApp()
{
	// 풀스크린 해제.
	pSwapChain->SetFullscreenState(false, NULL);
	// NULL은 타겟, 사용하고있는 모니터. 변경도 가능.

	// 메모리 해제.
	Memory::SafeRelease(pDevice);
	Memory::SafeRelease(pDeviceContext);
	Memory::SafeRelease(pSwapChain);
	Memory::SafeRelease(pRenderTargetView);

	Memory::SafeRelease(vertexBuffer);
	Memory::SafeRelease(vertexShader);
	Memory::SafeRelease(pixelShader);
	Memory::SafeRelease(vertexShaderBuffer);
	Memory::SafeRelease(pixelShaderBuffer);
	Memory::SafeRelease(vertexInputLayout);

	Memory::SafeRelease(indexBuffer);

	Memory::SafeRelease(cbBuffer);

	Memory::SafeRelease(pTexture);
	Memory::SafeRelease(pTextureSamplerState);
	
	Memory::SafeRelease(rsState);

	// 블렌드 스테이트 해제.
	Memory::SafeRelease(alphaBlendState);

	Memory::SafeRelease(depthStencilView);
	Memory::SafeRelease(depthStencilBuffer);
}

int DXApp::Run()
{
	// 메인 메시지 처리.
	MSG msg;
	// 0으로 초기화.
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		// 메시지 처리.
		// GetMessage();
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			// 메시지 해석.
			TranslateMessage(&msg);
			// 메시지 전달.
			DispatchMessage(&msg);
		}

		// DirectX 렌더링 루프.
		else
		{
			// 타이머 실행.
			gameTimer.UpdateTime();

			float deltaTime = gameTimer.DeltaTime();
			
			// 입력 감지.
			DetectInput(deltaTime);

			// Update Scene.
			// 타이머에서 초당 프레임 시간 계산해서 설정.
			Update(deltaTime);

			// Render Scene.
			Render();
		}
	}

	return (int)msg.wParam;
}

bool DXApp::Init()
{
	// 윈도우 초기화.
	//if (!InitWindow())
	//if (false == InitWindow())
	if (InitWindow() == false)
		return false;

	// DirectX 초기화.
	if (InitDirect3D() == false)
		return false;

	// 장면 초기화.
	if (InitScene() == false)
		return false;

	if (InitDirectInput(hinstance) == false)
		return false;

	//pSwapChain->SetFullscreenState(true, NULL); // 윈도우에서 풀스크린모드로 변경

	return true;
}

bool DXApp::InitWindow()
{
	// 윈도우 클래스 값 생성/설정.
	WNDCLASSEX wc;
	// 구조체 초기화.
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = L"WindowClass";
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = MainMsgProc;

	// 윈도우 클래스 등록.
	if (RegisterClassEx(&wc) == false)
		return false;

	// 윈도우 생성.
	hwnd = CreateWindow(L"WindowClass", appTitle,
		wndStyle, 0, 0, clientWidth, clientHeight,
		NULL, NULL, hinstance, NULL);
	// 생성 실패하면 종료.
	if (hwnd == NULL)
	{
		return false;
	}

	// 창 띄우기.
	ShowWindow(hwnd, SW_SHOW);

	return true;
}

bool DXApp::InitDirect3D()
{
	// 스왑 체인 정보 설정.
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hwnd;
	swapDesc.Windowed = true;
	//swapDesc.Windowed = false; // 풀스크린 모드
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // 화면 크기 변경되면 갱신.
	swapDesc.BufferDesc.Width = clientWidth;
	swapDesc.BufferDesc.Height = clientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	// 장치 및 스왑체인 생성.
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &pSwapChain, 
		&pDevice, NULL, &pDeviceContext);

	// 오류 확인.
	//if (SUCCEEDED(hr)) // S_OK
	if (FAILED(hr))		 // E_FAIL
	{
		MessageBox(NULL, L"장치 생성 실패", L"오류", MB_OK);
		return false;
	}

	// 2D 텍스처 생성 (렌더 타겟 용).
	ID3D11Texture2D* pBackBufferTexture;
	hr = pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D),
		(void**)&pBackBufferTexture);

	if (FAILED(hr))		 // E_FAIL
	{
		MessageBox(NULL, L"스왑 버퍼 생성 실패", L"오류", MB_OK);
		return false;
	}

	// 렌더 타켓 뷰 생성.
	hr = pDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&pRenderTargetView);
	
	if (FAILED(hr))		 // E_FAIL
	{
		MessageBox(NULL, L"렌더 타겟 뷰 생성 실패", L"오류", MB_OK);
		return false;
	}

	if (!InitDepthStencilBuffer())
	{
		return false;
	}

	// 렌더 타켓 뷰 바인딩. 파이프 라인에 연결.
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, depthStencilView);

	// 리소스 해제.
	Memory::SafeRelease(pBackBufferTexture);

	return true;
}

bool DXApp::ShaderCompile()
{
	// 쉐이더 컴파일.
	HRESULT hr;
	// 정점 쉐이더 컴파일.
	hr = D3DX11CompileFromFile(L"VertexShader.fx", NULL, NULL, "main", "vs_5_0", NULL, NULL, NULL, &vertexShaderBuffer, NULL, NULL);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"정점 쉐이더 컴파일 실패", L"오류", MB_OK);
		return false;
	}

	// 정점 쉐이더 오브젝트 생성.
	hr = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"정점 쉐이더 오브젝트 생성 실패", L"오류", MB_OK);
		return false;
	}

	// 정점 쉐이더 설정.(바인딩).
	pDeviceContext->VSSetShader(vertexShader, NULL, NULL);

	// 픽셀 쉐이더.
	hr = D3DX11CompileFromFile(L"PixelShader.fx", NULL, NULL, "main",
		"ps_5_0", NULL, NULL, NULL, &pixelShaderBuffer, NULL, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"픽셀 쉐이더 컴파일 실패", L"오류", MB_OK);
		return false;
	}

	// 픽셀 쉐이더 오브젝트 생성.
	hr = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"픽셀 쉐이더 오브젝트 생성 실패", L"오류", MB_OK);
		return false;
	}

	// 픽셀 쉐이더 바인딩.
	pDeviceContext->PSSetShader(pixelShader, NULL, NULL);

	return true;
}

void DXApp::InitVertexBuffer()
{
	// 정점 만들기.
	//Vertex vertices[] =
	//{
	//	// Front Face
	//	Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,-1.0f, -1.0f, -1.0f),
	//	Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f),
	//	Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f),
	//	Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),
	//	// Back Face
	//	Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f,-1.0f, -1.0f, 1.0f),
	//	Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f),
	//	Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f),
	//	Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f,-1.0f,  1.0f, 1.0f),
	//	// Top Face
	//	Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,-1.0f, 1.0f, -1.0f),
	//	Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f,-1.0f, 1.0f,  1.0f),
	//	Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f),
	//	Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f),
	//	// Bottom Face
	//	Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,-1.0f, -1.0f, -1.0f),
	//	Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
	//	Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f),
	//	Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,-1.0f, -1.0f,  1.0f),
	//	// Left Face
	//	Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,-1.0f, -1.0f,  1.0f),
	//	Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,-1.0f,  1.0f,  1.0f),
	//	Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f,-1.0f,  1.0f, -1.0f),
	//	Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,-1.0f, -1.0f, -1.0f),
	//	// Right Face
	//	Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
	//	Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f),
	//	Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f),
	//	Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f),
	//};

	//nVertices = ARRAYSIZE(vertices);
	
	// FBX 로드
	HRESULT hr = LoadFBX("SK_CharM_Cardboard.FBX", &pVertices);
	if (FAILED(hr))
		return;

	// 정점 개수 저장.
	nVertices = pVertices.size();

	// sizeof(vertices) / sizeof(vertices[0]);
	// 버텍스 버퍼 서술자(Descriptor).
	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.ByteWidth = sizeof(Vertex) * nVertices;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;

	// 서브 리소스 생성.
	D3D11_SUBRESOURCE_DATA vbData;
	ZeroMemory(&vbData, sizeof(D3D11_SUBRESOURCE_DATA));
	vbData.pSysMem = &pVertices[0];

	// 정점 버퍼 생성.
	pDevice->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// 정점 버퍼 바인딩.
	pDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
}

void DXApp::InitIndexBuffer()
{
	// 인덱스 배열 만들기.
	DWORD indices[] =
	{
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	// 인덱스 개수 저장.
	nIndices = ARRAYSIZE(indices);

	// 인덱스 버퍼 서술자(Descriptor).
	D3D11_BUFFER_DESC ibDesc;
	ZeroMemory(&ibDesc, sizeof(D3D11_BUFFER_DESC));
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.ByteWidth = sizeof(DWORD) * ARRAYSIZE(indices);
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;

	// 서브 리소스 생성.
	D3D11_SUBRESOURCE_DATA ibData;
	ZeroMemory(&ibData, sizeof(D3D11_SUBRESOURCE_DATA));
	ibData.pSysMem = indices;

	// 인덱스 버퍼 생성.
	pDevice->CreateBuffer(&ibDesc, &ibData, &indexBuffer);

	// 인덱스 버퍼 바인딩.
	pDeviceContext->IASetIndexBuffer(indexBuffer,
		DXGI_FORMAT_R32_UINT, 0);
}

bool DXApp::InitInputLayout()
{
	// 입력 레이아웃.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 입력 레이아웃 생성.
	HRESULT hr = pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &vertexInputLayout);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"입력 레이아웃 생성 실패", L"오류", MB_OK);
		return false;
	}

	// 입력 레이아웃 바인딩.
	pDeviceContext->IASetInputLayout(vertexInputLayout);
}

void DXApp::SetViewport()
{
	// 뷰포트 정보 설정.
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = clientWidth;
	viewport.Height = clientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// 뷰포트 설정.
	pDeviceContext->RSSetViewports(1, &viewport);
}

// 공간 변환 행렬 설정.
void DXApp::InitTransformMatrix()
{
	// World x View x Projection;
	// 월드 행렬. (위치/회전/스케일).
	world = XMMatrixIdentity();

	float rotX = XMConvertToRadians(-90.0f);
	float rotY = XMConvertToRadians(180.0f);

	XMMATRIX rotationX = XMMatrixRotationX(rotX);
	XMMATRIX rotationY = XMMatrixRotationY(rotY);
	world *= rotationX*rotationY;


	// 카메라 위치.
	cameraPos = XMVectorSet(0.0f, 0.0f, -300.0f, 0.0f);

	// 카메라가 바라보는 위치.
	cameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	// 카메라 위 방향.
	cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// 뷰 행렬.
	view = XMMatrixLookAtLH(cameraPos, cameraTarget, cameraUp);

	// 시야각.
	float fovY = 75.0f * 3.14 / 180.0f;

	// 투영 행렬.
	projection = XMMatrixPerspectiveFovLH(fovY, (float)clientWidth / clientHeight,
		1.0f, 1000.0f);

	// 상수 버퍼 데이터 설정 및 파이프라인 바인딩.
	// 전치행렬을 만들어서 데이터 채우기.
	// DX는 행우선 / HLSL은 열우선.
	CBPerObject data;
	data.world = XMMatrixTranspose(world);
	data.view = XMMatrixTranspose(view);
	data.projection = XMMatrixTranspose(projection);

	// 버퍼 디스크립터.
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = sizeof(CBPerObject);
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;

	// 서브 리소스 생성.
	D3D11_SUBRESOURCE_DATA cbData;
	ZeroMemory(&cbData, sizeof(D3D11_SUBRESOURCE_DATA));
	cbData.pSysMem = &data;

	// 상수 버퍼 생성.
	HRESULT hr = pDevice->CreateBuffer(&cbDesc, &cbData, &cbBuffer);

	// 파이프라인에 바인딩.
	pDeviceContext->VSSetConstantBuffers(0, 1, &cbBuffer);
}

bool DXApp::InitScene()
{
	// 쉐이더 컴파일.
	if (!ShaderCompile())
		return false;

	// 정점 버퍼 설정.
	InitVertexBuffer();
	
	// 인덱스 버퍼 설정.
	//InitIndexBuffer();
	// fbx는 기본적으로 인덱스를 사용안한다

	// 입력 레이아웃 설정.
	if (!InitInputLayout())
		return false;

	// 기본 토폴로지 설정 (TRIANGLELIST로 설정).
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 뷰포트 설정.
	SetViewport();

	// 공간 변환 행렬 설정.
	InitTransformMatrix();

	// 라이트 정보 설정 및 바인딩.
	InitLightCB();

	// 텍스처 로드(샘플러 초기화).
	if (!InitTexture())
		return false;

	// 텍스처 바인딩.
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);
	
	// 샘플러 스테이트 바인딩.
	pDeviceContext->PSSetSamplers(0, 1, &pTextureSamplerState);

	// 래스터라이저 상태 설정.
	if (SetRasterizerState() == false)
		return false;

	// 블렌드 스테이트 설정.
	SetBlendState();

	return true;
}

bool DXApp::InitTexture()
{
	HRESULT hr;
	// 텍스처 파일 로드.
	hr = D3DX11CreateShaderResourceViewFromFile(pDevice,
		L"Char_M_Cardboard_D.png", NULL, NULL, &pTexture, NULL);

	if (FAILED(hr))
		return false;

	// 샘플러 상태 서술자 설정.
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 샘플러 상태(State) 생성.
	hr = pDevice->CreateSamplerState(&samplerDesc, &pTextureSamplerState);

	if (FAILED(hr))
		return false;

	return true;
}

bool DXApp::SetBlendState()
{
	// 블렌드 스테이드 서술자 설정.
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend =
		D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp =
		D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask
		= D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// 블렌드 스테이트 생성.
	pDevice->CreateBlendState(&blendDesc, &alphaBlendState);

	// 블렌드 스테이트 바인딩.
	pDeviceContext->OMSetBlendState(alphaBlendState, 0, 0xffffffff);

	return true;
}

void DXApp::InitLightCB()
{
	// 상수 버퍼로 전달할 라이트 값 설정.
	CBLight cbLight;
	ZeroMemory(&cbLight, sizeof(cbLight));
	cbLight.lightPosition = XMVectorSet(500.0f, 500.0f, -500.0f, 1.0f);
	cbLight.cameraPosition = cameraPos;

	// 버텍스 버퍼 서술자(Descriptor).
	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vbDesc.ByteWidth = sizeof(CBLight);
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;

	// 서브 리소스 생성.
	D3D11_SUBRESOURCE_DATA vbData;
	ZeroMemory(&vbData, sizeof(D3D11_SUBRESOURCE_DATA));
	vbData.pSysMem = &cbLight;

	// 버퍼 생성.
	ID3D11Buffer* lightBuffer;
	pDevice->CreateBuffer(&vbDesc, &vbData, &lightBuffer);

	// 정점 파이프라인에 바인딩.
	pDeviceContext->VSSetConstantBuffers(1, 1, &lightBuffer);
}

bool DXApp::InitDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
	
	depthStencilDesc.Width = clientWidth;
	depthStencilDesc.Height = clientHeight;
	depthStencilDesc.MipLevels = 1;
	// 렌더링 속도를 높이기 위해 축소한걸 미리 만들더놈.
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	
	// 24비트는 뎁스에 쓰고 8비트는 스텐실에 씀.
	// depth는 0~1사이의 값 (unsgined normalize).
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//UINT sampleCount = 1;
	//UINT maxQualityLevel = 0;
	//LPCWSTR test;
	//pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &maxQualityLevel);
	//test = maxQualityLevel + L" ";
	//MessageBox(NULL, test , L"fffff", MB_OK);

	HRESULT hr;
	hr= pDevice->CreateTexture2D(&depthStencilDesc,NULL,&depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CreateTexture2D.", L"fffff", MB_OK);
		return false;
	}

	// 뎁스 스텐실 뷰 생성.
	hr = pDevice->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CreateDepthStencilView.", L"ffff", MB_OK);
		return false;
	}
	return true;
}

bool DXApp::InitDirectInput(HINSTANCE hinstance)
{
	// 입력초기화
	HRESULT hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"DirectInput8Create.", L"ffff", MB_OK);
		return false;
	}

	// 장치 초기화
	hr = directInput->CreateDevice(GUID_SysKeyboard, &diKeyBoard, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CreateDevice GUID_SysKeyboard.", L"ffff", MB_OK);
		return false;
	}

	hr = directInput->CreateDevice(GUID_SysMouse, &diMouse, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CreateDevice GUID_SysMouse.", L"ffff", MB_OK);
		return false;
	}

	hr = diKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"SetDataFormat c_dfDIKeyboard.", L"ffff", MB_OK);
		return false;
	}

	hr = diKeyBoard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"SetCooperativeLevel c_dfDIKeyboard.", L"ffff", MB_OK);
		return false;
	}

	hr = diMouse->SetDataFormat(&c_dfDIMouse);	
	if (FAILED(hr))
	{
		MessageBox(NULL, L"SetDataFormat diMouse.", L"ffff", MB_OK);
		return false;
	}

	hr = diMouse->SetCooperativeLevel(hwnd,DISCL_EXCLUSIVE| DISCL_FOREGROUND | DISCL_NOWINKEY);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"SetCooperativeLevel(마ㅜㅇ스 협력 레벨) diMouse.", L"ffff", MB_OK);
		return false;
	}
	return true;
}

void DXApp::DetectInput(double deltaTime)
{
	// 입력 감지를 위한 값 선언.
	DIMOUSESTATE currentMouseState;
	BYTE keyboardState[256];

	// 입력 획득.
	diKeyBoard->Acquire();
	diMouse->Acquire();

	// 키보드 마우스 장치 상태 얻어오기.
	HRESULT hr = diKeyBoard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"GetDeviceState diKeyBoard.", L"ffff", MB_OK);
		return;
	}
	hr = diMouse->GetDeviceState(sizeof(currentMouseState), (LPVOID)&currentMouseState);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"GetDeviceState diMouse.", L"ffff", MB_OK);
		return;
	}

	// 키입력 확인.
	if (keyboardState[DIK_ESCAPE] & 0x80)
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}
	if (keyboardState[DIK_LEFT] & 0x80)
	{	
		moveLeftRight += 1.0f * deltaTime;
		//rotZ -= 1.0f * deltaTime;
	}
	if (keyboardState[DIK_RIGHT] & 0x80)
	{
		//rotZ += 1.0f * deltaTime;
		moveLeftRight -= 1.0f * deltaTime;
	}
	if (keyboardState[DIK_UP] & 0x80)
	{
		//rotX -= 1.0f * deltaTime;
		moveBackForward -= 1.0f * deltaTime;
	}
	if (keyboardState[DIK_DOWN] & 0x80)
	{
		//rotX += 1.0f * deltaTime;
		moveBackForward += 1.0f * deltaTime;
	}

	// 회전값이 0~360 를 반복하도록 설정 ( 단위 : 라디안)
	//if (rotX > 6.28f)
	//	rotX -= 6.28f;

	//if (rotX < 6.28f)
	//	rotX += 6.28f;

	//if (rotZ > 6.28f)
	//	rotZ -= 6.28f;

	//if (rotZ < 6.28f)
	//	rotZ += 6.28f;

	// 마우스느 ㄴlx ly만 신경ㅆ면된다
	if (currentMouseState.lX != mouseLastState.lX || currentMouseState.lY != mouseLastState.lY)
	{
		cameraYaw -= currentMouseState.lX * 0.001f;
		cameraPitch -= currentMouseState.lY * 0.001f;
	}

	// 이번 프레임의 마우스 입력을 저장.
	mouseLastState = currentMouseState;

	// 카메라 갱신.
	//updateCamera();
}

void DXApp::updateCamera()
{
	// 카메라 회전.
	cameraRotationMatrix = XMMatrixRotationRollPitchYaw(cameraPitch, cameraYaw, 0.0f);

	// 카메라가 보는 방향 설정.
	cameraTarget = XMVector3TransformCoord(defaultForward, cameraRotationMatrix);
	cameraTarget = XMVector3Normalize(cameraTarget);

	XMMATRIX rotateYTemp;
	rotateYTemp = XMMatrixRotationY(cameraYaw);

	// ㅋ메라 오른쪽 방향 설정 (회전값적용해서 방향설정)
	cameraRight = XMVector3TransformCoord(defaultRight, rotateYTemp);
	cameraUp = XMVector3TransformCoord(cameraUp, rotateYTemp);
	cameraForward = XMVector3TransformCoord(defaultForward, rotateYTemp);

	// 카메라 위치 이동.
	cameraPos += moveLeftRight * cameraRight;
	cameraPos += moveBackForward * cameraForward;

	moveLeftRight = 0.0f;
	moveBackForward= 0.0f;
	
	// ㅋ메라 바라보는 방향 벡터에 위치 적용
	cameraTarget = cameraPos + cameraTarget;

	// 뷰 행령 갱신
	view = XMMatrixLookAtLH(cameraPos, cameraTarget, cameraUp);

}

HRESULT DXApp::LoadFBX(const char * fileName, std::vector<Vertex>* pOutVertex)
{
	if (fbxManager == NULL)
	{
		// 초기 SDK Manager 설정.
		fbxManager = FbxManager::Create();
		FbxIOSettings * pIOSettings = FbxIOSettings::Create(fbxManager, IOSROOT);

		fbxManager->SetIOSettings(pIOSettings);
	}

	FbxGeometryConverter geometryConverter(fbxManager);
	FbxImporter *pImporter = FbxImporter::Create(fbxManager, "");
	FbxScene* pFbxScene = FbxScene::Create(fbxManager, "");

	// 임포터 초기화.
	if (!pImporter->Initialize(fileName, -1, fbxManager->GetIOSettings()))
	{
		return E_FAIL;
	}

	// 3D 씬 임포트.
	if (!pImporter->Import(pFbxScene))
	{
		return E_FAIL;
	}

	// 임포터 해제.
	pImporter->Destroy();

	// FbxNode 읽기.
	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
	if (pFbxRootNode == NULL)
		return E_FAIL;

	// 모델링 폴리곤 기본 구조를 삼각면으로 설정.
	geometryConverter.Triangulate(pFbxScene, true);

	int childCount = pFbxRootNode->GetChildCount();
	for (int ix = 0; ix < childCount; ++ix)
	{
		// 자식 계층 노드 읽어오기.
		FbxNode* pFbxChildNode = pFbxRootNode->GetChild(ix);
		if (pFbxChildNode->GetNodeAttribute() == NULL)
			continue;

		// 노드 속성 타입 확인.
		FbxNodeAttribute::EType attrType
			= pFbxChildNode->GetNodeAttribute()->GetAttributeType();

		// 노드 타입이 메쉬가 아닌 경우에는 무시.
		if (attrType != FbxNodeAttribute::EType::eMesh)
			continue;

		// 메쉬 정보 읽기.
		FbxMesh* pMesh = pFbxChildNode->GetMesh();


		// 컨트롤 포인트 수 배열 정보 얻어오기.
		// 컨트롤 포인트 수는 만약 네모모양 요소면 4개 버텍스는 6개가 됨
		FbxVector4* pVertices = pMesh->GetControlPoints();
		// 폴리곤 수 확인.
		int polygonCount = pMesh->GetPolygonCount();
		int vertexCounter = 0;
		for (int jx = 0; jx < polygonCount; ++jx)
		{
			// 폴리곤을 구성하는 정점 개수 구하기.
			int vertexCount = pMesh->GetPolygonSize(jx);
			for (int kx = 0; kx < vertexCount; ++kx)
			{
				// 삼각면 번호, 정점 번호를 통해 정점 읽기.
				int vertexIndex = pMesh->GetPolygonVertex(jx, kx);

				// 정점 위치 정보 읽어오기.
				Vertex vertex;
				vertex.position.x = (float)pVertices[vertexIndex].mData[0];
				vertex.position.y = (float)pVertices[vertexIndex].mData[1];
				vertex.position.z = (float)pVertices[vertexIndex].mData[2];

				// 노멀 값 읽어오기.
				XMFLOAT3 vertexNormal = ReadNormal(pMesh, vertexIndex, vertexCounter);

				// 정점 구조체에 노멀 값 추가.
				vertex.normal = vertexNormal;

				// uv값 읽어오기.
				XMFLOAT2 vertexUV = ReadUV(pMesh, vertexIndex, vertexCounter);
				
				// 정점 구조체에 추가.
				vertex.texCoord = vertexUV;

				// 정점 배열에 추가.
				pOutVertex->push_back(vertex);
				++vertexCounter;
			}
		}
	}

	return S_OK;

	return E_NOTIMPL;
}

XMFLOAT3 DXApp::ReadNormal(FbxMesh * pMesh, int controlPointIndex, int vertexCounter)
{
	XMFLOAT3 normal(0.0f, 0.0f, 0.0f);
	if (pMesh->GetElementNormalCount() < 1)
	{
		MessageBox(NULL, L"노멀 값이 유효하지 않습니다.", L"오류", MB_OK);
		return normal;
	}

	FbxGeometryElementNormal* vertexNormal = pMesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			normal.x = (float)vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0];
			normal.y = (float)vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1];
			normal.z = (float)vertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2];
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(controlPointIndex);
			normal.x = (float)vertexNormal->GetDirectArray().GetAt(index).mData[0];
			normal.y = (float)vertexNormal->GetDirectArray().GetAt(index).mData[1];
			normal.z = (float)vertexNormal->GetDirectArray().GetAt(index).mData[2];
		}
		break;

		default: return normal;
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			normal.x = (float)vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0];
			normal.y = (float)vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1];
			normal.z = (float)vertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2];
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(vertexCounter);
			normal.x = (float)vertexNormal->GetDirectArray().GetAt(index).mData[0];
			normal.y = (float)vertexNormal->GetDirectArray().GetAt(index).mData[1];
			normal.z = (float)vertexNormal->GetDirectArray().GetAt(index).mData[2];
		}
		break; default: return normal;
		}
	}
	break;

	default: return normal;
	}

	return normal;
}

XMFLOAT2 DXApp::ReadUV(FbxMesh * pMesh, int vertexIndex, int vertexCounter)
{
	// uv가 있는지 확인.
	if (pMesh->GetElementUVCount() < 1)
	{
		MessageBox(NULL, L"UV값이 유효하지망ㄹ", L"오류", MB_OK);

		return NULL;
	}

	// 리턴용 변수 선언.
	XMFLOAT2 texCoord(0.0f, 0.0f);
	// uv 전체 배열 얻어오기.
	FbxGeometryElementUV* vertexUV = pMesh->GetElementUV(0);
		
	//삼항연산자.
	const bool IsUsingIndex = vertexUV->GetReferenceMode() != FbxGeometryElement::eDirect;
	// eDirect가 아니면 인덱스를 쓰는거.
	
	const int indexCount = IsUsingIndex ? vertexUV->GetIndexArray().GetCount() : 0;
	
	switch (vertexUV->GetMappingMode())
	{
		//uv 속성이 제어점 유형인 경우.
		case FbxGeometryElement::eByControlPoint: // 제어점 기준으로 
		{
			//현재 uv값을 읽어올 인덱스 얻어오기
			int index = IsUsingIndex ? vertexUV->GetIndexArray().GetAt(vertexIndex) : vertexIndex;

			// uv값 읽어오기, (directx와 fbx uv좌표는 반대)
			texCoord.x = (float)vertexUV->GetDirectArray().GetAt(index).mData[0];
			texCoord.y = 1.0f - (float)vertexUV->GetDirectArray().GetAt(index).mData[1];

			// uv 값 반환
			return texCoord;
		}

		//uv 속성이 제어점 유형인 경우.
		case FbxGeometryElement::eByPolygonVertex: // 정점기준
		{
			int index = IsUsingIndex ? vertexUV->GetIndexArray().GetAt(vertexCounter) : vertexCounter;

			texCoord.x = (float)vertexUV->GetDirectArray().GetAt(index).mData[0];
			texCoord.y = 1.0f - (float)vertexUV->GetDirectArray().GetAt(index).mData[1];


			return texCoord;
		}

		default : 
		{
			MessageBox(NULL, L"UV값이 유효하지망ㄹ", L"오류", MB_OK);

			return NULL;
		}


	}
	
	return NULL;
}

bool DXApp::SetRasterizerState()
{
	// 래스터라이저 서술자 설정.
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.CullMode = D3D11_CULL_BACK;		// 뒷면 안그리기.
	rsDesc.FillMode = D3D11_FILL_SOLID;	// 와이퍼 프레임으로 그리기.

	HRESULT hr;
	// 래스터라이저 상태 생성.
	hr = pDevice->CreateRasterizerState(&rsDesc, &rsState);
	if (FAILED(hr))
		return false;

	// 바인딩.
	pDeviceContext->RSSetState(rsState);

	return true;
}

LRESULT DXApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		// ESC 키 눌렸는지 확인.
		if (wParam == VK_ESCAPE)
		{
			// 창 닫기 여부를 묻는 메시지 박스 생성.
			if (MessageBox(NULL, L"종료하시겠습니까?", L"종료", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				// Yes(예) 키 눌리면 창 삭제.
				DestroyWindow(this->hwnd);
			}
		}
		return 0;

		// 창이 삭제되면 실행.
	case WM_DESTROY:
		// 프로그램 종료 메시지 호출.
		PostQuitMessage(0);
		return 0;

	default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}