//
//  Model.cpp
//
//  CJ Gribel 2016, cjgribel@gmail.com
//

#include "Model.h"

//Model fuctions
void Model::UpdateTransform() {
	transform = mat4f_identity;

	for (auto parent : parents)
		transform = transform * parent->GetBaseTransform();
	transform = transform * GetBaseTransform();
}

void Model::UpdateUnscaledTransform() {
	transform = mat4f_identity;

	for (auto parent : parents)
		transform = transform * parent->GetBaseTransform();
	transform = transform * GetBaseTransform();
}

void Model::AddParentModel(Model* parent) {
	for (auto parentModel : parent->parents)
		parents.push_back(parentModel);

	parents.push_back(parent);
}

mat4f Model::GetBaseTransform() const {
	if (rotation == vec3f_zero)
		return  mat4f::translation(translation) * mat4f::scaling(scale);

	mat4f baseTransform = mat4f_identity;
	if (rotationState == RotationState::First || rotationState == RotationState::Twice) {
		if(!(secondRotation == vec3f_zero))
			baseTransform = baseTransform * mat4f::rotation(angle, secondRotation);
		else
			baseTransform = baseTransform * mat4f::rotation(angle, rotation);
	}
		

	baseTransform = baseTransform * mat4f::translation(translation);
	
	if (rotationState == RotationState::Normal || rotationState == RotationState::Twice)
		baseTransform = baseTransform * mat4f::rotation(angle, rotation);
		
	
	baseTransform = baseTransform * mat4f::scaling(scale);
	return baseTransform;
}

void Model::SetTranslation(const vec3f& newTranslation) { translation = newTranslation; }
void Model::SetScale(const vec3f& newScale) { scale = newScale; }
void Model::SetScale(const float& newScale) { scale = { newScale, newScale, newScale }; }
void Model::SetAngleSpeed(const float& newAngleSpeed) { angleSpeed = newAngleSpeed; }
void Model::SetRotateState(bool rotateFirst, bool rotateNormal) {
	if (rotateFirst && rotateNormal) {
		rotationState = RotationState::Twice;
		return;
	}
	rotationState = rotateFirst ? RotationState::First : RotationState::Normal;
}
void Model::SetRotation(const vec3f& newRotation, bool setSecondRotation) {
	if (setSecondRotation) {
		secondRotation = newRotation;
		secondRotation.normalize();
	}
	else {
		rotation = newRotation;
		rotation.normalize();
	}
}

void Model::SetAngle(const float& newAngle) {
	angle = newAngle * angleSpeed;
	while (angle > 2 * PI)
		angle -= 2 * PI;
	while (angle < -2 * PI)
		angle += 2 * PI;
}

void Model::SetTransform(const vec3f& newTranslation, const vec3f& newRotation, const vec3f& newScale) {
	SetTranslation(newTranslation);
	SetRotation(newRotation);
	SetScale(newScale);
}
void Model::SetTransform(const vec3f& newTranslation, const vec3f& newRotation, const float& newScale) {
	SetTranslation(newTranslation);
	SetRotation(newRotation);
	SetScale(newScale);
}

vec4f Model::GetTranslation() const { 
	std::cout << transform << std::endl;
	return { translation, 1 }; }

mat4f* Model::GetTransform() { return &transform; }



void Model::LoadTexture(Material& material) {
	HRESULT hr;
	// Load Diffuse texture
	//
	if (material.Kd_texture_filename.size()) {

		hr = LoadTextureFromFile(
			dxdevice,
			dxdevice_context,
			material.Kd_texture_filename.c_str(),
			&material.diffuse_texture);
		std::cout << "\t" << material.Kd_texture_filename
			<< (SUCCEEDED(hr) ? " - OK" : "- FAILED") << std::endl;
	}

	// + other texture types here - see Material class
		// ...
}


QuadModel::QuadModel(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context)
	: Model(dxdevice, dxdevice_context)
{
	// Vertex and index arrays
	// Once their data is loaded to GPU buffers, they are not needed anymore
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	// Populate the vertex array with 4 vertices
	Vertex v0, v1, v2, v3;
	v0.Pos = { -0.5, -0.5f, 0.0f };
	v0.Normal = { 0, 0, 1 };
	v0.TexCoord = { 0, 0 };
	v1.Pos = { 0.5, -0.5f, 0.0f };
	v1.Normal = { 0, 0, 1 };
	v1.TexCoord = { 0, 1 };
	v2.Pos = { 0.5, 0.5f, 0.0f };
	v2.Normal = { 0, 0, 1 };
	v2.TexCoord = { 1, 1 };
	v3.Pos = { -0.5, 0.5f, 0.0f };
	v3.Normal = { 0, 0, 1 };
	v3.TexCoord = { 1, 0 };
	vertices.push_back(v0);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	// Populate the index array with two triangles
	// Triangle #1
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(3);
	// Triangle #2
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	// Vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0 };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = (UINT)(vertices.size()*sizeof(Vertex));
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
	ibufferDesc.ByteWidth = (UINT)(indices.size()*sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	const HRESULT ihr = dxdevice->CreateBuffer(&ibufferDesc, &idata, &index_buffer);
	SETNAME(index_buffer, "IndexBuffer");
    
	nbr_indices = (unsigned int)indices.size();
}


void QuadModel::Render() const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex);
	const UINT32 offset = 0;
	dxdevice_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Bind our index buffer
	dxdevice_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Make the drawcall
	dxdevice_context->DrawIndexed(nbr_indices, 0, 0);
}


