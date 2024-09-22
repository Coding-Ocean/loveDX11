cbuffer b1 : register(b1)
{
    float4 Diffuse;
};
Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PS_IN i) : SV_TARGET
{
    return Texture.Sample(Sampler, i.uv) * Diffuse;
}
