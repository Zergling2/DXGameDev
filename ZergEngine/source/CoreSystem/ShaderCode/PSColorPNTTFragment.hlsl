#include "Lighting.hlsli"

// [Sampler State]
// ss_common

// [Constant Buffer]
cbuffer Cb0 : register(b0)
{
    CbPerFrame cb_perFrame;
}

cbuffer Cb1 : register(b1)
{
    CbPerCamera cb_perCamera;
}

cbuffer Cb2 : register(b2)
{
    CbPerSubset cb_perSubset;
}

// [Texture]
Texture2D tex2d_diffuseMap : register(t0);
Texture2D tex2d_specularMap : register(t1);
Texture2D tex2d_normalMap : register(t2);

PSOutput main(PSInputPNTTFragment input)
{
    PSOutput output;
    
    if (IsUsingMaterial(cb_perSubset.mtl.mtlFlag))
    {
        float3 normalW;
        
        if (IsUsingNormalMap(cb_perSubset.mtl.mtlFlag))
        {
            // 노말 맵으로부터 노말 샘플링 및 월드 공간으로 변환
            // 래스터라이저 보간으로 인해 비정규화 되었을 수 있으므로 노말을 다시 정규화
            input.normalW = normalize(input.normalW);
            input.tangentW = input.tangentW - dot(input.tangentW, input.normalW) * input.normalW;
            float3 bitangentW = cross(input.normalW, input.tangentW);
    
            // Local -> Tangent 행렬의 역행렬을 만든다. 기저벡터 T, B, N을 column major로 배치하면 Local -> Tangent 행렬이 되는데
            // T, B, N은 직교행렬이므로 단순히 전치시켜서 row major로 배치시키는 것만으로 역행렬인 Tangent -> Local 행렬을 구할 수 있다.
            // 그런데 사실 T, B, N이 버텍스 셰이더에서 월드 공간으로 변환되어져서 넘어왔으므로 이건 Tangent -> World 행렬이 된다.
            float3x3 tbn = float3x3(
                input.tangentW,
                bitangentW,
                input.normalW
            );         // Tangent -> World
    
            float3 normalT = tex2d_normalMap.Sample(ss_normalMap, input.texCoord).rgb * 2.0f - 1.0f; // [0, 1] -> [-1, 1]
            normalW = normalize(mul(normalT, tbn)); // 노말맵 샘플을 월드 탄젠트 스페이스에서 월드 스페이스로
            // normalW = mul(normalT, tbn); // 노말맵 샘플을 월드 탄젠트 스페이스에서 월드 스페이스로
        }
        else
        {
            normalW = input.normalW;
        }
        
        // 라이팅 계산
        const float3 toEyeW = normalize(cb_perCamera.cameraPosW - input.posW);
    
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
        float4 addA, addD, addS;
        uint i;
        for (i = 0; i < cb_perFrame.dlCount; ++i)
        {
            ComputeDirectionalLight(cb_perFrame.dl[i], cb_perSubset.mtl, normalW, toEyeW, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cb_perFrame.plCount; ++i)
        {
            ComputePointLight(cb_perFrame.pl[i], cb_perSubset.mtl, input.posW, normalW, toEyeW, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
    
        for (i = 0; i < cb_perFrame.slCount; ++i)
        {
            ComputeSpotLight(cb_perFrame.sl[i], cb_perSubset.mtl, input.posW, normalW, toEyeW, addA, addD, addS);
            ambient += addA;
            diffuse += addD;
            specular += addS;
        }
        
        if (IsUsingDiffuseMap(cb_perSubset.mtl.mtlFlag))
            output.color = tex2d_diffuseMap.Sample(ss_common, input.texCoord) * (ambient + diffuse) + specular;
        else
            output.color = (ambient + diffuse) + specular;
    
        output.color.a = cb_perSubset.mtl.diffuse.a; // 알파 값은 재질의 diffuse 속성에서 추출 (tr map 미사용시..)
        output.color = saturate(output.color);
    }
    else
    {
        // 재질 없음
        output.color = NO_MATERIAL_COLOR;
    }

    return output;
}
