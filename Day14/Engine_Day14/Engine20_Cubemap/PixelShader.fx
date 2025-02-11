// 픽셀 입력 구조체.
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 lightDir : TEXCOORD1;
	float3 viewDir : TEXCOORD2;
	//float3 reflection : TEXCOORD3;

	// 추가.
	float3 T : TEXCOORD3;
	float3 B : TEXCOORD4;
	float3 N : TEXCOORD5;
};

Texture2D objTexture : register(t0);
Texture2D normalTexture : register(t1);
// 큐브맵 추가.
TextureCube cubemapTexture : register(t2);
SamplerState objSampler : register(s0);

// 픽셀 쉐이더 함수.
float4 main(PS_INPUT input) : SV_TARGET
{
	// 탄젠트 노멀 값 읽어오기 (노멀맵 텍스처에서).
	float3 tangentNormal = normalTexture.Sample(objSampler, input.uv);
	
	// 노멀 값 계산.
	tangentNormal = normalize(tangentNormal * 2.0f -1.0f);

	// 월드 공간 변환 행렬 만들기.
	float3x3 TBN = float3x3(normalize(input.T), normalize(input.B), normalize(input.N));

	// 월드 노멀 구하기.
	float3 worldNormal = normalize(mul(tangentNormal, TBN));

	float3 lightDir = normalize(input.lightDir);

	// 난반사(디퓨즈) 구하기 / 0이하 값 자르기.
	float3 diffuse = saturate(dot(worldNormal, -lightDir));

	// 디퓨즈맵 읽어오기.
	float3 albedo = objTexture.Sample(objSampler, input.uv);
	// 디퓨즈 빛의 세기와 디퓨즈맵 값 곱하기.
	diffuse = diffuse * albedo;

	// 정반사(스페큘러).
	float3 specular = 0;
	// 카메라 방향벡터.
	float3 viewDir = normalize(input.viewDir);
	if (diffuse.x > 0)
	{
		// 반사 벡터 구하기.
		float3 reflection = reflect(lightDir, worldNormal);

		// 반사 벡터와 뷰벡터의 각도 구하기.
		specular = dot(reflection, -viewDir);
		
		// 0 - 1 사이의 값으로 고정 / 영역 줄이고 빛 강도 증폭.
		specular = pow(saturate(specular), 20.0f);
	}

	// 주변광(ambient).
	float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo;

	// 카메라 벡터의 반사 벡터.
	float3 viewReflect = reflect(viewDir, worldNormal);

	// 큐브맵.
	float3 environment = cubemapTexture.Sample(objSampler, viewReflect).rgb;

	// 최종 색상.
	return float4(diffuse + specular + ambient + environment * 0.5f, 1.0f);
	//return float4(environment, 1.0f);
}