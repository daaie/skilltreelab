// 상수 버퍼 선언.
cbuffer CBPerObject
{
	matrix world;		// 월드 행렬.
	matrix view;		// 뷰 행렬.
	matrix projection;	// 투영 행렬.
};

// 상수 버퍼 - 라이트 정보.
cbuffer CBLight
{
	float3 worldLightPosition;
	float3 worldCameraPosition;
};

// 정점 입력 구조체.
struct VS_INPUT
{
	float4 pos : POSITION;
	//float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
};

Texture2D objTexture;
SamplerState objSampler;

// 정점 출력 구조체.
struct VS_OUTPUT
{	
	float4 pos : SV_POSITION;
	//float2 texCoord : TEXCOORD;
	float3 diffuse : TEXCOORD1;

	float3 viewDir : TEXCOORD2;
	float3 reflection : TEXCOORD3;
};

VS_OUTPUT VS_Main(VS_INPUT input)
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

	// 뷰 벡터 구하기 (단위벡터 처리).
	float3 viewDir = normalize(output.pos.xyz - worldCameraPosition);
	output.viewDir = viewDir;

	// 뷰 변환.
	output.pos = mul(output.pos, view);
	// 투영 변환.
	output.pos = mul(output.pos, projection);

	// 정점에서 입력받은 텍스처 좌표 반환.
	//output.texCoord = input.texCoord;

	// 월드 노멀 구하기.
	float3 worldNormal 
		= mul(input.normal, (float3x3)world);
	// 단위 벡터로 만들기.
	worldNormal = normalize(worldNormal);

	// 디퓨즈 계산 (dot = 내적).
	output.diffuse = dot(-lightDir, worldNormal);

	// 반사 벡터 구하기 (reflect = 반사광 구하는 함수).
	//output.reflection = reflect(lightDir, worldNormal);

	// R = P +  2n(-P·n).
	output.reflection = lightDir + 2.0f * worldNormal * dot(-lightDir, worldNormal);

	return output;
}

float4 PS_Main(VS_OUTPUT psInput) : SV_TARGET
{
	// 0이하 자르기.
	float3 diffuse = saturate(psInput.diffuse);

	// 보간기를 거친 값을 다시 정렬.
	float3 reflection = normalize(psInput.reflection);
	float3 viewDir = normalize(psInput.viewDir);
	float3 specular = 0;

	if (diffuse.x > 0)
	{
		// 반사광과 뷰 벡터 간의 내적 구하기.
		specular = dot(reflection, -viewDir);
		// 0-1 사이로 값 고정.
		specular = saturate(specular);
		// 영역 줄이고 빛 증폭 시키기.
		specular = pow(specular, 20.0f);
	}

	// 주변광.
	float3 ambient = float3(0.1f, 0.1f, 0.1f);

	// 난반사광(디퓨즈) 색상.
	float3 diffuseColor = float3(1.0f, 1.0f, 1.0f);

	// 최종 색상.
	return float4(diffuseColor * diffuse + ambient + specular, 1);
	//return float4(specular, 1);
}