
Texture2D texDiffuse : register(t0);
Texture2D normalMap : register(t1);

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

cbuffer LightSources : register(b2)
{
    int numberOfLights;
    float4 lightPositions[8];
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float4 WorldPosition : WORD_POSITION;
	float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float DecodeNormal(float value) { return value * 2 - 1; }

float3 GetNormalFromNormalMap(float2 textureCoordinate)
{
    float3 encodedNormal = normalMap.Sample(textureSampler, textureCoordinate).xyz;
    float3 normal = float3(DecodeNormal(encodedNormal.x), DecodeNormal(encodedNormal.y), DecodeNormal(encodedNormal.z));
    return normalize(normal);
}

float3 CalculateNormal(float3 normal) { return normalize(normal);}

float3 CalculateNormal(float3 tangent, float3 binormal, float3 normal, float2 textureCoordinate)
{
    float3 localNormal = GetNormalFromNormalMap(textureCoordinate);
    float3x3 tbnMatrix = transpose(float3x3(tangent, binormal, normal));
    return normalize(mul(tbnMatrix, localNormal));
}

float CalculateSpecularStrength(float3 lightVector, float3 cameraVector, float3 normal, float shininess)
{
    float3 cameraNormal = normalize(cameraVector);
    float3 reflectionVector = reflect(lightVector, normal);
    float3 reflectionNormal = normalize(reflectionVector);
    float reflectionAngle = max(0, -dot(reflectionNormal, cameraNormal));
	
    return pow(reflectionAngle, shininess);
}

float4 PS_main(PSIn input) : SV_Target
{
	bool directionalLight = false;
	bool debugTextureCoordinates = false;
    float debugNormals = 0; //0 inget, 1 normaler, 2 tangenter, 3 binormaler;
	
    bool includeAmbient = true;
    bool includeDiffuse = true;
    bool includeSpecular = true;
	
    bool manyLightSources = false;
    
	float3 ambientColour = kA.xyz;
	float3 diffuseColour = kD.xyz;
	float3 specularColour = kS.xyz;
    float shininess = kS.w;
    bool hasNormalMap = kD.w;
    float4 outputColour = float4(0, 0, 0, 1);
	
    diffuseColour = texDiffuse.Sample(textureSampler, input.TexCoord).xyz;
	
    float ambientTextureStrength = 0.25f;
    float3 texturedAmbientColour = ambientColour * diffuseColour * ambientTextureStrength;
	
    float3 lightVector = directionalLight ? lightPosition.xyz : lightPosition.xyz - input.WorldPosition.xyz;
    float3 normal = hasNormalMap ? CalculateNormal(input.Tangent, input.Binormal, input.Normal, input.TexCoord) : CalculateNormal(input.Normal);
	float3 lightNormal = normalize(lightVector);
    float3 cameraVector = cameraPosition.xyz - input.WorldPosition.xyz;
    float lightStrength = max(0, dot(lightNormal, normal));;
    float specularStrength = CalculateSpecularStrength(lightVector, cameraVector, normal, shininess);
    
    //if (manyLightSources)
    //{
    //    for (int i = 0; i < numberOfLights; ++i)
    //    {
    //        lightVector = lightPositions[i].xyz - input.WorldPosition.xyz;
    //        lightStrength += max(0, dot(normalize(lightVector), normal));
    //        specularStrength += CalculateSpecularStrength(lightVector, cameraVector, normal, shininess);
    //    }
    //}
    //else
    //{
    //    lightStrength = max(0, dot(lightNormal, normal));
    //    specularStrength = CalculateSpecularStrength(lightVector, cameraVector, normal, shininess);
    //}
    
	
    if (includeAmbient)
        outputColour.xyz += texturedAmbientColour;
	if(includeDiffuse)
        outputColour.xyz += diffuseColour * lightStrength;
    if (includeSpecular)
        outputColour.xyz += specularColour * specularStrength * diffuseColour;

	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
	if (debugNormals == 1)
		outputColour = float4(normal * 0.5 + 0.5, 1);
    else if (debugNormals == 2)
        outputColour = float4(input.Tangent * 0.5 + 0.5, 1);
    else if (debugNormals == 3)
        outputColour = float4(input.Binormal * 0.5 + 0.5, 1);
    
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
	if (debugTextureCoordinates)
		outputColour = float4(input.TexCoord, 0, 1);
	
	return outputColour;
}