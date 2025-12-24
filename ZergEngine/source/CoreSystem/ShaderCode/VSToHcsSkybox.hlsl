#include "ShaderCommon.hlsli"

/*
[Constant Buffer]
PerCamera
*/

cbuffer Cb0 : register(b0)
{
    CbPerCamera cb_perCamera;
}

#define LEFT_BOTTOM_FRONT   float3(-1.0f, -1.0f, +1.0f)
#define LEFT_TOP_FRONT      float3(-1.0f, +1.0f, +1.0f)
#define RIGHT_BOTTOM_FRONT  float3(+1.0f, -1.0f, +1.0f)
#define RIGHT_TOP_FRONT     float3(+1.0f, +1.0f, +1.0f)
#define LEFT_BOTTOM_BACK    float3(-1.0f, -1.0f, -1.0f)
#define LEFT_TOP_BACK       float3(-1.0f, +1.0f, -1.0f)
#define RIGHT_BOTTOM_BACK   float3(+1.0f, -1.0f, -1.0f)
#define RIGHT_TOP_BACK      float3(+1.0f, +1.0f, -1.0f)

// Inside facing box
// TRIANGLELIST!
static const float3 g_skyboxVertices[36] =
{
    LEFT_BOTTOM_FRONT,
    LEFT_TOP_FRONT,
    RIGHT_BOTTOM_FRONT,
    
    RIGHT_TOP_FRONT,
    RIGHT_BOTTOM_FRONT,
    LEFT_TOP_FRONT,
    // 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
    RIGHT_BOTTOM_FRONT,
    RIGHT_TOP_FRONT,
    RIGHT_BOTTOM_BACK,
    
    RIGHT_TOP_BACK,
    RIGHT_BOTTOM_BACK,
    RIGHT_TOP_FRONT,
    // 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
    RIGHT_BOTTOM_BACK,
    RIGHT_TOP_BACK,
    LEFT_BOTTOM_BACK,
    
    LEFT_TOP_BACK,
    LEFT_BOTTOM_BACK,
    RIGHT_TOP_BACK,
    // 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
    LEFT_BOTTOM_BACK,
    LEFT_TOP_BACK,
    LEFT_BOTTOM_FRONT,
    
    LEFT_TOP_FRONT,
    LEFT_BOTTOM_FRONT,
    LEFT_TOP_BACK,
    // 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
    LEFT_TOP_BACK,
    RIGHT_TOP_BACK,
    LEFT_TOP_FRONT,
    
    RIGHT_TOP_FRONT,
    LEFT_TOP_FRONT,
    RIGHT_TOP_BACK,
    // 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
    LEFT_BOTTOM_BACK,
    LEFT_BOTTOM_FRONT,
    RIGHT_BOTTOM_BACK,
    
    RIGHT_BOTTOM_FRONT,
    RIGHT_BOTTOM_BACK,
    LEFT_BOTTOM_FRONT
};

static const float4 g_identityR0 = { 1.0f, 0.0f, 0.0f, 0.0f };
static const float4 g_identityR1 = { 0.0f, 1.0f, 0.0f, 0.0f };
static const float4 g_identityR2 = { 0.0f, 0.0f, 1.0f, 0.0f };

PSInputSkyboxFragment main(uint vertexId : SV_VertexID)
{
    PSInputSkyboxFragment output;
    
    float4x4 w = float4x4(
        g_identityR0,
        g_identityR1,
        g_identityR2,
        float4(cb_perCamera.cameraPosW, 1.0f)
    );
    float4x4 wvp = mul(w, cb_perCamera.vp);
    
    float3 posL = g_skyboxVertices[vertexId];
    float4 posH = mul(float4(posL, 1.0f), wvp).xyww; // z = w ==> z / w = 1.0f ==> far plane縑 嬪纂
    output.pos = posH;
    output.texCoord = posL; // 煎鏽 漱攪蒂 斜渠煎 聽粽裘 價Ы葭 漱攪煎 餌辨
    
    return output;
}
