#include"WinMain.h"
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ INT)
{
    //ウィンドウ
    {
        //ウィンドウクラス登録
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
        //ウィンドウをつくって表示
        {
            //まず、表示位置、ウィンドウの大きさ調整
            RECT windowRect = { 0, 0, ClientWidth, ClientHeight };
            AdjustWindowRect(&windowRect, WindowStyle, FALSE);
            int windowLeft = ClientLeft + windowRect.left;
            int windowTop = ClientTop + windowRect.top;
            int windowWidth = windowRect.right - windowRect.left;
            int windowHeight = windowRect.bottom - windowRect.top;
            //つぎに、ウィンドウをつくる
            HWnd = CreateWindowEx(
                NULL,//拡張スタイルなし
                L"GAME_WINDOW",
                WindowTitle,
                WindowStyle,
                windowLeft,
                windowTop,
                windowWidth,
                windowHeight,
                NULL,//親ウィンドウなし
                NULL,//メニューなし
                hInstance,
                NULL//複数ウィンドウなし
            );
            //ウィンドウ表示
            ShowWindow(HWnd, SW_SHOW);
        }
    }
    //デバイス、コンテキスト
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
        //デバッグインターフェイス
        Hr = Device->QueryInterface(
            //__uuidof(ID3D11Debug),reinterpret_cast<void**>(&Debug)
            IID_PPV_ARGS(&Debug)
        );
        assert(SUCCEEDED(Hr));
#endif
    }
    //リソース
    {
        //スワップチェインをつくる(バックバッファを含んでいる)
        {
            //DXGIファクトリをつくる
            IDXGIFactory4* dxgiFactory;
            Hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
            assert(SUCCEEDED(Hr));

            //スワップチェインを記述してつくる
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

            //もういらない
            dxgiFactory->Release();
        }
        //バックバッファのビューをつくる
        {
            //スワップチェインからバックバッファを取得
            ID3D11Texture2D* backBuffer;
            Hr = SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
            assert(SUCCEEDED(Hr));

            //バックバッファのビューをつくる
            Hr = Device->CreateRenderTargetView(backBuffer, nullptr, &BackBufferView);
            assert(SUCCEEDED(Hr));

            //もういらない
            backBuffer->Release();
        }
        //デプスステンシルバッファのビューをつくる
        {
            //デプスステンシルバッファの記述
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
            //デプスステンシルバッファをつくる
            ID3D11Texture2D* depthStencilBuffer = 0;
            Hr = Device->CreateTexture2D(&desc, nullptr, &depthStencilBuffer);
            assert(SUCCEEDED(Hr));

            //デプスステンシルバッファのビューをつくる
            D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = desc.Format;
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            dsvDesc.Texture2D.MipSlice = 0;
            Hr = Device->CreateDepthStencilView(depthStencilBuffer, &dsvDesc, &DepthStencilBufferView);
            assert(SUCCEEDED(Hr));

            depthStencilBuffer->Release();
        }

        //データファイルを開く
        std::ifstream file("assets\\kame\\kame.txt");
        assert(!file.fail());

        //頂点バッファ-positions
        {
            //頂点位置データを読み込む
            std::string dataType;
            int num = 0;
            file >> dataType;
            assert(dataType == "positions");
            file >> num; //頂点数
            num *= 3; //１頂点データはｘｙｚの３つ。３倍するとnumは配列の要素数となる。
            std::vector<float> positions(num);
            for (int i = 0; i < num; i++) {
                file >> positions[i];
            }

            //バッファ記述
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(float) * num; //配列の全バイト数
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //頂点バッファとしてGPUに紐づけ
            //データ
            D3D11_SUBRESOURCE_DATA data = {};
            data.pSysMem = positions.data();
            //バッファを作成
            Hr = Device->CreateBuffer(&desc, &data, &PositionBuffer);
            assert(SUCCEEDED(Hr));
        }
        //頂点バッファ-texcoords
        {
            //頂点テクスチャ座標データを読み込む
            std::string dataType;
            int num = 0;
            file >> dataType;
            assert(dataType == "texcoords");
            file >> num;//頂点数
            num *= 2;//１ﾃｸｽﾁｬ座標はｘｙの２つ。２倍するとnumは配列の要素数となる。
            std::vector<float> texcoords(num);
            for (int i = 0; i < num; i++) {
                file >> texcoords[i];
            }

            //バッファ記述
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(float) * num;//配列の全長
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;   //頂点バッファとしてGPUに紐づけ
            //データの設定
            D3D11_SUBRESOURCE_DATA data{};
            data.pSysMem = texcoords.data();
            //バッファを作成
            Hr = Device->CreateBuffer(&desc, &data, &TexcoordBuffer);
            assert(SUCCEEDED(Hr));
        }
        //インデックスバッファ
        {
            //インデックスデータを読み込む
            std::string dataType;
            int num = 0;
            file >> dataType;
            assert(dataType == "indices");
            file >> num;//インデックスデータはこのまま配列の要素数となる。
            std::vector<unsigned short> indices(num);
            for (int i = 0; i < num; i++) {
                file >> indices[i];
            }

            //バッファの記述
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(unsigned short) * num;
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//インデックスバッファとしてGPUに紐づけ
            //データ
            D3D11_SUBRESOURCE_DATA data = {};
            data.pSysMem = indices.data();
            //バッファをつくる
            Hr = Device->CreateBuffer(&desc, &data, &IndexBuffer);
            assert(SUCCEEDED(Hr));
        }
        //コンスタントバッファ０「マトリックス用」入れ物だけつくる
        {
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(float) * 16;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//定数バッファとして紐づけ
            desc.Usage = D3D11_USAGE_DYNAMIC;//動的なバッファ
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPUから更新する
            Hr = Device->CreateBuffer(&desc, nullptr, &ConstantBuffer0);
            assert(SUCCEEDED(Hr));
        }
        //コンスタントバッファ１「ディフューズ用」入れ物だけつくる
        {
            D3D11_BUFFER_DESC desc = {};
            desc.ByteWidth = sizeof(float) * 4;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;//定数バッファとして紐づけ
            desc.Usage = D3D11_USAGE_DYNAMIC;//動的なバッファ
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPUから更新する
            Hr = Device->CreateBuffer(&desc, nullptr, &ConstantBuffer1);
            assert(SUCCEEDED(Hr));
        }
        //テクスチャバッファビューをつくる
        {
            //テクスチャファイル名を読み込む
            std::string dataType;
            file >> dataType;
            assert(dataType == "texture");
            std::string filename;
            file >> filename;

            //テクスチャデータをファイルから読み込む
            unsigned char* pixels = nullptr;
            int texWidth = 0;
            int texHeight = 0;
            int bytePerPixel = 4;
            pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, nullptr, bytePerPixel);
            assert(pixels != nullptr);

            //テクスチャバッファ記述
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
            //テクスチャデータ
            D3D11_SUBRESOURCE_DATA data = {};
            data.pSysMem = pixels;
            data.SysMemPitch = texWidth * bytePerPixel;
            //テクスチャバッファをつくる
            ID3D11Texture2D* textureBuffer = nullptr;
            Hr = Device->CreateTexture2D(&desc, &data, &textureBuffer);
            assert(SUCCEEDED(Hr));

            //テクスチャバッファのビューをつくる
            Hr = Device->CreateShaderResourceView(textureBuffer, nullptr, &TextureBufferView);
            assert(SUCCEEDED(Hr));

            //解放
            textureBuffer->Release();
            stbi_image_free(pixels);
        }
    }{}
    //パイプライン
    {
        //インプットレイアウト と シェーダー
        {
            //頂点インプットレイアウトを定義し、サイズを求めておく
            UINT slot0 = 0, slot1 = 1;
            D3D11_INPUT_ELEMENT_DESC Elements[]{
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, slot0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    slot1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
            UINT numElements = sizeof(Elements) / sizeof(Elements[0]);

            //シェーダーバイトコードの読み込み
            BIN_FILE11 vs("assets\\cso\\VertexShader.cso");
            assert(vs.succeeded());
            BIN_FILE11 ps("assets\\cso\\PixelShader.cso");
            assert(ps.succeeded());

            //頂点インプットレイアウトをつくる
            Hr = Device->CreateInputLayout(Elements, numElements,
                vs.code(), vs.size(), &InputLayout);
            assert(SUCCEEDED(Hr));

            //頂点シェーダをつくる
            Hr = Device->CreateVertexShader(vs.code(), vs.size(), NULL, &VertexShader);
            assert(SUCCEEDED(Hr));

            //ピクセルシェーダーをつくる
            Hr = Device->CreatePixelShader(ps.code(), ps.size(), NULL, &PixelShader);
            assert(SUCCEEDED(Hr));
        }
        //ラスタライザーステート
        {
            D3D11_RASTERIZER_DESC desc;
            desc.FrontCounterClockwise = TRUE;
            desc.CullMode = D3D11_CULL_BACK;
            //desc.CullMode = D3D11_CULL_NONE;
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
        //表示領域を作成
        {
            Viewport.Width = static_cast<float>(ClientWidth);
            Viewport.Height = static_cast<float>(ClientHeight);
            Viewport.TopLeftX = 0;
            Viewport.TopLeftY = 0;
            Viewport.MinDepth = D3D11_MIN_DEPTH;// 0.0f
            Viewport.MaxDepth = D3D11_MAX_DEPTH;// 1.0f
        }
        //サンプラステート
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
        //ブレンドステート
        {
            D3D11_BLEND_DESC desc = {};
            desc.AlphaToCoverageEnable = TRUE;
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
        //デプスステンシルステート
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

    //メインループ
    while (true)
    {
        //ウィンドウメッセージの取得、送出
        {
            MSG msg = { 0 };
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    break;
                }
                DispatchMessage(&msg);
                continue;
            }
        }
        //バックバッファのクリア
        {
            //ブレンドステートをセット
            Context->OMSetBlendState(BlendState, nullptr, 0xffffffff);
            //デプスステンシルステートをセット
            Context->OMSetDepthStencilState(DepthStencilState, 0);
            //バックバッファとデプスステンシルバッファをレンダーターゲットに設定
            Context->OMSetRenderTargets(1, &BackBufferView, DepthStencilBufferView);

            //レンダーターゲットビューを指定した色でクリア
            FLOAT clearColor[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
            Context->ClearRenderTargetView(BackBufferView, clearColor);
            //デプスステンシルバッファを1.0でクリア
            Context->ClearDepthStencilView(DepthStencilBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        }
        //バックバッファへ描画
        {
            //頂点インプットレイアウトをセット
            Context->IASetInputLayout(InputLayout);
            //トポロジーをセット
            Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            //頂点バッファをまとめてからセット
            ID3D11Buffer* buffers[] = { PositionBuffer, TexcoordBuffer };
            UINT stride[] = { sizeof(XMFLOAT3),sizeof(XMFLOAT2) };
            UINT offset[] = { 0,0 };
            Context->IASetVertexBuffers(0, _countof(buffers), buffers, stride, offset);
            //インデックスバッファをセット
            Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

            //頂点シェーダをセット
            Context->VSSetShader(VertexShader, nullptr, 0);
            //コンスタントバッファ０をセット
            UINT b0 = 0;
            Context->VSSetConstantBuffers(b0, 1, &ConstantBuffer0);

            //ラスタライザーをセット
            Context->RSSetState(RasterizerState);
            //ビューポートをセット
            Context->RSSetViewports(1, &Viewport);

            //ピクセルシェーダをセット
            Context->PSSetShader(PixelShader, nullptr, 0);
            //コンスタントバッファ１をセット
            UINT b1 = 1;
            Context->PSSetConstantBuffers(b1, 1, &ConstantBuffer1);
            //サンプラーをセット
            UINT s0 = 0;
            Context->PSSetSamplers(s0, 1, &SamplerState);

            //コンスタントバッファ０を更新
            static float radian = 0.0f;
            radian += 0.01f;
            XMVECTOR eye = { 0, 3.0f, -10.f }, focus = { 0, 3.0f, 0 }, up = { 0, 1, 0 };
            XMMATRIX view = XMMatrixLookAtLH(eye, focus, up);
            float angle = 3.14f / 4, aspect = ClientWidth / (float)ClientHeight;
            float near_ = 1.0f, far_ = 21.0f;
            XMMATRIX proj = XMMatrixPerspectiveFovLH(angle, aspect, near_, far_);
            XMMATRIX world = XMMatrixRotationY(radian);
            XMMATRIX mat = world * view * proj;
            updateConstantBuffer(ConstantBuffer0, &mat, sizeof(mat));

            //コンスタントバッファ１を更新
            XMFLOAT4 diffuse = { 1.0f,1.0f,1.0f,1.0f };
            updateConstantBuffer(ConstantBuffer1, &diffuse, sizeof(diffuse));

            //テクスチャセット
            UINT t0 = 0;
            Context->PSSetShaderResources(t0, 1, &TextureBufferView);
#if 0
            //インデックスを使わない描画（頂点が、すでにポリゴンごとに並んでいる）
            D3D11_BUFFER_DESC desc;
            PositionBuffer->GetDesc(&desc);
            UINT numVertices = desc.ByteWidth / (sizeof(float) * 3);
            Context->Draw(numVertices, 0);
#else
            //インデックスで描画
            D3D11_BUFFER_DESC desc;
            IndexBuffer->GetDesc(&desc);
            UINT numIndices = desc.ByteWidth / 2;
            Context->DrawIndexed(numIndices, 0, 0);
#endif
        }
        //バックバッファの表示
        {
            SwapChain->Present(1, 0);
        }
    }

    //解放
    {
        //パイプライン
        DepthStencilState->Release();
        BlendState->Release();
        SamplerState->Release();
        RasterizerState->Release();
        VertexShader->Release();
        PixelShader->Release();
        InputLayout->Release();
        //リソース
        TextureBufferView->Release();
        ConstantBuffer1->Release();
        ConstantBuffer0->Release();
        IndexBuffer->Release();
        TexcoordBuffer->Release();
        PositionBuffer->Release();
        DepthStencilBufferView->Release();
        BackBufferView->Release();
        SwapChain->Release();
        //デバイス・コンテキスト
        Context->Release();
        Device->Release();

#ifdef _DEBUG
        //Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        //Debug->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL);
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