OBJModel::OBJModel(
	const std::string& objfile,
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context)
	: Model(dxdevice, dxdevice_context)
{

	transform = GetBaseTransform();

	// Load the OBJ
	OBJLoader* mesh = new OBJLoader();
	mesh->Load(objfile);

	// Load and organize indices in ranges per drawcall (material)

	std::vector<unsigned> indices;
	unsigned int i_ofs = 0;

	for (auto& dc : mesh->drawcalls)
	{
		// Append the drawcall indices
		for (auto& tri : dc.tris)
			indices.insert(indices.end(), tri.vi, tri.vi + 3);

		// Create a range
		unsigned int i_size = (unsigned int)dc.tris.size() * 3;
		int mtl_index = dc.mtl_index > -1 ? dc.mtl_index : -1;
		index_ranges.push_back({ i_ofs, i_size, 0, mtl_index });

		i_ofs = (unsigned int)indices.size();
	}

	// Vertex array descriptor
	D3D11_BUFFER_DESC vbufferDesc = { 0 };
	vbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbufferDesc.CPUAccessFlags = 0;
	vbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vbufferDesc.MiscFlags = 0;
	vbufferDesc.ByteWidth = (UINT)(mesh->vertices.size()*sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = &(mesh->vertices)[0];
	// Create vertex buffer on device using descriptor & data
	HRESULT vhr = dxdevice->CreateBuffer(&vbufferDesc, &vdata, &vertex_buffer);
	SETNAME(vertex_buffer, "VertexBuffer");
    
	// Index array descriptor
	D3D11_BUFFER_DESC ibufferDesc = { 0 };
	ibufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufferDesc.CPUAccessFlags = 0;
	ibufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufferDesc.MiscFlags = 0;
	ibufferDesc.ByteWidth = (UINT)(indices.size()*sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	HRESULT ihr = dxdevice->CreateBuffer(&ibufferDesc, &idata, &index_buffer);
	SETNAME(index_buffer, "IndexBuffer");
    
	// Copy materials from mesh
	append_materials(mesh->materials);

	std::cout << "Number of materials in mesh: " << mesh->materials.size() << std::endl;
	for (auto const& material : mesh->materials) {
		std::cout << "Ambient: " << material.Ka << " Diffuse: " << material.Kd << " Specular: " << material.Ks << std::endl;
	}


	// Go through materials and load textures (if any) to device
	std::cout << "Loading textures..." << std::endl;
	for (auto& mtl : materials)
	{
		LoadTexture(mtl);
	}
	std::cout << "Done." << std::endl;

	SAFE_DELETE(mesh);
}


void OBJModel::Render() const
{
	// Bind vertex buffer
	const UINT32 stride = sizeof(Vertex);
	const UINT32 offset = 0;
	dxdevice_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Bind index buffer
	dxdevice_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Iterate drawcalls
	for (auto& irange : index_ranges)
	{
		// Fetch material
		const Material& mtl = materials[irange.mtl_index];

		// Bind diffuse texture to slot t0 of the PS
		dxdevice_context->PSSetShaderResources(0, 1, &mtl.diffuse_texture.texture_SRV);
		// + bind other textures here, e.g. a normal map, to appropriate slots

		// Make the drawcall
		dxdevice_context->DrawIndexed(irange.size, irange.start, 0);
	}
}

const std::vector<Material>& OBJModel::GetMaterials() const { return materials; }

void OBJModel::UpdateSpecular(const vec3f& newDiffuseColour, const vec3f& newSpecularColour) {
	for (Material material : materials) {
		material.Kd = newDiffuseColour;
		material.Ks = newSpecularColour;
		std::cout << "Ambient: " << material.Ka << " Diffuse: " << material.Kd << " Specular: " << material.Ks << std::endl;
	}
		
}

OBJModel::~OBJModel()
{
	for (auto& material : materials)
	{
		SAFE_RELEASE(material.diffuse_texture.texture_SRV);

		// Release other used textures ...
	}
}