#include <d3d11.h>
#include <d3dcompiler.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <windows.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

#define MAX_LOADSTRING 100
WCHAR szTitle[MAX_LOADSTRING] = L"MyApp Aptukov Mikhail";
WCHAR szWindowClass[MAX_LOADSTRING] = L"MyAppClass";

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
ID3D11VertexShader* g_pTransparentVS = nullptr;
ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3D11InputLayout* g_pVertexLayout = nullptr;
ID3D11Buffer* g_pVertexBuffer = nullptr;
ID3D11Buffer* g_pModelBuffer = nullptr;
ID3D11Buffer* g_pVPBuffer = nullptr;
ID3D11ShaderResourceView* g_pCubeTextureRV = nullptr;

ID3D11ShaderResourceView* g_pCubeNormalTextureRV = nullptr;
ID3D11SamplerState* g_pSamplerLinear = nullptr;

ID3D11VertexShader* g_pSkyboxVS = nullptr;
ID3D11PixelShader* g_pSkyboxPS = nullptr;
ID3D11InputLayout* g_pSkyboxInputLayout = nullptr;
ID3D11Buffer* g_pSkyboxVertexBuffer = nullptr;
ID3D11Buffer* g_pSkyboxVPBuffer = nullptr;
ID3D11ShaderResourceView* g_pSkyboxTextureRV = nullptr;

ID3D11PixelShader* g_pTransparentPS = nullptr;
ID3D11Buffer* g_pTransparentBuffer = nullptr;

ID3D11Buffer* g_pLightBuffer = nullptr;

ID3D11PixelShader* g_pLightPS = nullptr;
ID3D11Buffer* g_pLightColorBuffer = nullptr;

ID3D11Buffer* g_pBlueColorBuffer = nullptr;
ID3D11Buffer* g_pGreenColorBuffer = nullptr;

ID3D11BlendState* g_pAlphaBlendState = nullptr;
ID3D11DepthStencilState* g_pDSStateTrans = nullptr;

float g_CubeAngle = 0.0f;
float g_CameraAngle = 0.0f;
bool  g_MouseDragging = false;
POINT g_LastMousePos = { 0, 0 };
float g_CameraAzimuth = 0.0f;
float g_CameraElevation = 0.0f;


struct SimpleVertex
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};


