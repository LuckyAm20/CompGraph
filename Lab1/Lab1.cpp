﻿#include <d3d11.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <windows.h>

#pragma comment(lib, "d3d11.lib")

#define MAX_LOADSTRING 100
WCHAR szTitle[MAX_LOADSTRING] = L"MyApp Aptukov Mikhail";
WCHAR szWindowClass[MAX_LOADSTRING] = L"MyAppClass";

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

float g_CurrentColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
float g_TargetColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT             InitD3D(HWND hWnd);
void                CleanupD3D();
void                Render();
void                GenerateNewTargetColor();

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
    wcex.hbrBackground = NULL;
    wcex.lpszClassName = szWindowClass;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
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
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

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

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_pImmediateContext->RSSetViewports(1, &vp);

    return S_OK;
}

void CleanupD3D()
{
    if (g_pImmediateContext)
        g_pImmediateContext->ClearState();
    if (g_pRenderTargetView)
        g_pRenderTargetView->Release();
    if (g_pSwapChain)
        g_pSwapChain->Release();
    if (g_pImmediateContext)
        g_pImmediateContext->Release();
    if (g_pd3dDevice)
        g_pd3dDevice->Release();
}

void GenerateNewTargetColor()
{
    float newColor[3];
    do {
        newColor[0] = static_cast<float>(rand()) / RAND_MAX;
        newColor[1] = static_cast<float>(rand()) / RAND_MAX;
        newColor[2] = static_cast<float>(rand()) / RAND_MAX;
    } while ((newColor[0] < 0.05f && newColor[1] < 0.05f && newColor[2] < 0.05f) ||
        (newColor[0] > 0.95f && newColor[1] > 0.95f && newColor[2] > 0.95f));
    g_TargetColor[0] = newColor[0];
    g_TargetColor[1] = newColor[1];
    g_TargetColor[2] = newColor[2];
}

void Render()
{
    float lerpFactor = 0.01f;
    for (int i = 0; i < 3; ++i)
        g_CurrentColor[i] += (g_TargetColor[i] - g_CurrentColor[i]) * lerpFactor;

    if (std::fabs(g_CurrentColor[0] - g_TargetColor[0]) < 0.01f &&
        std::fabs(g_CurrentColor[1] - g_TargetColor[1]) < 0.01f &&
        std::fabs(g_CurrentColor[2] - g_TargetColor[2]) < 0.01f)
    {
        GenerateNewTargetColor();
    }

    float ClearColor[4] = { g_CurrentColor[0], g_CurrentColor[1], g_CurrentColor[2], 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
    g_pSwapChain->Present(1, 0);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ERASEBKGND:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        COLORREF clr = RGB(static_cast<int>(g_CurrentColor[0] * 255),
            static_cast<int>(g_CurrentColor[1] * 255),
            static_cast<int>(g_CurrentColor[2] * 255));
        HBRUSH hBrush = CreateSolidBrush(clr);
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);
        EndPaint(hWnd, &ps);
        return 1;
    }
    case WM_SIZE:
        if (g_pSwapChain && wParam != SIZE_MINIMIZED)
        {
            if (g_pRenderTargetView)
            {
                g_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
                g_pRenderTargetView->Release();
                g_pRenderTargetView = nullptr;
            }
            HRESULT hr = g_pSwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            if (SUCCEEDED(hr))
            {
                ID3D11Texture2D* pBuffer = nullptr;
                hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBuffer);
                if (SUCCEEDED(hr))
                {
                    hr = g_pd3dDevice->CreateRenderTargetView(pBuffer, nullptr, &g_pRenderTargetView);
                    pBuffer->Release();
                }
                if (SUCCEEDED(hr))
                {
                    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);
                    RECT rc;
                    GetClientRect(hWnd, &rc);
                    D3D11_VIEWPORT vp = {};
                    vp.Width = static_cast<FLOAT>(rc.right - rc.left);
                    vp.Height = static_cast<FLOAT>(rc.bottom - rc.top);
                    vp.MinDepth = 0.0f;
                    vp.MaxDepth = 1.0f;
                    g_pImmediateContext->RSSetViewports(1, &vp);
                }
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
