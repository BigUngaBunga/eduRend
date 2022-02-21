
Texture2D texDiffuse : register(t0);

SamplerState textureSampler : register(s0);

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
	bool directionalLight = false;
	bool debugTextureCoordinates = false;
	bool debugNormals = false;
	
    bool includeAmbient = kD.w == 1;
    //bool includeAmbient = true;
    bool includeDiffuse = true;
    bool includeSpecular = true;
	
	float3 ambientColour = kA.xyz;
	float3 diffuseColour = kD.xyz;
	float3 specularColour = kS.xyz;
    float shininess = kS.w;
    float4 outputColour = float4(0, 0, 0, 1);
	
    diffuseColour = texDiffuse.Sample(textureSampler, input.TexCoord).xyz;
	
    float ambientTextureStrength = 0.25f;
    float3 texturedAmbientColour = ambientColour * diffuseColour * ambientTextureStrength;
	
    float3 lightVector;
	
    if (directionalLight)
    { lightVector = lightPosition.xyz; }
    else
    { lightVector = lightPosition.xyz - input.WorldPosition.xyz; }
	
	//En ljuspunkt
    float3 normal = normalize(input.Normal);
    
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
        outputColour.xyz += texturedAmbientColour;
	if(includeDiffuse)
        outputColour.xyz += diffuseColour * lightStrength;
    if (includeSpecular)
        outputColour.xyz += specularColour * specularStrength * diffuseColour;
        

	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	if (debugNormals)
		outputColour = float4(input.Normal * 0.5 + 0.5, 1);

	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
	if (debugTextureCoordinates)
		outputColour = float4(input.TexCoord, 0, 1);

    if (kD.w < 1.0f)
        outputColour = float4(0.5f, 0.5f, 0.0f, 1.0f);
	
	return outputColour;
}