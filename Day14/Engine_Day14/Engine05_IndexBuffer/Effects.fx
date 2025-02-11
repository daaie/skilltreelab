// 정점 입력 구조체.
struct VS_INPUT
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

// 정점 출력 구조체.
struct VS_OUTPUT
{	
	float4 pos : SV_POSITION;
	//float4 pos : POSITION;
	float4 color : COLOR;
};

VS_OUTPUT VS_Main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.pos = input.pos;
	output.color = input.color;

	return output;
}

float4 PS_Main(VS_OUTPUT psInput) : SV_TARGET
{
	//return float4(1.0f, 1.0f, 0.0f, 1.0f);
	return psInput.color;
}