SimpleVertex g_CubeVertices[] =
{

    { -0.5f, -0.5f,  0.5f,    0,0,1,   0.0f, 1.0f },
    {  0.5f, -0.5f,  0.5f,    0,0,1,   1.0f, 1.0f },
    {  0.5f,  0.5f,  0.5f,    0,0,1,   1.0f, 0.0f },
    { -0.5f, -0.5f,  0.5f,    0,0,1,   0.0f, 1.0f },
    {  0.5f,  0.5f,  0.5f,    0,0,1,   1.0f, 0.0f },
    { -0.5f,  0.5f,  0.5f,    0,0,1,   0.0f, 0.0f },

    {  0.5f, -0.5f, -0.5f,    0,0,-1,  0.0f, 1.0f },
    { -0.5f, -0.5f, -0.5f,    0,0,-1,  1.0f, 1.0f },
    { -0.5f,  0.5f, -0.5f,    0,0,-1,  1.0f, 0.0f },
    {  0.5f, -0.5f, -0.5f,    0,0,-1,  0.0f, 1.0f },
    { -0.5f,  0.5f, -0.5f,    0,0,-1,  1.0f, 0.0f },
    {  0.5f,  0.5f, -0.5f,    0,0,-1,  0.0f, 0.0f },

    { -0.5f, -0.5f, -0.5f,   -1,0,0,   0.0f, 1.0f },
    { -0.5f, -0.5f,  0.5f,   -1,0,0,   1.0f, 1.0f },
    { -0.5f,  0.5f,  0.5f,   -1,0,0,   1.0f, 0.0f },
    { -0.5f, -0.5f, -0.5f,   -1,0,0,   0.0f, 1.0f },
    { -0.5f,  0.5f,  0.5f,   -1,0,0,   1.0f, 0.0f },
    { -0.5f,  0.5f, -0.5f,   -1,0,0,   0.0f, 0.0f },

    {  0.5f, -0.5f,  0.5f,    1,0,0,   0.0f, 1.0f },
    {  0.5f, -0.5f, -0.5f,    1,0,0,   1.0f, 1.0f },
    {  0.5f,  0.5f, -0.5f,    1,0,0,   1.0f, 0.0f },
    {  0.5f, -0.5f,  0.5f,    1,0,0,   0.0f, 1.0f },
    {  0.5f,  0.5f, -0.5f,    1,0,0,   1.0f, 0.0f },
    {  0.5f,  0.5f,  0.5f,    1,0,0,   0.0f, 0.0f },

    { -0.5f,  0.5f,  0.5f,    0,1,0,   0.0f, 1.0f },
    {  0.5f,  0.5f,  0.5f,    0,1,0,   1.0f, 1.0f },
    {  0.5f,  0.5f, -0.5f,    0,1,0,   1.0f, 0.0f },
    { -0.5f,  0.5f,  0.5f,    0,1,0,   0.0f, 1.0f },
    {  0.5f,  0.5f, -0.5f,    0,1,0,   1.0f, 0.0f },
    { -0.5f,  0.5f, -0.5f,    0,1,0,   0.0f, 0.0f },

    { -0.5f, -0.5f, -0.5f,    0,-1,0,  0.0f, 1.0f },
    {  0.5f, -0.5f, -0.5f,    0,-1,0,  1.0f, 1.0f },
    {  0.5f, -0.5f,  0.5f,    0,-1,0,  1.0f, 0.0f },
    { -0.5f, -0.5f, -0.5f,    0,-1,0,  0.0f, 1.0f },
    {  0.5f, -0.5f,  0.5f,    0,-1,0,  1.0f, 0.0f },
    { -0.5f, -0.5f,  0.5f,    0,-1,0,  0.0f, 0.0f },
};

struct SkyboxVertex
{
    float x, y, z;
};

SkyboxVertex g_SkyboxVertices[] =
{
    { -1.0f, -1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f },
    { -1.0f, -1.0f,  1.0f },
    { -1.0f, -1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f },
    { -1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
    { -1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f, -1.0f },
    { 1.0f, -1.0f, -1.0f },
    { 1.0f,  1.0f, -1.0f },
    { 1.0f,  1.0f,  1.0f },
    { 1.0f, -1.0f, -1.0f },
    { 1.0f,  1.0f,  1.0f },
    { 1.0f, -1.0f,  1.0f },
    { -1.0f, 1.0f, -1.0f },
    { -1.0f, 1.0f,  1.0f },
    { 1.0f, 1.0f,  1.0f },
    { -1.0f, 1.0f, -1.0f },
    { 1.0f, 1.0f,  1.0f },
    { 1.0f, 1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
    { -1.0f, -1.0f, -1.0f },
    { 1.0f, -1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
    { 1.0f, -1.0f, -1.0f },
    { 1.0f, -1.0f,  1.0f },
};

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT             InitD3D(HWND hWnd);
HRESULT             InitGraphics();
void                CleanupD3D();
void                Render();

struct LightBufferType
{
    XMFLOAT3 light0Pos;
    float pad0;
    XMFLOAT3 light0Color;
    float pad1;
    XMFLOAT3 light1Pos;
    float pad2;
    XMFLOAT3 light1Color;
    float pad3;
    XMFLOAT3 ambient;
    float pad4;
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HWND hWnd = FindWindow(szWindowClass, szTitle);
    if (!hWnd)
        return FALSE;

    if (FAILED(InitD3D(hWnd)))
        return FALSE;

    if (FAILED(InitGraphics()))
    {
        CleanupD3D();
        return FALSE;
    }

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }
    CleanupD3D();
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = szWindowClass;
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 600,
        nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

HRESULT InitD3D(HWND hWnd)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    UINT createDeviceFlags = 0;
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0 };
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        createDeviceFlags, featureLevelArray, 1,
        D3D11_SDK_VERSION, &sd, &g_pSwapChain,
        &g_pd3dDevice, &featureLevel, &g_pImmediateContext);
    if (FAILED(hr))
        return hr;

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
        return hr;
    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ID3D11Texture2D* pDepthStencil = nullptr;
    hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
    if (FAILED(hr))
        return hr;
    hr = g_pd3dDevice->CreateDepthStencilView(pDepthStencil, nullptr, &g_pDepthStencilView);
    pDepthStencil->Release();
    if (FAILED(hr))
        return hr;

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    return S_OK;
}

