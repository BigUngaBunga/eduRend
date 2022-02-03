
Texture2D texDiffuse : register(t0);

cbuffer LightAndCameraBuffer : register(b0)
{
	float4 lightPosition;
	float4 cameraPosition;
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	bool debugDirectionalLight = true;
	bool debugLambert = true;
	bool debugNormals = true;

	float3 difuseColour = { 0.5, 0.1, 0 };

	// Debug directional light
	if (debugDirectionalLight) {
		float3 lightVector = { lightPosition.xyz };
		float lightStrength = max(0, dot(normalize(lightVector), normalize(input.Normal)));
		float4 outputColour = float4(difuseColour * lightStrength, 1);
		return outputColour;
	}
		

	// Debug lambert
	if (debugLambert) {
		float3 lightVector = { lightPosition.x - input.Pos.x, lightPosition.y - input.Pos.y, lightPosition.z - input.Pos.z };
		float lightStrength = max(0, dot(normalize(lightVector), normalize(input.Normal)));
		float4 outputColour = float4(difuseColour * lightStrength, 1);
		return outputColour;
	}
		

	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	if(debugNormals)
		return float4(input.Normal*0.5+0.5, 1);

	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
	return float4(input.TexCoord, 0, 1);
}