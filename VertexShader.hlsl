cbuffer b0 : register(b0)
{
    matrix Mat;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VS_OUT o;
    o.pos = mul(Mat, pos);
    o.uv = uv;
    return o;
}

//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//    return mul(Mat, pos);
//}