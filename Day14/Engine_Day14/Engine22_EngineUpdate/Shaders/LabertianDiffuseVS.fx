// 상수 버퍼 선언.
cbuffer CBPerObject : register(b0)
{
	matrix world;		// 월드 행렬.
	matrix view;		// 뷰 행렬.
	matrix projection;	// 투영 행렬.
};

// 상수 버퍼 - 라이트 정보.
cbuffer CBLight : register(b1)
{
	float3 worldLightPosition;
	float3 worldCameraPosition;
};

// 정점 입력 구조체.
struct VS_INPUT
{
	float4 pos : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;
};

Texture2D objTexture : register(t0);
SamplerState objSampler : register(s0);

// 정점 출력 구조체.
struct VS_OUTPUT
{	
	float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 diffuse : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	//output.pos = input.pos;

	// 공간 변환.
	// 월드 변환.
	output.pos = mul(input.pos, world);

	// 라이트 벡터 구하기.
	float3 lightDir = output.pos.xyz - worldLightPosition;
	// 단위 벡터로 만들기.
	lightDir = normalize(lightDir);

	// 뷰 변환.
	output.pos = mul(output.pos, view);
	// 투영 변환.
	output.pos = mul(output.pos, projection);

	// 정점에서 입력받은 텍스처 좌표 반환.
	output.texCoord = input.texCoord;

	// 월드 노멀 구하기.
	float3 worldNormal 
		= mul(input.normal, (float3x3)world);
	// 단위 벡터로 만들기.
	worldNormal = normalize(worldNormal);

	// 디퓨즈 계산 (dot = 내적).
	output.diffuse = dot(-lightDir, worldNormal) * 0.5f + 0.5f;

	return output;
}