HRESULT InitGraphics()
{
    HRESULT hr = S_OK;
    ID3DBlob* pBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    hr = D3DCompileFromFile(L"VS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "vs_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Vertex Shader Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    hr = g_pd3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        nullptr, &g_pVertexShader);
    if (FAILED(hr))
    {
        pBlob->Release();
        return hr;
    }
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                        D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3,           D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(float) * 6,           D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = g_pd3dDevice->CreateInputLayout(layoutDesc, 3, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &g_pVertexLayout);
    pBlob->Release();
    if (FAILED(hr))
        return hr;

    hr = D3DCompileFromFile(L"PS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "ps_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Pixel Shader Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    hr = g_pd3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        nullptr, &g_pPixelShader);
    pBlob->Release();
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * ARRAYSIZE(g_CubeVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = g_CubeVertices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pVertexBuffer);
    if (FAILED(hr))
        return hr;

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(XMMATRIX);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pModelBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC vpBufferDesc = {};
    vpBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vpBufferDesc.ByteWidth = sizeof(XMMATRIX);
    vpBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vpBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = g_pd3dDevice->CreateBuffer(&vpBufferDesc, nullptr, &g_pVPBuffer);
    if (FAILED(hr))
        return hr;

    hr = CreateDDSTextureFromFile(g_pd3dDevice, L"cube.dds", nullptr, &g_pCubeTextureRV);
    if (FAILED(hr))
        return hr;

    hr = CreateDDSTextureFromFile(g_pd3dDevice, L"cube_normal.dds", nullptr, &g_pCubeNormalTextureRV);
    if (FAILED(hr))
        return hr;

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
    if (FAILED(hr))
        return hr;

    hr = D3DCompileFromFile(L"Skybox_VS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "vs_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Skybox VS Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    hr = g_pd3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        nullptr, &g_pSkyboxVS);
    if (FAILED(hr))
    {
        pBlob->Release();
        return hr;
    }
    D3D11_INPUT_ELEMENT_DESC skyboxLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = g_pd3dDevice->CreateInputLayout(skyboxLayout, 1, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &g_pSkyboxInputLayout);
    pBlob->Release();
    if (FAILED(hr))
        return hr;

    hr = D3DCompileFromFile(L"Skybox_PS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "ps_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Skybox PS Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    hr = g_pd3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        nullptr, &g_pSkyboxPS);
    pBlob->Release();
    if (FAILED(hr))
        return hr;

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SkyboxVertex) * ARRAYSIZE(g_SkyboxVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    initData.pSysMem = g_SkyboxVertices;
    hr = g_pd3dDevice->CreateBuffer(&bd, &initData, &g_pSkyboxVertexBuffer);
    if (FAILED(hr))
        return hr;

    vpBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vpBufferDesc.ByteWidth = sizeof(XMMATRIX);
    vpBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vpBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = g_pd3dDevice->CreateBuffer(&vpBufferDesc, nullptr, &g_pSkyboxVPBuffer);
    if (FAILED(hr))
        return hr;

    hr = CreateDDSTextureFromFile(g_pd3dDevice, L"skybox.dds", nullptr, &g_pSkyboxTextureRV);
    if (FAILED(hr))
        return hr;

    hr = D3DCompileFromFile(L"Transparent_PS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "ps_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Transparent PS Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    hr = g_pd3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        nullptr, &g_pTransparentPS);
    pBlob->Release();
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC tbDesc = {};
    tbDesc.Usage = D3D11_USAGE_DEFAULT;
    tbDesc.ByteWidth = sizeof(XMFLOAT4);
    tbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    tbDesc.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&tbDesc, nullptr, &g_pTransparentBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC lbDesc = {};
    lbDesc.Usage = D3D11_USAGE_DEFAULT;
    lbDesc.ByteWidth = sizeof(LightBufferType);
    lbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lbDesc.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&lbDesc, nullptr, &g_pLightBuffer);
    if (FAILED(hr))
        return hr;

    hr = D3DCompileFromFile(L"Light_PS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "ps_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Light PS Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    hr = g_pd3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        nullptr, &g_pLightPS);
    pBlob->Release();
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC lcbDesc = {};
    lcbDesc.Usage = D3D11_USAGE_DEFAULT;
    lcbDesc.ByteWidth = sizeof(XMFLOAT4);
    lcbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lcbDesc.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&lcbDesc, nullptr, &g_pLightColorBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC colorBufferDesc = {};
    colorBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    colorBufferDesc.ByteWidth = sizeof(XMFLOAT4);
    colorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    colorBufferDesc.CPUAccessFlags = 0;

    hr = g_pd3dDevice->CreateBuffer(&colorBufferDesc, nullptr, &g_pBlueColorBuffer);
    if (FAILED(hr)) return hr;

    hr = g_pd3dDevice->CreateBuffer(&colorBufferDesc, nullptr, &g_pGreenColorBuffer);
    if (FAILED(hr)) return hr;

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = g_pd3dDevice->CreateBlendState(&blendDesc, &g_pAlphaBlendState);
    if (FAILED(hr))
        return hr;

    D3D11_DEPTH_STENCIL_DESC dsDescTrans = {};
    dsDescTrans.DepthEnable = true;
    dsDescTrans.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDescTrans.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    hr = g_pd3dDevice->CreateDepthStencilState(&dsDescTrans, &g_pDSStateTrans);
    if (FAILED(hr))
        return hr;

    hr = D3DCompileFromFile(L"transparent_vs.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "vs_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "Transparent VS Compile Error", MB_OK);
            pErrorBlob->Release();
        }
        return hr;
    }
    hr = g_pd3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        nullptr, &g_pTransparentVS);
    pBlob->Release();
    if (FAILED(hr))
        return hr;

    return S_OK;
}

