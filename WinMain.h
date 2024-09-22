#pragma once
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#include<dxgi1_6.h>
#include<cmath>
#include<d3d11.h>
#include<cassert>
#include<windows.h>
#include<DirectXMath.h>
using namespace DirectX;
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void updateConstantBuffer(ID3D11Resource* buffer, void* data, size_t size);

//�E�B���h�E--------------------------------------------------------------------
LPCWSTR	WindowTitle = L"loveDX11";
int ClientWidth = 1280;
int ClientHeight = 720;
int DisplayWidth = GetSystemMetrics(SM_CXSCREEN);//������
int DisplayHeight = GetSystemMetrics(SM_CYSCREEN);//��������
int ClientLeft = (DisplayWidth - ClientWidth) / 2;//�����\��
int ClientTop = (DisplayHeight - ClientHeight) / 2;//�����\��
HBRUSH HBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);//�w�i�F
#if 1 
DWORD WindowStyle = WS_OVERLAPPEDWINDOW;//�E�B���h�E�g����
#else
DWORD WindowStyle = WS_POPUP;//Alt + F4�ŕ���
#endif
HWND HWnd;//�E�B���h�E�n���h��

//�f�o�C�X�A�R���e�L�X�g-------------------------------------------------------
ID3D11Device* Device;//�o�b�t�@��o�b�t�@�r���[�A�p�C�v���C���X�e�[�g������֐�������
ID3D11DeviceContext* Context;//�o�b�t�@�r���[��p�C�v���C���X�e�[�g���p�C�v���C���ɃZ�b�g����֐�������
ID3D11Debug* Debug;
HRESULT Hr;

//���\�[�X-------------------------------------------------------------------
IDXGISwapChain1* SwapChain;
ID3D11RenderTargetView* BackBufferView;
ID3D11DepthStencilView* DepthStencilBufferView;
ID3D11Buffer* PositionBuffer;
ID3D11Buffer* TexcoordBuffer;
ID3D11Buffer* IndexBuffer;
ID3D11Buffer* ConstantBuffer0;
ID3D11Buffer* ConstantBuffer1;
ID3D11ShaderResourceView* TextureBufferView0;
ID3D11ShaderResourceView* TextureBufferView1;

//�p�C�v���C��-------------------------------------------------------------------
ID3D11InputLayout* InputLayout;
ID3D11VertexShader* VertexShader;
ID3D11PixelShader* PixelShader;
ID3D11RasterizerState* RasterizerState;
D3D11_VIEWPORT Viewport;
ID3D11SamplerState* SamplerState;
ID3D11BlendState* BlendState;
ID3D11DepthStencilState* DepthStencilState;

//WinMain.h�ɒǉ����Ă��������A�ʂɃw�b�_���������������
#include<fstream>
//�R���p�C���ς݃V�F�[�_��ǂݍ��ރt�@�C���o�b�t�@�N���X
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