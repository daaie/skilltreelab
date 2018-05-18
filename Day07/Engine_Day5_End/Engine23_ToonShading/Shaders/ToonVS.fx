// ��� ���� ����.
cbuffer CBPerObject : register(b0)
{
	matrix world;		// ���� ���.
	matrix view;		// �� ���.
	matrix projection;	// ���� ���.
};

// ��� ���� - ����Ʈ ����.
cbuffer CBLight : register(b1)
{
	float3 worldLightPosition;
	float3 worldCameraPosition;
};

// ���� �Է� ����ü.
struct VS_INPUT
{
	float4 pos : POSITION;
	float2 texCoord : TEXCOORD; // �ؽ��ĸ������ϱ� �ʿ��ϴ�
	float3 normal : NORMAL; // ī���̶� ��� �ʿ���µ� �ϴ� �־������
	float3 binormal : BINORMAL;
	float3 tangent : TANGENT;
};

// ���� ��� ����ü.
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

	// ���� ��ȯ.
	// ���� ��ȯ.
	output.pos = mul(input.pos, world);

	// ����Ʈ ���� ���ϱ�.
	float3 lightDir = output.pos.xyz - worldLightPosition;
	// ���� ���ͷ� �����.
	lightDir = normalize(lightDir);

	// �� ��ȯ.
	output.pos = mul(output.pos, view);
	// ���� ��ȯ.
	output.pos = mul(output.pos, projection);

	// �������� �Է¹��� �ؽ�ó ��ǥ ��ȯ.
	output.texCoord = input.texCoord;

	// ���� ��� ���ϱ�.
	float3 worldNormal
		= mul(input.normal, (float3x3)world);
	// ���� ���ͷ� �����.
	worldNormal = normalize(worldNormal);
	
	// ��ǻ�� ��� (dot = ����).
	// output.diffuse = dot(-lightDir, worldNormal);
	output.diffuse = dot(-lightDir, worldNormal) * 0.5f + 0.5f;
	// ���� ����Ʈ -> �ֺ����� �� �����!!



	return output;
}