void CleanupD3D()
{
    if (g_pImmediateContext)
        g_pImmediateContext->ClearState();

    if (g_pVertexBuffer)        g_pVertexBuffer->Release();
    if (g_pVertexLayout)        g_pVertexLayout->Release();
    if (g_pVertexShader)        g_pVertexShader->Release();
    if (g_pPixelShader)         g_pPixelShader->Release();
    if (g_pModelBuffer)         g_pModelBuffer->Release();
    if (g_pVPBuffer)            g_pVPBuffer->Release();
    if (g_pCubeTextureRV)       g_pCubeTextureRV->Release();

    if (g_pCubeNormalTextureRV) g_pCubeNormalTextureRV->Release();
    if (g_pSamplerLinear)       g_pSamplerLinear->Release();
    if (g_pRenderTargetView)    g_pRenderTargetView->Release();
    if (g_pDepthStencilView)    g_pDepthStencilView->Release();
    if (g_pSwapChain)           g_pSwapChain->Release();
    if (g_pImmediateContext)    g_pImmediateContext->Release();
    if (g_pd3dDevice)           g_pd3dDevice->Release();

    if (g_pSkyboxVertexBuffer)  g_pSkyboxVertexBuffer->Release();
    if (g_pSkyboxInputLayout)   g_pSkyboxInputLayout->Release();
    if (g_pSkyboxVS)            g_pSkyboxVS->Release();
    if (g_pSkyboxPS)            g_pSkyboxPS->Release();
    if (g_pSkyboxVPBuffer)      g_pSkyboxVPBuffer->Release();
    if (g_pSkyboxTextureRV)     g_pSkyboxTextureRV->Release();

    if (g_pTransparentPS)       g_pTransparentPS->Release();
    if (g_pTransparentBuffer)   g_pTransparentBuffer->Release();
    if (g_pAlphaBlendState)     g_pAlphaBlendState->Release();
    if (g_pDSStateTrans)        g_pDSStateTrans->Release();

    if (g_pLightBuffer)         g_pLightBuffer->Release();
    if (g_pLightPS)             g_pLightPS->Release();
    if (g_pLightColorBuffer)    g_pLightColorBuffer->Release();
    if (g_pTransparentVS) g_pTransparentVS->Release();
}

