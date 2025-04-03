#include "ShaderCommon.hlsli"

#define LEFT_BOTTOM_FRONT   float3(-1.0f, -1.0f, +1.0f)
#define LEFT_TOP_FRONT      float3(-1.0f, +1.0f, +1.0f)
#define RIGHT_BOTTOM_FRONT  float3(+1.0f, -1.0f, +1.0f)
#define RIGHT_TOP_FRONT     float3(+1.0f, +1.0f, +1.0f)
#define LEFT_BOTTOM_BACK    float3(-1.0f, -1.0f, -1.0f)
#define LEFT_TOP_BACK       float3(-1.0f, +1.0f, -1.0f)
#define RIGHT_BOTTOM_BACK   float3(+1.0f, -1.0f, -1.0f)
#define RIGHT_TOP_BACK      float3(+1.0f, +1.0f, -1.0f)

PSInputSkyboxColoring main(uint vertexId : SV_VertexID)
{
    PSInputSkyboxColoring output;
    
    // Inside facing
    // TRIANGLELIST
    float3 skyboxVertices[36] =
    {
        LEFT_BOTTOM_FRONT,
        LEFT_TOP_FRONT,
        RIGHT_BOTTOM_FRONT,
        
        RIGHT_TOP_FRONT,
        RIGHT_BOTTOM_FRONT,
        LEFT_TOP_FRONT,
        
        
        
        RIGHT_BOTTOM_FRONT,
        RIGHT_TOP_FRONT,
        RIGHT_BOTTOM_BACK,
        
        RIGHT_TOP_BACK,
        RIGHT_BOTTOM_BACK,
        RIGHT_TOP_FRONT,
        
        
        
        RIGHT_BOTTOM_BACK,
        RIGHT_TOP_BACK,
        LEFT_BOTTOM_BACK,
        
        LEFT_TOP_BACK,
        LEFT_BOTTOM_BACK,
        RIGHT_TOP_BACK,
        
        
        
        LEFT_BOTTOM_BACK,
        LEFT_TOP_BACK,
        LEFT_BOTTOM_FRONT,
        
        LEFT_TOP_FRONT,
        LEFT_BOTTOM_FRONT,
        LEFT_TOP_BACK,
        
        
        
        LEFT_TOP_BACK,
        RIGHT_TOP_BACK,
        LEFT_TOP_FRONT,
        
        RIGHT_TOP_FRONT,
        LEFT_TOP_FRONT,
        RIGHT_TOP_BACK,
        
        
        
        LEFT_BOTTOM_BACK,
        LEFT_BOTTOM_FRONT,
        RIGHT_BOTTOM_BACK,
        
        RIGHT_BOTTOM_FRONT,
        RIGHT_BOTTOM_BACK,
        LEFT_BOTTOM_FRONT
    };
    
    output.posH = mul(float4(skyboxVertices[vertexId], 1.0f), cbpsky_wvp).xyww; // z = w ==> z / w = 1.0f ==> far plane¿¡ À§Ä¡
    output.posL = skyboxVertices[vertexId];
    
    return output;
}
