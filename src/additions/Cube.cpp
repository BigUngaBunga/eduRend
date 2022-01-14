#include "Cube.h"

Cube::Cube(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context, const float length) : Model(dxdevice, dxdevice_context) {

	float point = length / 2;
	std::array<vec3f, 6> cubeNormals = { vec3f{0, 0, 1}, vec3f{1, 0, 0}, vec3f{0, 0, -1}, vec3f{-1, 0, 0}, vec3f{0, 1, 0}, vec3f{0, -1, 0} };
	std::array<std::array<vec3f, 4>, 2> cubeCorners = { vec3f{point, point, point}, vec3f{-point, point, point}, vec3f{point, -point, point}, vec3f{-point, -point, point},
					vec3f{point, point, -point}, vec3f{-point, point, -point}, vec3f{point, -point, -point}, vec3f{point, -point, -point} };

	for (size_t i = 0; i < cubeNormals.size(); i++)
	{
		int cornerCounter = i % (cubeCorners[0].size() -1);
		if (i < cubeCorners[0].size())
			CreateSquare(cubeNormals[i], cubeCorners[0][i], cubeCorners[0][cornerCounter], cubeCorners[1][i], cubeCorners[1][cornerCounter]);
		else
			CreateSquare(cubeNormals[i], cubeCorners[cornerCounter][0], cubeCorners[cornerCounter][1], cubeCorners[cornerCounter][2], cubeCorners[cornerCounter][3]);
	}

	//TODO fatta vad det är som händer
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

}

void Cube::CreateSquare(const vec3f& normal, const vec3f& vectorA, const vec3f& vectorB, const vec3f& vectorC, const vec3f& vectorD) {

	Vertex v1, v2, v3, v4;
	v1.Normal = v2.Normal = v3.Normal = v4.Normal = normal;
	v1.Pos = vectorA;
	v2.Pos = vectorB;
	v3.Pos = vectorC;
	v4.Pos = vectorD;
	//TODO lägg till texturkoordinater
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);
	AddASquare();
}


void Cube::AddASquare() {
	indices.push_back(nbr_indices);
	indices.push_back(nbr_indices + 1);
	indices.push_back(nbr_indices + 3);

	indices.push_back(nbr_indices + 1);
	indices.push_back(nbr_indices + 2);
	indices.push_back(nbr_indices + 3);

	nbr_indices += 4;
}


void Cube::Render() const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
	const UINT32 offset = 0;
	dxdevice_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Bind our index buffer
	dxdevice_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Make the drawcall
	dxdevice_context->DrawIndexed(nbr_indices, 0, 0);
}