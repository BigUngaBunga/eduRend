
Texture2D texDiffuse : register(t0);

cbuffer LightAndCameraBuffer : register(b0)
{
	float4 lightPosition;
	float4 cameraPosition;
};

cbuffer MaterialBuffer : register(b1)
{
	float4 kA;
	float4 kD;
	float4 kS;
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float4 WorldPosition : WORD_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	bool debugDirectionalLight = false;
	bool debugTextureCoordinates = false;
	bool debugNormals = false;
	
	float shininess = 4;
	float3 ambientColour = kA.xyz;
	float3 difuseColour = kD.xyz;
	float3 specularColour = kS.xyz;
	float4 outputColour;
	
	//TODO kolla på hur vektorerna går, då ljuset reflekteras på motsatta sida också, där lambert blir mörk.

	//En ljuspunkt
	float4 lightVector = input.WorldPosition - lightPosition;
	float3 lightNormal = normalize(-lightVector).xyz;
	float4 cameraVector = cameraPosition - input.WorldPosition;
	float3 cameraNormal = normalize(cameraVector).xyz;
	float3 reflectionVector = lightVector.xyz - 2 * (dot(lightVector.xyz, input.Normal) * input.Normal);
	float3 reflectionNormal = normalize(reflectionVector);
	float reflectionAngle = dot(reflectionNormal, cameraNormal);
	
	float lightStrength = max(0, dot(lightNormal, input.Normal));
	float specularStrength = max(0, pow(reflectionAngle, shininess));
	outputColour = float4((ambientColour + difuseColour * lightStrength + specularColour * specularStrength).xyz, 1);
	
	// "solljus"
	if (debugDirectionalLight)
	{
		lightVector.xyz = lightPosition.xyz;
		
		outputColour = float4(difuseColour.xyz * lightStrength, 1);
	}

	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	if (debugNormals)
		outputColour = float4(input.Normal * 0.5 + 0.5, 1);

	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
	if (debugTextureCoordinates)
		outputColour = float4(input.TexCoord, 0, 1);

	return outputColour;
}