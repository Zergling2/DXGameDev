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
            // �븻 �����κ��� �븻 ���ø� �� ���� �������� ��ȯ
            // �����Ͷ����� �������� ���� ������ȭ �Ǿ��� �� �����Ƿ� �븻�� �ٽ� ����ȭ
            input.normalW = normalize(input.normalW);
            input.tangentW = input.tangentW - dot(input.tangentW, input.normalW) * input.normalW;
            float3 bitangentW = cross(input.normalW, input.tangentW);
    
            // Local -> Tangent ����� ������� �����. �������� T, B, N�� column major�� ��ġ�ϸ� Local -> Tangent ����� �Ǵµ�
            // T, B, N�� ��������̹Ƿ� �ܼ��� ��ġ���Ѽ� row major�� ��ġ��Ű�� �͸����� ������� Tangent -> Local ����� ���� �� �ִ�.
            // �׷��� ��� T, B, N�� ���ؽ� ���̴����� ���� �������� ��ȯ�Ǿ����� �Ѿ�����Ƿ� �̰� Tangent -> World ����� �ȴ�.
            float3x3 tbn = float3x3(input.tangentW, bitangentW, input.normalW);         // Tangent -> World
    
            float3 normalT = tex2d_normalMap.Sample(ss_normalMap, input.texCoord).rgb * 2.0f - 1.0f; // [0, 1] -> [-1, 1]
            normalW = normalize(mul(normalT, tbn)); // �븻�� ������ ���� ź��Ʈ �����̽����� ���� �����̽���
            // normalW = mul(normalT, tbn); // �븻�� ������ ���� ź��Ʈ �����̽����� ���� �����̽���
        }
        else
        {
            normalW = input.normalW;
        }
        
        // ������ ���
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
    
        output.color.a = cb_perSubset.mtl.diffuse.a; // ���� ���� ������ diffuse �Ӽ����� ���� (tr map �̻���..)
        output.color = saturate(output.color);
    }
    else
    {
        // ���� ����
        output.color = NO_MATERIAL_COLOR;
    }

    return output;
}
