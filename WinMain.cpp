#include"WinMain.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
    //�E�B���h�E
    {
        //�E�B���h�E�N���X�o�^
        {
            WNDCLASSEX windowClass = {};
            windowClass.lpszClassName = L"GAME_WINDOW";
            windowClass.hbrBackground = HBackground;
            windowClass.hInstance = hInstance;
            windowClass.lpfnWndProc = WndProc;
            windowClass.hIcon = LoadIcon(NULL, IDC_ICON);
            windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
            windowClass.cbSize = sizeof(WNDCLASSEX);
            RegisterClassEx(&windowClass);
        }
        //�E�B���h�E�������ĕ\��
        {
            //�܂��A�\���ʒu�A�E�B���h�E�̑傫������
            RECT windowRect = { 0, 0, ClientWidth, ClientHeight };
            AdjustWindowRect(&windowRect, WindowStyle, FALSE);
            int windowLeft = ClientLeft + windowRect.left;
            int windowTop = ClientTop + windowRect.top;
            int windowWidth = windowRect.right - windowRect.left;
            int windowHeight = windowRect.bottom - windowRect.top;
            //���ɁA�E�B���h�E������
            HWnd = CreateWindowEx(
                NULL,//�g���X�^�C���Ȃ�
                L"GAME_WINDOW",
                WindowTitle,
                WindowStyle,
                windowLeft,
                windowTop,
                windowWidth,
                windowHeight,
                NULL,//�e�E�B���h�E�Ȃ�
                NULL,//���j���[�Ȃ�
                hInstance,
                NULL//�����E�B���h�E�Ȃ�
            );
            //�E�B���h�E�\��
            ShowWindow(HWnd, SW_SHOW);
        }
    }
    //�f�o�C�X�A�R���e�L�X�g
    {
        UINT creationFlags = 0;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);
        Hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &Device,
            nullptr,
            &Context);
        assert(SUCCEEDED(Hr));

#ifdef _DEBUG
        //�f�o�b�O�C���^�[�t�F�C�X
        Hr = Device->QueryInterface(
            __uuidof(ID3D11Debug),
            reinterpret_cast<void**>(&Debug));
        assert(SUCCEEDED(Hr));
