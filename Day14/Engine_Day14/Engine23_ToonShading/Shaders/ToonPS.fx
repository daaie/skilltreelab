Texture2D objTexture : register(t0);
SamplerState objSampler : register(s0);

// 픽셀 입력 구조체.
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 diffuse : TEXCOORD1;
};

float4 main(PS_INPUT psInput) : SV_TARGET
{
	float3 albedo = objTexture.Sample(objSampler, psInput.texCoord).rgb;

	// 0이하 자르기.
	float3 diffuse = saturate(psInput.diffuse);

	float toonStep = 3.0f;

	// 툰 셰이딩 계산.
	diffuse = ceil(diffuse * toonStep) / toonStep;

	// 텍스처에서 읽어온 색상 값 적용.
	diffuse *= albedo;

	// 주변광.
	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	// 최종 색상.
	//return float4(diffuse + ambient, 1);
	return float4(diffuse, 1);
}