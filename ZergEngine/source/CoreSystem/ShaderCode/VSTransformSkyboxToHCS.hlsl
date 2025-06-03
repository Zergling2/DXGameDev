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

PSInputSkyboxFragment main(uint vertexId : SV_VertexID)
{
    PSInputSkyboxFragment output;
    
    float4x4 w = float4x4(
        float4(1.0f, 0.0f, 0.0f, 0.0f),
        float4(0.0f, 1.0f, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(cb_perCamera.cameraPosW, 1.0f)
    );
    float4x4 wvp = mul(w, cb_perCamera.vp);
    
    output.posH = mul(float4(g_skyboxVertices[vertexId], 1.0f), wvp).xyww; // z = w ==> z / w = 1.0f ==> far plane縑 嬪纂
    output.posL = g_skyboxVertices[vertexId];
    
    return output;
}