#endif
    }
    //���\�[�X
    {
        //�X���b�v�`�F�C��������(�o�b�N�o�b�t�@���܂�ł���)
        {
            //DXGI�t�@�N�g��������
            IDXGIFactory4* dxgiFactory;
            Hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
            assert(SUCCEEDED(Hr));

            //�X���b�v�`�F�C�����L�q���Ă���
            DXGI_SWAP_CHAIN_DESC1 desc = {};
            desc.Width = ClientWidth;
            desc.Height = ClientHeight;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            desc.BufferCount = 2;
            desc.SampleDesc.Count = 1;
            desc.Scaling = DXGI_SCALING_NONE;
            Hr = dxgiFactory->CreateSwapChainForHwnd(Device, HWnd, &desc, NULL, NULL,
                &SwapChain);
            assert(SUCCEEDED(Hr));

            //��������Ȃ�
            dxgiFactory->Release();
        }
        //�o�b�N�o�b�t�@�̃r���[������
        {
            //�X���b�v�`�F�C������o�b�N�o�b�t�@���擾
            ID3D11Texture2D* backBuffer;
            Hr = SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
            assert(SUCCEEDED(Hr));

            //�o�b�N�o�b�t�@�̃r���[������
            Hr = Device->CreateRenderTargetView(backBuffer, nullptr, &BackBufferView);
            assert(SUCCEEDED(Hr));

            //��������Ȃ�
            backBuffer->Release();
        }
        //�f�v�X�X�e���V���o�b�t�@�̃r���[������
        {
            //�f�v�X�X�e���V���o�b�t�@�̋L�q
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = ClientWidth;
            desc.Height = ClientHeight;
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.CPUAccessFlags = 0;
            desc.Format = DXGI_FORMAT_D32_FLOAT;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.MiscFlags = 0;
            //�f�v�X�X�e���V���o�b�t�@������
            ID3D11Texture2D* depthStencilBuffer = 0;
            Hr = Device->CreateTexture2D(&desc, nullptr, &depthStencilBuffer);
            assert(SUCCEEDED(Hr));

            //�f�v�X�X�e���V���o�b�t�@�̃r���[������
            D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = desc.Format;
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            dsvDesc.Texture2D.MipSlice = 0;
            Hr = Device->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &DepthStencilBufferView);
            assert(SUCCEEDED(Hr));

            depthStencilBuffer->Release();
        }
        //���_�o�b�t�@-positions
        {
            //�R�p�`�̒��_�ʒu�f�[�^��p��
            XMFLOAT3 positions[] = {
                { -0.5f, -0.5f, 0.0f },
                { -0.5f,  0.5f, 0.0f },
                {  0.5f, -0.5f, 0.0f },
                {  0.5f,  0.5f, 0.0f },
            };
            //�o�b�t�@�L�q
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(positions);//�z��̑S��
            desc.Usage = D3D11_USAGE_DEFAULT;                 //�f�t�H���g�A�N�Z�X
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;        //���_�o�b�t�@�Ƃ���GPU�ɕR�Â�
            desc.CPUAccessFlags = 0;                          //CPU�̃o�b�t�@�ւ̃A�N�Z�X����
            //�f�[�^
            D3D11_SUBRESOURCE_DATA data = {};
            data.pSysMem = positions;
            //�o�b�t�@���쐬
            Hr = Device->CreateBuffer(&desc, &data, &PositionBuffer);
            assert(SUCCEEDED(Hr));
        }
        //���_�o�b�t�@-texcoords
        {
            //�S�p�`�̒��_�e�N�X�`�����W�f�[�^��p��
            XMFLOAT2 texcoords[] = {
                { 0.0f, 1.0f },
                { 0.0f, 0.0f },
                { 1.0f, 1.0f },
                { 1.0f, 0.0f },
            };
            //�o�b�t�@�L�q
            D3D11_BUFFER_DESC desc{};
            desc.ByteWidth = sizeof(texcoords);//�z��̑S��
            desc.Usage = D3D11_USAGE_DEFAULT;            //�f�t�H���g�A�N�Z�X
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //���_�o�b�t�@�Ƃ���GPU�ɕR�Â�
            desc.CPUAccessFlags = 0;                     //CPU�̃o�b�t�@�ւ̃A�N�Z�X����
            //�f�[�^�̐ݒ�
            D3D11_SUBRESOURCE_DATA data{};
            data.pSysMem = texcoords;
            //�o�b�t�@���쐬
            Hr = Device->CreateBuffer(&desc, &data, &TexcoordBuffer);
            assert(SUCCEEDED(Hr));
        }
        //�C���f�b�N�X�o�b�t�@
        {
            //�C���f�b�N�X�f�[�^��p��
            unsigned short indices[] = {
                0,1,2,
                3,2,1,
            };
            //�o�b�t�@�̋L�q
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(indices);
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//�C���f�b�N�X�o�b�t�@�Ƃ���GPU�ɕR�Â�
            desc.CPUAccessFlags = 0;
            //�f�[�^
            D3D11_SUBRESOURCE_DATA data = {};
            data.pSysMem = indices;
            //�o�b�t�@������
            Hr = Device->CreateBuffer(&desc, &data, &IndexBuffer);
            assert(SUCCEEDED(Hr));
        }
        //�R���X�^���g�o�b�t�@�O�u�}�g���b�N�X�p�v
        {
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(float) * 16;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//�萔�o�b�t�@�Ƃ��ĕR�Â�
            desc.Usage = D3D11_USAGE_DYNAMIC;//���I�ȃo�b�t�@
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU����X�V����
            Hr = Device->CreateBuffer(&desc, nullptr, &ConstantBuffer0);
            assert(SUCCEEDED(Hr));
        }
        //�R���X�^���g�o�b�t�@�P�u�f�B�t���[�Y�p�v
        {
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(float) * 4;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//�萔�o�b�t�@�Ƃ��ĕR�Â�
            desc.Usage = D3D11_USAGE_DYNAMIC;//���I�ȃo�b�t�@
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU����X�V����
            Hr = Device->CreateBuffer(&desc, nullptr, &ConstantBuffer1);
            assert(SUCCEEDED(Hr));
        }
        //�e�N�X�`���o�b�t�@�r���[�O������
        {
            //�e�N�X�`���f�[�^�������ł���
            UINT texWidth = 5;
            UINT texHeight = 5;
            struct pixel { unsigned char r, g, b, a; };
            UINT bytePerPixel = sizeof(pixel);
            pixel* pixels = new pixel[texWidth * texHeight];
            for (UINT y = 0; y < texHeight; y++) {
                for (UINT x = 0; x < texWidth; x++) {
                    if ((x + y) % 2) {
                        pixels[x + y * texWidth] = { 255,255,255,255 };
                    }
                    else {
                        pixels[x + y * texWidth] = { 0,0,0,255 };
                    }
                }
            }

            //�e�N�X�`���o�b�t�@�L�q
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width = texWidth;
            desc.Height = texHeight;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            desc.CPUAccessFlags = 0;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.MiscFlags = 0;
            //�e�N�X�`���f�[�^
            D3D11_SUBRESOURCE_DATA data{};
            data.pSysMem = (void*)pixels;
            data.SysMemPitch = texWidth * bytePerPixel;
            data.SysMemSlicePitch = texWidth * texHeight * bytePerPixel;
            //�e�N�X�`���o�b�t�@������
            ID3D11Texture2D* textureBuffer = nullptr;
            Hr = Device->CreateTexture2D(&desc, &data, &textureBuffer);
            assert(SUCCEEDED(Hr));

            //�e�N�X�`���o�b�t�@�̃r���[������
            Hr = Device->CreateShaderResourceView(textureBuffer, nullptr, &TextureBufferView0);
            assert(SUCCEEDED(Hr));

            //���
            textureBuffer->Release();
            delete[] pixels;
        }
        //�e�N�X�`���o�b�t�@�r���[�P������
        {
            //�e�N�X�`���f�[�^���t�@�C������ǂݍ���
            const char* filename = "assets\\penguin1.png";
            unsigned char* pixels = nullptr;
            int texWidth;
            int texHeight;
            int bytePerPixel;
            pixels = stbi_load(filename, &texWidth, &texHeight, &bytePerPixel, 4);
            assert(pixels != nullptr);

            //�e�N�X�`���o�b�t�@�L�q
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = texWidth;
            desc.Height = texHeight;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            desc.CPUAccessFlags = 0;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.MiscFlags = 0;
            //�e�N�X�`���f�[�^
            D3D11_SUBRESOURCE_DATA data = {};
            data.pSysMem = (void*)pixels;
            data.SysMemPitch = texWidth * bytePerPixel;
            data.SysMemSlicePitch = texWidth * texHeight * bytePerPixel;
            //�e�N�X�`���o�b�t�@������
            ID3D11Texture2D* textureBuffer = nullptr;
            Hr = Device->CreateTexture2D(&desc, &data, &textureBuffer);
            assert(SUCCEEDED(Hr));

            //�e�N�X�`���o�b�t�@�̃r���[������
            Hr = Device->CreateShaderResourceView(textureBuffer, nullptr, &TextureBufferView1);
            assert(SUCCEEDED(Hr));

            //���
            textureBuffer->Release();
            stbi_image_free(pixels);
        }
    }{}
    //�p�C�v���C��
    {
        //�C���v�b�g���C�A�E�g �� �V�F�[�_�[
        {
            //���_�C���v�b�g���C�A�E�g���`���A�T�C�Y�����߂Ă���
            UINT slot0 = 0;
            UINT slot1 = 1;
            D3D11_INPUT_ELEMENT_DESC Elements[]{
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, slot0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    slot1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
            UINT numElements = sizeof(Elements) / sizeof(Elements[0]);

            //�V�F�[�_�[�o�C�g�R�[�h�̓ǂݍ���
            BIN_FILE11 vs("assets\\VertexShader.cso");
            assert(vs.succeeded());
            BIN_FILE11 ps("assets\\PixelShader.cso");
            assert(ps.succeeded());

            //���_�C���v�b�g���C�A�E�g������
            Hr = Device->CreateInputLayout(Elements, numElements,
                vs.code(), vs.size(), &InputLayout);
            assert(SUCCEEDED(Hr));

            //���_�V�F�[�_������
            Hr = Device->CreateVertexShader(vs.code(), vs.size(), NULL, &VertexShader);
            assert(SUCCEEDED(Hr));

            //�s�N�Z���V�F�[�_�[������
            Hr = Device->CreatePixelShader(ps.code(), ps.size(), NULL, &PixelShader);
            assert(SUCCEEDED(Hr));
        }
        //���X�^���C�U�[�X�e�[�g
        {
            D3D11_RASTERIZER_DESC desc;
            desc.FrontCounterClockwise = FALSE;
            desc.CullMode = D3D11_CULL_NONE;
            desc.DepthBias = 0;
            desc.DepthBiasClamp = 0;
            desc.SlopeScaledDepthBias = 0;
            desc.DepthClipEnable = FALSE;
            desc.ScissorEnable = FALSE;
            desc.MultisampleEnable = FALSE;
            desc.AntialiasedLineEnable = FALSE;
            desc.FillMode = D3D11_FILL_SOLID;
            //desc.FillMode = D3D11_FILL_WIREFRAME;
            Device->CreateRasterizerState(&desc, &RasterizerState);
        }
        //�\���̈���쐬
        {
            Viewport.Width = static_cast<float>(ClientWidth);
            Viewport.Height = static_cast<float>(ClientHeight);
            Viewport.TopLeftX = 0;
            Viewport.TopLeftY = 0;
            Viewport.MinDepth = D3D11_MIN_DEPTH;// 0.0f
            Viewport.MaxDepth = D3D11_MAX_DEPTH;// 1.0f
        }
        //�T���v���X�e�[�g
        {
            D3D11_SAMPLER_DESC desc = {};
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            //desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            desc.MinLOD = 0;
            desc.MaxLOD = D3D11_FLOAT32_MAX;
            Hr = Device->CreateSamplerState(&desc, &SamplerState);
        }
        //�u�����h�X�e�[�g
        {
            D3D11_BLEND_DESC desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            for (int i = 0; i < 1; i++) {
                desc.RenderTarget[i].BlendEnable = TRUE;

                desc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
                desc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
                desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;

                desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
                desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
                desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
                desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            }
            Hr = Device->CreateBlendState(&desc, &BlendState);
        }
        //�f�v�X�X�e���V���X�e�[�g
        {
            D3D11_DEPTH_STENCIL_DESC desc = {};
            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            desc.StencilEnable = FALSE;
            Hr = Device->CreateDepthStencilState(&desc, &DepthStencilState);
            assert(SUCCEEDED(Hr));
        }
    }
    //���C�����[�v
    while (true)
    {
        //�E�B���h�E���b�Z�[�W�̎擾�A���o
        {
            MSG msg = { 0 };
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) break;
                DispatchMessage(&msg);
                continue;
            }
        }
        //�o�b�N�o�b�t�@�̃N���A
        {
            //�u�����h�X�e�[�g���Z�b�g
            float blendFactor[4] = { 0,0,0,0 };
            Context->OMSetBlendState(BlendState, blendFactor, 0xffffffff);
            //�f�v�X�X�e���V���X�e�[�g���Z�b�g
            Context->OMSetDepthStencilState(DepthStencilState, 0);

            //�o�b�N�o�b�t�@�ƃf�v�X�X�e���V���o�b�t�@�������_�[�^�[�Q�b�g�ɐݒ�
            Context->OMSetRenderTargets(1, &BackBufferView, DepthStencilBufferView);
            //�����_�[�^�[�Q�b�g�r���[���w�肵���F�ŃN���A
            static float radian = 0.0f;
            float r = cos(radian) * 0.5f + 0.5f;
            radian += 0.01f;
            FLOAT clearColor[4] = { r, 0.25f, 0.5f, 1.0f };
            Context->ClearRenderTargetView(BackBufferView, clearColor);
            //�f�v�X�X�e���V���o�b�t�@��1.0�ŃN���A
            Context->ClearDepthStencilView(DepthStencilBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        }
        //�o�b�N�o�b�t�@�֕`��
        {
            //���_�C���v�b�g���C�A�E�g���Z�b�g
            Context->IASetInputLayout(InputLayout);
            //�g�|���W�[���Z�b�g
            Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            //���_�o�b�t�@���Z�b�g
            //���_�o�b�t�@���܂Ƃ߂Ă���Z�b�g
            ID3D11Buffer* buffers[] = { PositionBuffer, TexcoordBuffer };
            UINT stride[2] = { sizeof(XMFLOAT3), sizeof(XMFLOAT2) };
            UINT offset[2] = { 0, 0 };
            Context->IASetVertexBuffers(0, 2, buffers, stride, offset);
            //ID3D11Buffer* buffers[] = { PositionBuffer };
            //UINT stride[] = { sizeof(XMFLOAT3) };
            //UINT offset[] = { 0 };
            //Context->IASetVertexBuffers(0, 1, buffers, stride, offset);
            //�C���f�b�N�X�o�b�t�@���Z�b�g
            Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

            //���_�V�F�[�_���Z�b�g
            Context->VSSetShader(VertexShader, nullptr, 0);
            //�R���X�^���g�o�b�t�@�O���Z�b�g
            UINT b0 = 0;
            Context->VSSetConstantBuffers(b0, 1, &ConstantBuffer0);

            //���X�^���C�U�[���Z�b�g
            Context->RSSetState(RasterizerState);
            //�r���[�|�[�g���Z�b�g
            Context->RSSetViewports(1, &Viewport);
            //�s�N�Z���V�F�[�_���Z�b�g
            Context->PSSetShader(PixelShader, nullptr, 0);
            //�R���X�^���g�o�b�t�@�P���Z�b�g
            UINT b1 = 1;
            Context->PSSetConstantBuffers(b1, 1, &ConstantBuffer1);
            //�T���v���[���Z�b�g
            UINT s0 = 0;
            Context->PSSetSamplers(s0, 1, &SamplerState);

            //���ʂ̕ϐ�
            static float radian = 0.0f;
            radian += 0.01f;
            XMVECTOR eye = { 0,0,-3.0f }, focus = { 0, 0, 0 }, up = { 0, 1, 0 };
            XMMATRIX view = XMMatrixLookAtLH(eye, focus, up);
            XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)ClientWidth / ClientHeight, 1.0f, 20.f);
            //��
            {
                //�R���X�^���g�o�b�t�@�O���X�V
                XMMATRIX world = XMMatrixTranslation(-sin(radian),0,-cos(radian));
                XMMATRIX mat = world * view * proj;
                updateConstantBuffer(ConstantBuffer0, &mat, sizeof(mat));
                //�R���X�^���g�o�b�t�@�P���X�V
                XMFLOAT4 diffuse = { 0.0f,1.0f,1.0f,1.0f };
                updateConstantBuffer(ConstantBuffer1, &diffuse, sizeof(diffuse));
                //�e�N�X�`���Z�b�g
                UINT t0 = 0;
                Context->PSSetShaderResources(t0, 1, &TextureBufferView0);
                //�`��
                Context->DrawIndexed(6, 0, 0);
            }
            //�y���M��
            {
                //�R���X�^���g�o�b�t�@�O���X�V
                XMMATRIX world = XMMatrixTranslation(sin(radian), 0, cos(radian));;
                XMMATRIX mat = world * view * proj;
                updateConstantBuffer(ConstantBuffer0, &mat, sizeof(mat));
                //�R���X�^���g�o�b�t�@�P���X�V
                XMFLOAT4 diffuse = { 1.0f,1.0f,1.0f,1.0f };
                updateConstantBuffer(ConstantBuffer1, &diffuse, sizeof(diffuse));
                //�e�N�X�`���Z�b�g
                UINT t0 = 0;
                Context->PSSetShaderResources(t0, 1, &TextureBufferView1);
                //�`��
                Context->DrawIndexed(6, 0, 0);
            }
        }
        //�o�b�N�o�b�t�@�̕\��
        {
            SwapChain->Present(1, 0);
        }
    }{}

    //���
    {
        DepthStencilState->Release();
        BlendState->Release();
        SamplerState->Release();
        RasterizerState->Release();
        VertexShader->Release();
        PixelShader->Release();
        InputLayout->Release();

        TextureBufferView1->Release();
        TextureBufferView0->Release();
        ConstantBuffer1->Release();
        ConstantBuffer0->Release();
        IndexBuffer->Release();
        TexcoordBuffer->Release();
        PositionBuffer->Release();
        DepthStencilBufferView->Release();
        BackBufferView->Release();
        SwapChain->Release();

        Context->Release();
        Device->Release();
#ifdef _DEBUG
        Debug->Release();
#endif
    }
}

void updateConstantBuffer(ID3D11Resource* buffer, void* data, size_t size)
{
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;
    Hr = Context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
    assert(SUCCEEDED(Hr));
    memcpy(mappedBuffer.pData, data, size);
    Context->Unmap(buffer, 0);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
}
