
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
	
    bool includeAmbient = true;
    bool includeDiffuse = true;
    bool includeSpecular = true;
	
	float3 ambientColour = kA.xyz;
	float3 difuseColour = kD.xyz;
	float3 specularColour = kS.xyz;
    float shininess = kS.w;
    float4 outputColour = float4(0, 0, 0, 1);
	
	//TODO kolla på hur vektorerna går, då ljuset reflekteras på motsatta sida också, där lambert blir mörk.

	//En ljuspunkt
    float3 normal = normalize(input.Normal);
    float3 lightVector = lightPosition.xyz - input.WorldPosition.xyz;
	float3 lightNormal = normalize(lightVector);
    float3 cameraVector = cameraPosition.xyz - input.WorldPosition.xyz;
	float3 cameraNormal = normalize(cameraVector);
    float3 reflectionVector = lightVector - 2 * (dot(lightVector.xyz, normal) * normal);
	float3 reflectionNormal = normalize(reflectionVector);
	float reflectionAngle = -dot(reflectionNormal, cameraNormal);
    reflectionAngle = max(0, reflectionAngle);
	
    float lightStrength = max(0, dot(lightNormal, normal));
    float specularStrength = pow(reflectionAngle, shininess);
	
    if (includeAmbient)
        outputColour.xyz += ambientColour;
	if(includeDiffuse)
        outputColour.xyz += difuseColour * lightStrength;
	if(includeSpecular)
        outputColour.xyz += specularColour * specularStrength;
    //outputColour = float4((ambientColour + difuseColour * lightStrength + specularColour * specularStrength).xyz, 1);
	
	// "solljus"
	if (debugDirectionalLight)
	{
		lightVector = lightPosition.xyz;
        lightNormal = normalize(lightVector);
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