void Render()
{
    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    float ClearColor[4] = { 0.2f, 0.2f, 0.4f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
    g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    RECT rc;
    GetClientRect(FindWindow(szWindowClass, szTitle), &rc);
    float aspect = static_cast<float>(rc.right - rc.left) / (rc.bottom - rc.top);
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.1f, 100.0f);

    float radius = 6.0f;
    float camX = radius * sinf(g_CameraAzimuth) * cosf(g_CameraElevation);
    float camY = radius * sinf(g_CameraElevation);
    float camZ = radius * cosf(g_CameraAzimuth) * cosf(g_CameraElevation);
    XMVECTOR eyePos = XMVectorSet(camX, camY, camZ, 0.0f);
    XMVECTOR focusPoint = XMVectorZero();
    XMVECTOR upDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(eyePos, focusPoint, upDir);

    XMMATRIX viewSkybox = view;
    viewSkybox.r[3] = XMVectorSet(0, 0, 0, 1);
    XMMATRIX vpSkybox = XMMatrixTranspose(viewSkybox * proj);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(g_pImmediateContext->Map(g_pSkyboxVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        memcpy(mappedResource.pData, &vpSkybox, sizeof(XMMATRIX));
        g_pImmediateContext->Unmap(g_pSkyboxVPBuffer, 0);
    }

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    ID3D11DepthStencilState* pDSStateSkybox = nullptr;
    g_pd3dDevice->CreateDepthStencilState(&dsDesc, &pDSStateSkybox);
    g_pImmediateContext->OMSetDepthStencilState(pDSStateSkybox, 0);

    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_FRONT;
    rsDesc.FrontCounterClockwise = false;
    ID3D11RasterizerState* pSkyboxRS = nullptr;
    if (SUCCEEDED(g_pd3dDevice->CreateRasterizerState(&rsDesc, &pSkyboxRS)))
    {
        g_pImmediateContext->RSSetState(pSkyboxRS);
    }

    UINT stride = sizeof(SkyboxVertex);
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pSkyboxVertexBuffer, &stride, &offset);
    g_pImmediateContext->IASetInputLayout(g_pSkyboxInputLayout);
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pImmediateContext->VSSetShader(g_pSkyboxVS, nullptr, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pSkyboxVPBuffer);
    g_pImmediateContext->PSSetShader(g_pSkyboxPS, nullptr, 0);
    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pSkyboxTextureRV);
    g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
    g_pImmediateContext->Draw(ARRAYSIZE(g_SkyboxVertices), 0);

    pDSStateSkybox->Release();
    if (pSkyboxRS)
    {
        pSkyboxRS->Release();
        g_pImmediateContext->RSSetState(nullptr);
    }

    g_pImmediateContext->OMSetDepthStencilState(nullptr, 0);

    g_CubeAngle += 0.01f;
    XMMATRIX model = XMMatrixRotationY(g_CubeAngle);
    XMMATRIX modelT = XMMatrixTranspose(model);
    g_pImmediateContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &modelT, 0, 0);

    XMMATRIX vpCube = XMMatrixTranspose(view * proj);
    if (SUCCEEDED(g_pImmediateContext->Map(g_pVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        memcpy(mappedResource.pData, &vpCube, sizeof(XMMATRIX));
        g_pImmediateContext->Unmap(g_pVPBuffer, 0);
    }

    stride = sizeof(SimpleVertex);
    offset = 0;
    g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    g_pImmediateContext->IASetInputLayout(g_pVertexLayout);
    g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pImmediateContext->VSSetShader(g_pVertexShader, nullptr, 0);
    g_pImmediateContext->PSSetShader(g_pPixelShader, nullptr, 0);
    g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pModelBuffer);
    g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pVPBuffer);
    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pLightBuffer);
    ID3D11ShaderResourceView* cubeSRVs[2] = { g_pCubeTextureRV, g_pCubeNormalTextureRV };
    g_pImmediateContext->PSSetShaderResources(0, 2, cubeSRVs);
    g_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
    g_pImmediateContext->Draw(ARRAYSIZE(g_CubeVertices), 0);

    static float orbitAngle = 0.0f;
    orbitAngle += 0.005f;
    XMMATRIX model2 = XMMatrixRotationY(orbitAngle) * XMMatrixTranslation(3.0f, 0.0f, 0.0f) * XMMatrixRotationY(-orbitAngle);
    XMMATRIX model2T = XMMatrixTranspose(model2);
    g_pImmediateContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &model2T, 0, 0);
    g_pImmediateContext->Draw(ARRAYSIZE(g_CubeVertices), 0);

    float blendFactor[4] = { 0, 0, 0, 0 };
    g_pImmediateContext->OMSetBlendState(g_pAlphaBlendState, blendFactor, 0xFFFFFFFF);
    g_pImmediateContext->OMSetDepthStencilState(g_pDSStateTrans, 0);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    g_pImmediateContext->PSSetShaderResources(0, 1, &g_pCubeTextureRV);;

    static float blueAnim = 0.0f;
    blueAnim += 0.02f;
    float blueOffsetZ = 2.0f * sinf(blueAnim);
    XMMATRIX modelBlue = XMMatrixTranslation(-2.0f, 0.0f, blueOffsetZ);
    XMVECTOR bluePos = XMVectorSet(-2.0f, 0.0f, blueOffsetZ, 1.0f);

    static float greenAnim = 0.0f;
    greenAnim += 0.02f;
    float greenOffsetY = 2.0f * sinf(greenAnim);
    XMMATRIX modelGreen = XMMatrixTranslation(2.0f, greenOffsetY, 0.0f);
    XMVECTOR greenPos = XMVectorSet(2.0f, greenOffsetY, 0.0f, 1.0f);

    float blueDist = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(bluePos, eyePos)));
    float greenDist = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(greenPos, eyePos)));

    g_pImmediateContext->PSSetShader(g_pTransparentPS, nullptr, 0);
    g_pImmediateContext->VSSetShader(g_pTransparentVS, nullptr, 0);
    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pLightBuffer);


    if (blueDist >= greenDist)
    {
        XMMATRIX modelBlueT = XMMatrixTranspose(modelBlue);
        g_pImmediateContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &modelBlueT, 0, 0);

        XMFLOAT4 blueColor(0.0f, 0.0f, 1.0f, 0.5f);
        g_pImmediateContext->UpdateSubresource(g_pBlueColorBuffer, 0, nullptr, &blueColor, 0, 0);

        g_pImmediateContext->PSSetConstantBuffers(1, 1, &g_pBlueColorBuffer);

        g_pImmediateContext->Draw(ARRAYSIZE(g_CubeVertices), 0);


        XMMATRIX modelGreenT = XMMatrixTranspose(modelGreen);
        g_pImmediateContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &modelGreenT, 0, 0);

        XMFLOAT4 greenColor(0.0f, 1.0f, 0.0f, 0.5f);
        g_pImmediateContext->UpdateSubresource(g_pGreenColorBuffer, 0, nullptr, &greenColor, 0, 0);

        g_pImmediateContext->PSSetConstantBuffers(1, 1, &g_pGreenColorBuffer);

        g_pImmediateContext->Draw(ARRAYSIZE(g_CubeVertices), 0);

    }
    else
    {
        XMMATRIX modelGreenT = XMMatrixTranspose(modelGreen);
        g_pImmediateContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &modelGreenT, 0, 0);

        XMFLOAT4 greenColor(0.0f, 1.0f, 0.0f, 0.5f);
        g_pImmediateContext->UpdateSubresource(g_pGreenColorBuffer, 0, nullptr, &greenColor, 0, 0);

        g_pImmediateContext->PSSetConstantBuffers(1, 1, &g_pGreenColorBuffer);

        g_pImmediateContext->Draw(ARRAYSIZE(g_CubeVertices), 0);

        XMMATRIX modelBlueT = XMMatrixTranspose(modelBlue);
        g_pImmediateContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &modelBlueT, 0, 0);

        XMFLOAT4 blueColor(0.0f, 0.0f, 1.0f, 0.5f);
        g_pImmediateContext->UpdateSubresource(g_pBlueColorBuffer, 0, nullptr, &blueColor, 0, 0);

        g_pImmediateContext->PSSetConstantBuffers(1, 1, &g_pBlueColorBuffer);

        g_pImmediateContext->Draw(ARRAYSIZE(g_CubeVertices), 0);
    }

    g_pImmediateContext->OMSetBlendState(nullptr, blendFactor, 0xFFFFFFFF);
    g_pImmediateContext->OMSetDepthStencilState(nullptr, 0);


    LightBufferType lightData;
    lightData.light0Pos = XMFLOAT3(1.0f, 0.0f, 0.0f);
    lightData.light0Color = XMFLOAT3(3.0f, 3.0f, 0.0f);
    lightData.light1Pos = XMFLOAT3(-1.0f, 0.0f, 0.0f);
    lightData.light1Color = XMFLOAT3(3.0f, 3.0f, 3.0f);
    lightData.ambient = XMFLOAT3(0.0f, 0.0f, 0.0f);
    g_pImmediateContext->UpdateSubresource(g_pLightBuffer, 0, nullptr, &lightData, 0, 0);

    g_pImmediateContext->PSSetShader(g_pLightPS, nullptr, 0);
    g_pImmediateContext->PSSetConstantBuffers(0, 1, &g_pLightColorBuffer);

    XMMATRIX lightScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);


    XMMATRIX lightTranslate0 = XMMatrixTranslation(1.0f, 0.0f, 0.0f);
    XMMATRIX lightModel0 = XMMatrixTranspose(lightScale * lightTranslate0);
    g_pImmediateContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &lightModel0, 0, 0);
    XMFLOAT4 lightColor0(1.0f, 1.0f, 0.0f, 1.0f);
    g_pImmediateContext->UpdateSubresource(g_pLightColorBuffer, 0, nullptr, &lightColor0, 0, 0);
    g_pImmediateContext->Draw(ARRAYSIZE(g_CubeVertices), 0);

    XMMATRIX lightTranslate1 = XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
    XMMATRIX lightModel1 = XMMatrixTranspose(lightScale * lightTranslate1);
    g_pImmediateContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &lightModel1, 0, 0);
    XMFLOAT4 lightColor1(1.0f, 1.0f, 1.0f, 1.0f);
    g_pImmediateContext->UpdateSubresource(g_pLightColorBuffer, 0, nullptr, &lightColor1, 0, 0);
    g_pImmediateContext->Draw(ARRAYSIZE(g_CubeVertices), 0);

    g_pSwapChain->Present(1, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
        g_MouseDragging = true;
        g_LastMousePos.x = LOWORD(lParam);
        g_LastMousePos.y = HIWORD(lParam);
        SetCapture(hWnd);
        break;
    case WM_LBUTTONUP:
        g_MouseDragging = false;
        ReleaseCapture();
        break;
    case WM_MOUSEMOVE:
        if (g_MouseDragging)
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int dx = x - g_LastMousePos.x;
            int dy = y - g_LastMousePos.y;
            g_CameraAzimuth += dx * 0.005f;
            g_CameraElevation += dy * 0.005f;
            if (g_CameraElevation > XM_PIDIV2 - 0.01f)
                g_CameraElevation = XM_PIDIV2 - 0.01f;
            if (g_CameraElevation < -XM_PIDIV2 + 0.01f)
                g_CameraElevation = -XM_PIDIV2 + 0.01f;
            g_LastMousePos.x = x;
            g_LastMousePos.y = y;
        }
        break;
    case WM_KEYDOWN:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
