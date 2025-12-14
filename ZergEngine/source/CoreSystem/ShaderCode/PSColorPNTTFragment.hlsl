#include "Lighting.hlsli"

// [Sampler State]
// ss_common

// [Constant Buffer]
cbuffer Cb0 : register(b0)
{
    CbPerForwardRenderingFrame cb_perFrame;
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
        // 래스터라이저 보간으로 인해 비정규화 되었을 수 있으므로 노말을 다시 정규화
        float3 normalW = normalize(input.normalW);
        
        if (IsUsingNormalMap(cb_perSubset.mtl.mtlFlag))
        {
            // 노말 맵으로부터 노말 샘플링 및 월드 공간으로 변환
            float3 tangentW = input.tangentW - dot(input.tangentW, normalW) * normalW;

            // T, B, N이 모두 정규화된 벡터일 경우
            // T x B = N
            // B x N = T
            // N x T = B
            // 이 성립한다.
            // (T x B = N, B x N = T, N x T = B)
            float3 bitangentW = cross(normalW, tangentW);
            
            // input에 들어온 Normal과 Tangent는 월드 공간 기준의 벡터이다. (버텍스 셰이더에서 월드 변환시켜서 넘어옴.)
            // 따라서 T, B, N을 열으로 배치하여 행렬
            // Tx Bx Nx
            // Ty By Ny
            // Tz Bz Nz
            // 을 만들면 이 행렬은 (월드 공간 -> 탄젠트 공간) 변환 행렬이 된다.
            
            // 그런데 지금 필요한 것은 노말 텍스쳐에서 샘플링한 벡터(탄젠트 공간에서의 노말)를 월드 공간으로 변환하는 것이므로
            // 위의 행렬의 역행렬이 필요하다.
            // 그런데 위 행렬은 정규직교 행렬이었으므로 전치하는 것만으로 빠르게 역행렬을 구할 수 있다.
            
            // 따라서 위 행렬을 전치시킨 행렬
            // Tx Ty Tz
            // Bx By Bz
            // Nx Ny Nz
            // 는 (탄젠트 공간 -> 월드 공간) 변환 행렬이 된다.
            float3x3 tbn = float3x3(
                tangentW,
                bitangentW,
                normalW
            );         // Tangent -> World
    
            float3 normalT = tex2d_normalMap.Sample(ss_normalMap, input.texCoord).rgb * 2.0f - 1.0f; // [0, 1] -> [-1, 1]
            // normalW = normalize(mul(normalT, tbn)); // 노말맵 샘플을 월드 탄젠트 스페이스에서 월드 스페이스로
            normalW = mul(normalT, tbn); // 노말맵 샘플을 월드 탄젠트 스페이스에서 월드 스페이스로
        }
        
        // 라이팅 계산
        const float3 toEyeW = normalize(cb_perCamera.cameraPosW - input.posW);
    
        float4 ambientLight = float4(cb_perFrame.ambientLight, 0.0f);
        float4 diffuseLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specularLight = float4(0.0f, 0.0f, 0.0f, 0.0f);

        uint i;
        float4 oDL;
        float4 oSL;
        for (i = 0; i < cb_perFrame.dlCount; ++i)
        {
            ComputeDirectionalLight(cb_perFrame.dl[i], cb_perSubset.mtl.specular.w, normalW, toEyeW, oDL, oSL);
            diffuseLight += oDL;
            specularLight += oSL;
        }
    
        for (i = 0; i < cb_perFrame.plCount; ++i)
        {
            ComputePointLight(cb_perFrame.pl[i], cb_perSubset.mtl.specular.w, input.posW, normalW, toEyeW, oDL, oSL);
            diffuseLight += oDL;
            specularLight += oSL;
        }
    
        for (i = 0; i < cb_perFrame.slCount; ++i)
        {
            ComputeSpotLight(cb_perFrame.sl[i], cb_perSubset.mtl.specular.w, input.posW, normalW, toEyeW, oDL, oSL);
            diffuseLight += oDL;
            specularLight += oSL;
        }
    
        float4 diffuse = cb_perSubset.mtl.diffuse * (diffuseLight + ambientLight);
        float4 specular = cb_perSubset.mtl.specular * specularLight;
        
        if (IsUsingDiffuseMap(cb_perSubset.mtl.mtlFlag))
            diffuse *= tex2d_diffuseMap.Sample(ss_common, input.texCoord);

        output.color.rgb = (diffuse + specular).rgb;
        output.color.a = diffuse.a; // 알파 채널은 diffuse 속성에서 가져온다.

        output.color = saturate(output.color);
    }
    else
    {
        // 재질 없음
        output.color = NO_MATERIAL_COLOR;
    }
    
    return output;
}
