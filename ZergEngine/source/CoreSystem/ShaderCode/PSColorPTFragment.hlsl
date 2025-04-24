#include "ShaderCommon.hlsli"

/*
[Sampler State]
Mesh Texture Sampler

[Texture]
LightMap t0
DiffuseMap t1
NormalMap t2
SpecularMap t3

[Constant Buffer]
PerSubset
*/

cbuffer Cb0 : register(b0)
{
    CbPerSubset cb_perSubset;
}

Texture2D tex2d_lightMap : register(t0);
Texture2D tex2d_diffuseMap : register(t1);
Texture2D tex2d_normalMap : register(t2);
Texture2D tex2d_specularMap : register(t3);

PSOutput main(PSInputPTFragment input)
{
    PSOutput output;
    
    // ������ ��� X
    // Diffuse �ؽ����� ���ø� ���� �״�� ���
    if (IsUsingMaterial(cb_perSubset.mtl.mtlFlag) && IsUsingDiffuseMap(cb_perSubset.mtl.mtlFlag))
        output.color = tex2d_diffuseMap.Sample(ss_meshTexSampler, input.texCoord);
    else    // ���� ����
        output.color = NO_MATERIAL_COLOR;
    
    return output;
}
