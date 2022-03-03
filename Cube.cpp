#define _USE_MATH_DEFINES
#include "Cube.h"
#include <cmath>
using std::cout;

Cube::Cube(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context, const float length, bool invertNormals) : Model(dxdevice, dxdevice_context) {

	float point = length / 2;
	std::array<vec3f, 6> cubeNormals = { vec3f{0, 0, 1}, vec3f{0, 0, -1}, vec3f{-1, 0, 0}, vec3f{1, 0, 0}, vec3f{0, 1, 0}, vec3f{0, -1, 0} };
	if (invertNormals)
		for (vec3f& normal : cubeNormals) normal *= -1;

	std::array<std::array<vec3f, 4>, 2> cubeCorners = { vec3f{point, point, point}, vec3f{-point, point, point}, vec3f{point, point, -point}, vec3f{-point, point, -point},
					 vec3f{point, -point, point}, vec3f{-point, -point, point}, vec3f{point, -point, -point}, vec3f{-point, -point, -point} };
	int cornerCounter = 0;
	for (size_t i = 0; i < cubeNormals.size(); i++)
	{
		if (i < 2) {
			cornerCounter = i * 2;
			CreateSquare(cubeNormals[i], cubeCorners[0][cornerCounter], cubeCorners[0][cornerCounter + 1], cubeCorners[1][cornerCounter], cubeCorners[1][cornerCounter + 1]);
		}
		else if (i < 4) {
			CreateSquare(cubeNormals[i], cubeCorners[0][i % 2], cubeCorners[0][i], cubeCorners[1][i % 2], cubeCorners[1][i]);
		}
		else {
			cornerCounter = (i + 1) % (cubeCorners[0].size() + 1);
			CreateSquare(cubeNormals[i], cubeCorners[cornerCounter][1], cubeCorners[cornerCounter][0], cubeCorners[cornerCounter][3], cubeCorners[cornerCounter][2]);
		}		
	}

	CreateSquare(cubeNormals[0], cubeCorners[0][0], cubeCorners[0][1], cubeCorners[1][0], cubeCorners[1][1]);

	InitializeRender();
	LoadTexture(material);
	material.isSkybox = invertNormals;
}

void Cube::InitializeRender() {
	// Vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0 };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = (UINT)(vertices.size() * sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &vertices[0];
	// Create vertex buffer on device using descriptor & data
	const HRESULT vhr = dxdevice->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);
	SETNAME(vertex_buffer, "VertexBuffer");

	//  Index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0 };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	const HRESULT ihr = dxdevice->CreateBuffer(&ibufferDesc, &idata, &index_buffer);
	SETNAME(index_buffer, "IndexBuffer");

	nbr_indices = (unsigned int)indices.size();
}

void Cube::CreateSquare(const vec3f& normal, const vec3f& vectorA, const vec3f& vectorB, const vec3f& vectorC, const vec3f& vectorD) {

	Vertex v1, v2, v3, v4;
	v1.Normal = v2.Normal = v3.Normal = v4.Normal = normal;
	v1.Pos = vectorA;
	v2.Pos = vectorB;
	v3.Pos = vectorC;
	v4.Pos = vectorD;

	if (normal.x + normal.y + normal.z < 0)
	{
		AddVertex(v2, { 1, 1 });
		AddVertex(v1, { 0, 1 });
		AddVertex(v4, { 0, 0 });
		AddVertex(v3, { 1, 0 });
	}
	else {
		AddVertex(v1, { 1, 1 });
		AddVertex(v2, { 0, 1 });
		AddVertex(v3, { 0, 0 });
		AddVertex(v4, { 1, 0 });
	}

	AddASquare();
}

void Cube::AddVertex(Vertex& vertex, const vec2f& textureCoordinate) {
	vertex.TexCoord = textureCoordinate;
	vertices.push_back(vertex);
}

void Cube::AddASquare() {
	int vertexA = nbr_indices, vertexB = nbr_indices + 1, vertexC = nbr_indices + 2, vertexD = nbr_indices + 3;

	indices.push_back(vertexD);
	indices.push_back(vertexC);
	indices.push_back(vertexB);
	CalculateTangentAndBinormal(vertices[vertexD], vertices[vertexC], vertices[vertexB]);


	indices.push_back(vertexC);
	indices.push_back(vertexA);
	indices.push_back(vertexB);
	CalculateTangentAndBinormal(vertices[vertexC], vertices[vertexA], vertices[vertexB]);

	nbr_indices += 4;
}

const Material& Cube::GetMaterial() const { return material; }

void Cube::Render() const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex);
	const UINT32 offset = 0;
	dxdevice_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Bind our index buffer
	dxdevice_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	dxdevice_context->PSSetShaderResources(0, 1, &material.diffuse_texture.texture_SRV);

	// Make the drawcall
	dxdevice_context->DrawIndexed(nbr_indices, 0, 0);
}