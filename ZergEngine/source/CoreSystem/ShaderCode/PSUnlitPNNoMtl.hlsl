#include "Lighting.hlsli"

// [Constant Buffer]
// cbuffer Cb0 : register(b0)
// {
//     CbPerForwardRenderingFrame cb_perFrame;
// }
// 
// cbuffer Cb1 : register(b1)
// {
//     CbPerCamera cb_perCamera;
// }
// 
// cbuffer Cb2 : register(b2)
// {
//     CbPerSubset cb_perSubset;
// }

PSOutput main(PSInputPNFragment input)
{
    PSOutput output;
    
    output.color = NO_MATERIAL_COLOR;
    
    return output;
}
