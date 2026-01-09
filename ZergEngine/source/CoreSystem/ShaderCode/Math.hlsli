// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Helper Functions
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

float3 ComputeNormalMapping(float3 normalW, float3 tangentW, float3 normalT)
{
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
    ); // Tangent -> World
    
    // return normalize(mul(normalT, tbn)); // 노말맵 샘플을 월드 탄젠트 스페이스에서 월드 스페이스로
    return mul(normalT, tbn); // 노말맵 샘플을 월드 탄젠트 스페이스에서 월드 스페이스로
}

// +Z Forward Quad를 뒤집어 UI 쿼드로 사용할 때 유용한 함수.
float3 FlipBasedOnYAxis(float3 v)
{
    v.x = -v.x;
    v.z = -v.z;
	
    return v;
}
