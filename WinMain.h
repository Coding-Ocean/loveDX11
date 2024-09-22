#pragma once
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#include<windows.h>
#include<dxgi1_6.h>
#include<d3d11.h>
#include<vector>
#include<cassert>
#include<DirectXMath.h>
using namespace DirectX;
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void updateConstantBuffer(ID3D11Resource* buffer, void* data, size_t size);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

//ウィンドウ--------------------------------------------------------------------
LPCWSTR	WindowTitle = L"loveDX11";
int ClientWidth = 1920;
int ClientHeight = 1080;
int DisplayWidth = GetSystemMetrics(SM_CXSCREEN);//幅測定
int DisplayHeight = GetSystemMetrics(SM_CYSCREEN);//高さ測定
int ClientLeft = (DisplayWidth - ClientWidth) / 2;//中央表示
int ClientTop = (DisplayHeight - ClientHeight) / 2;//中央表示
HBRUSH HBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//背景色
#if 0 
DWORD WindowStyle = WS_OVERLAPPEDWINDOW;//ウィンドウ枠あり
#else
DWORD WindowStyle = WS_POPUP;//Alt + F4で閉じる
#endif
HWND HWnd;//ウィンドウハンドル

//デバイス、コンテキスト-------------------------------------------------------
ID3D11Device* Device;//バッファやバッファビュー、パイプラインステートをつくる関数をもつ
ID3D11DeviceContext* Context;//バッファビューやパイプラインステートをパイプラインにセットする関数をもつ
ID3D11Debug* Debug;
HRESULT Hr;

//リソース-------------------------------------------------------------------
IDXGISwapChain1* SwapChain;
ID3D11RenderTargetView* BackBufferView;
ID3D11DepthStencilView* DepthStencilBufferView;
ID3D11Buffer* PositionBuffer;
ID3D11Buffer* TexcoordBuffer;
ID3D11Buffer* IndexBuffer;
ID3D11Buffer* ConstantBuffer0;
ID3D11Buffer* ConstantBuffer1;
ID3D11ShaderResourceView* TextureBufferView;

//パイプライン-------------------------------------------------------------------
ID3D11InputLayout* InputLayout;
ID3D11VertexShader* VertexShader;
ID3D11PixelShader* PixelShader;
ID3D11RasterizerState* RasterizerState;
D3D11_VIEWPORT Viewport;
ID3D11SamplerState* SamplerState;
ID3D11BlendState* BlendState;
ID3D11DepthStencilState* DepthStencilState;

//WinMain.hに追加してもいいが、別にヘッダを作った方がいい
#include<fstream>
//コンパイル済みシェーダを読み込むファイルバッファクラス
class BIN_FILE11 {
public:
    BIN_FILE11(const char* fileName) :Succeeded(false) {
        std::ifstream ifs(fileName, std::ios::binary);
        if (ifs.fail()) {
            return;
        }
        Succeeded = true;
        std::istreambuf_iterator<char> first(ifs);
        std::istreambuf_iterator<char> last;
        Buffer.assign(first, last);
        ifs.close();
    }
    bool succeeded() const { return Succeeded; }
    const char* code() const { return Buffer.data(); }
    size_t size() const { return Buffer.size(); }
private:
    std::string Buffer;
    bool Succeeded;
};