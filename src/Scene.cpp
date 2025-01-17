
#include "Scene.h"

//TODO ta bort
#include <functional> 

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	dxdevice(dxdevice),
	dxdevice_context(dxdevice_context),
	window_width(window_width),
	window_height(window_height)
{ }

void Scene::WindowResize(
	int window_width,
	int window_height)
{
	this->window_width = window_width;
	this->window_height = window_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{ 
	InitTransformationBuffer();
	InitLightAndCameraBuffer();
	InitMaterialBuffer();
	//InitLightBuffer();
	// + init other CBuffers
}

//
// Called once at initialization
//
void OurTestScene::InitiateModels() {
	sponza = new OBJModel("assets/crytek-sponza/sponza.obj", dxdevice, dxdevice_context);
	skybox = new Cube(dxdevice, dxdevice_context, 300, true);

	models.emplace("star", new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context));
	models.emplace("smallPlanet", new Cube(dxdevice, dxdevice_context, 3));
	models.emplace("moon", new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context));
	models.emplace("longship", new OBJModel("assets/longship/longship.obj", dxdevice, dxdevice_context));
	models.emplace("stationaryLongship", new OBJModel("assets/longship/longship.obj", dxdevice, dxdevice_context));

	models["smallPlanet"]->AddParentModel(models["star"]);
	models["moon"]->AddParentModel(models["smallPlanet"]);
	models["longship"]->AddParentModel(models["star"]);

	models["star"]->SetTransform({ 0, 3, 0 }, { 0, 0, 0 }, 1.5);
	models["smallPlanet"]->SetTransform({ 5, 0, 0 }, { 0.4f, 0.2f, -0.5f }, 0.5);
	models["smallPlanet"]->SetRotation({ 0, 1, 0 }, true);
	models["moon"]->SetTransform({ 3, 0, 0 }, { 0, 1, 0 }, 0.5);
	models["longship"]->SetTransform({ 0, 1, 0 }, { 1, 0, 0 }, 0.4f);

	models["stationaryLongship"]->SetTransform({ 5, 2, 4 }, {0, 0, 0}, 1);

	models["smallPlanet"]->SetRotateState(true, true);
	models["moon"]->SetRotateState(true, false);
	models["longship"]->SetRotateState(true, false);

	models["smallPlanet"]->SetAngleSpeed(0.4f);

	models.emplace("light", new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context));
	models.emplace("sphere", new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context));
	models.emplace("firstHand", new OBJModel("assets/hand/hand.obj", dxdevice, dxdevice_context));
	models.emplace("secondHand", new OBJModel("assets/hand/hand.obj", dxdevice, dxdevice_context));
	models.emplace("tyre", new OBJModel("assets/tyre/Tyre.obj", dxdevice, dxdevice_context));
	models.emplace("crate", new OBJModel("assets/WoodenCrate/WoodenCrate.obj", dxdevice, dxdevice_context));

	models["sphere"]->SetTransform({ -7.0f, 0.0f, -10 }, { 1.0f, 1.0f, 0.0f }, 1.5f);
	models["firstHand"]->SetTransform({ 1.6f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	models["secondHand"]->SetTransform({ 1.0f, 1.0f, -1.0f }, { 0.0f, -1.0f, 1.0f }, 1.7f);
	models["tyre"]->SetTransform({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, 0.2f);
	models["crate"]->SetTransform({ 0.0f, 0.0f, 0.0f }, { 1.0f, -1.0f, -1.0f }, 0.2f);

	models["firstHand"]->AddParentModel(models["sphere"]);
	models["secondHand"]->AddParentModel(models["sphere"]);
	models["tyre"]->AddParentModel(models["secondHand"]);
	models["crate"]->AddParentModel(models["tyre"]);

	models["secondHand"]->SetAngleSpeed(0.5);
	models["crate"]->SetAngleSpeed(5);

	models.emplace("torus", new OBJModel("assets/torus/torus.obj", dxdevice, dxdevice_context));
	models["torus"]->SetTransform({ 1.3f, 8.0f, 15.4f }, { 0.0f, -1.0f, 0.3f }, 1.5f);
	models["torus"]->SetAngleSpeed(0.21f);
}

void OurTestScene::Init()
{
	camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)window_width / window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	camera->moveTo({ 0, 0, 15 });
	UpdateSamplerDescription();
	InitiateModels();

	const char* filePaths[6] = { "assets/cubemaps/cubemaps/cloudyhillscube/cloudyhills_posx.png",
								"assets/cubemaps/cubemaps/cloudyhillscube/cloudyhills_negx.png",
								"assets/cubemaps/cubemaps/cloudyhillscube/cloudyhills_negy.png",
								"assets/cubemaps/cubemaps/cloudyhillscube/cloudyhills_posy.png",
								"assets/cubemaps/cubemaps/cloudyhillscube/cloudyhills_posz.png",
								"assets/cubemaps/cubemaps/cloudyhillscube/cloudyhills_negz.png"};

	//const char* filePaths[6] = { "assets/cubemaps/cubemaps/brightday/posx.png",
	//						"assets/cubemaps/cubemaps/brightday/negx.png",
	//						"assets/cubemaps/cubemaps/brightday/negy.png",
	//						"assets/cubemaps/cubemaps/brightday/posy.png",
	//						"assets/cubemaps/cubemaps/brightday/posz.png",
	//						"assets/cubemaps/cubemaps/brightday/negz.png" };


	LoadCubeTextureFromFile(dxdevice, filePaths, &cubeMapTexture);

	//AddLightSource(vec4f_zero);
	//AddLightSource(vec4f_zero);
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(float dt, InputHandler* input_handler)
{
	UpdateInput(dt, input_handler);
	
	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	// Sponza model-to-world transformation
	Msponza = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f); // Rotate pi/2 radians (90 degrees) around y

	mSkybox = mat4f::translation(camera->GetWorldPosition().xyz());// * mat4f::scaling(-1)
	//camera->get_ViewToWorldMatrix() *

	for (auto keyValue : models)
		keyValue.second->SetAngle(angle);

	for (auto keyValue : models)
		keyValue.second->UpdateTransform();

	//models["light"]->SetTranslation({ 20 , 10, 20});
	models["light"]->SetTranslation({ 20 * sin(angle / 10), 10, 20 * cos(angle / 10) });

	// Increment the rotation angle.
	angle += angle_vel * dt;

	// Print fps
	if (false) {
		fps_cooldown -= dt;
		if (fps_cooldown < 0.0)
		{
			std::cout << "fps " << (int)(1.0f / dt) << std::endl;
			//		printf("fps %i\n", (int)(1.0f / dt));
			fps_cooldown = 2.0;
		}
	}

}

void OurTestScene::UpdateInput(float dt, InputHandler* input_handler) {
	float cameraSpeed = input_handler->IsKeyPressed(Keys::Shift) ? camera_vel * 5 : camera_vel;

	//Camera rotation
	camera->rotate({ -(float)input_handler->GetMouseDeltaY() * dt, -(float)input_handler->GetMouseDeltaX() * dt, 0 });
	if (input_handler->IsKeyPressed(Keys::Q))
		camera->rotate({ 0, 0, cameraSpeed * dt * 3.0f });
	if (input_handler->IsKeyPressed(Keys::E))
		camera->rotate({ 0, 0, -cameraSpeed * dt * 3.0f });
	if (input_handler->IsKeyPressed(Keys::Tab)) {
		camera->SetZeroRoll();
		camera->ResetScale();
	}

	// Basic camera control
	if (input_handler->IsKeyPressed(Keys::W))
		camera->move({ 0.0f, 0.0f, -cameraSpeed * dt });
	if (input_handler->IsKeyPressed(Keys::S))
		camera->move({ 0.0f, 0.0f, cameraSpeed * dt });
	if (input_handler->IsKeyPressed(Keys::D))
		camera->move({ cameraSpeed * dt, 0.0f, 0.0f });
	if (input_handler->IsKeyPressed(Keys::A))
		camera->move({ -cameraSpeed * dt, 0.0f, 0.0f });

	float scaleSpeed = cameraSpeed * dt / 20;

	if (input_handler->IsKeyPressed(Keys::Up))
		camera->Scale({ 0, 0, scaleSpeed });
	if(input_handler->IsKeyPressed(Keys::Down))
		camera->Scale({ 0, 0,-scaleSpeed });
	if(input_handler->IsKeyPressed(Keys::Right))
		camera->Scale({ scaleSpeed , 0, 0 });
	if(input_handler->IsKeyPressed(Keys::Left))
		camera->Scale({ -scaleSpeed, 0, 0 });
	if (input_handler->IsKeyPressed(Keys::Plus))
		camera->Scale({ 0, scaleSpeed, 0 });
	if (input_handler->IsKeyPressed(Keys::Minus))
		camera->Scale({ 0, -scaleSpeed, 0 });

	if (input_handler->IsKeyClicked(Keys::F))
		samplerDescriptionSettings.ChangeFilter();
	if (input_handler->IsKeyClicked(Keys::G))
		samplerDescriptionSettings.ChangeAddressMode();

	if (input_handler->IsKeyClicked(Keys::N))
		displayNormalMaps = !displayNormalMaps;

	if (input_handler->IsKeyClicked(Keys::Z))
		showSponza = !showSponza;

}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	dxdevice_context->VSSetConstantBuffers(0, 1, &transformation_buffer);
	dxdevice_context->PSSetConstantBuffers(0, 1, &lightAndCameraBuffer);
	dxdevice_context->PSSetConstantBuffers(1, 1, &sceneMaterialBuffer);
	dxdevice_context->PSSetShaderResources(2, 1, &cubeMapTexture.texture_SRV);
	//dxdevice_context->PSSetConstantBuffers(2, 1, &lightBuffer);

	if (samplerDescriptionSettings.wasChanged) 
		UpdateSamplerDescription();

	vec4f lightPosition;
	lightPosition = { models["light"]->GetTransform()->m14, models["light"]->GetTransform()->m24, models["light"]->GetTransform()->m34, 1 };
	UpdateLightAndCameraBuffer(lightPosition, camera->GetWorldPosition());

	//UpdateLightSource(0, lightPosition);
	//UpdateLightSource(1, camera->GetWorldPosition());
	//UpdateLightBuffer();
	// Obtain the matrices needed for rendering from the camera
	Mview = camera->get_WorldToViewMatrix();
	Mproj = camera->get_ProjectionMatrix();



	for (const auto& keyValue : models) {
		if (auto model = dynamic_cast<OBJModel*>(keyValue.second)) {
			UpdateTransformationBuffer(model->transform, Mview, Mproj);

			int materialSize = model->GetIndexRangeSize();
			for (int i = 0; i < materialSize; ++i) {
				UpdateMaterialBuffer(model->GetMaterial(i));
				model->RenderIndexRange(i);
			}
		}
		else if (auto model = dynamic_cast<Cube*>(keyValue.second)) {
			UpdateTransformationBuffer(model->transform, Mview, Mproj);
			UpdateMaterialBuffer(model->GetMaterial());
			model->Render();
		}
	}
	
	if (showSponza) {
		UpdateTransformationBuffer(Msponza, Mview, Mproj);
		int materialSize = sponza->GetIndexRangeSize();
		for (int i = 0; i < materialSize; ++i) {
			UpdateMaterialBuffer(sponza->GetMaterial(i));
			sponza->RenderIndexRange(i);
		}
	}
	

	UpdateTransformationBuffer(mSkybox, Mview, Mproj);
	UpdateMaterialBuffer(skybox->GetMaterial());
	skybox->Render();
}

void OurTestScene::Release()
{
	SAFE_DELETE(sponza);
	SAFE_DELETE(camera);
	for (auto modelPointer : models)
		SAFE_DELETE(modelPointer.second);

	SAFE_RELEASE(transformation_buffer);
	SAFE_RELEASE(lightAndCameraBuffer);
	SAFE_RELEASE(sceneMaterialBuffer);
	SAFE_RELEASE(sampler);
}

void OurTestScene::WindowResize(int window_width, int window_height)
{
	if (camera)
		camera->aspect = float(window_width) / window_height;

	Scene::WindowResize(window_width, window_height);
}

void OurTestScene::UpdateSamplerDescription() {
	D3D11_FILTER filter;
	D3D11_TEXTURE_ADDRESS_MODE addressMode;

	int numberOfFilters = 3;
	int numberOfAddressModes = 3;
	samplerDescriptionSettings.filterType %= numberOfFilters;
	samplerDescriptionSettings.AddressModeType %= numberOfAddressModes;
	samplerDescriptionSettings.wasChanged = false;

	std::string filterName;
	std::string addressModeName;

	switch (samplerDescriptionSettings.filterType)
	{
	case 0:
		filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
		filterName = "anisotropic filter";
		break;
	case 1:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		filterName = "linear filter";
		break;
	default:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		filterName = "point filter";
		break;
	}
	
	switch (samplerDescriptionSettings.AddressModeType)
	{
	case 0:
		addressMode = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		addressModeName = "wrap";
		break;
	case 1:
		addressMode = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		addressModeName = "clamp";
		break;
	default:
		addressMode = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
		addressModeName = "mirror";
		break;
	}

	std::cout << "Filter type: " << filterName << " Address mode: " << addressModeName << std::endl;


	samplerDescriptor = {
	filter,
	addressMode, // AddressU
	addressMode, // AddressV
	addressMode, // AddressW
	0.0f, // MipLODBias
	16, // MaxAnisotropy
	D3D11_COMPARISON_NEVER, // ComapirsonFunc
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // BorderColor
	-FLT_MAX, // MinLOD
	FLT_MAX, // MaxLOD
	};

	dxdevice->CreateSamplerState(&samplerDescriptor, &sampler);
	dxdevice_context->PSSetSamplers(0, 1, &sampler);
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(TransformationBuffer);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &transformation_buffer));
}

void OurTestScene::UpdateTransformationBuffer(mat4f ModelToWorldMatrix, mat4f WorldToViewMatrix, mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrix_buffer_ = (TransformationBuffer*)resource.pData;
	matrix_buffer_->ModelToWorldMatrix = ModelToWorldMatrix;
	matrix_buffer_->WorldToViewMatrix = WorldToViewMatrix;
	matrix_buffer_->ProjectionMatrix = ProjectionMatrix;
	dxdevice_context->Unmap(transformation_buffer, 0);
}

void OurTestScene::InitLightAndCameraBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(LightAndCameraBuffer);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &lightAndCameraBuffer));
}

void OurTestScene::UpdateLightAndCameraBuffer(const vec4f& LightPosition, const vec4f& CameraPosition) {
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(lightAndCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	auto matrix_buffer_ = (LightAndCameraBuffer*)resource.pData;
	matrix_buffer_->LightPosition = LightPosition;
	matrix_buffer_->CameraPosition = CameraPosition;
	dxdevice_context->Unmap(lightAndCameraBuffer, 0);
}

void OurTestScene::InitMaterialBuffer() {
	HRESULT hr;
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(PhongMaterial);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;

	ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &sceneMaterialBuffer));	
}

void OurTestScene::UpdateMaterialBuffer(const Material& material) {
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(sceneMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	auto matrix_buffer_ = (PhongMaterial*)resource.pData;
	matrix_buffer_->kA = vec4f(material.Ka, material.isSkybox);
	matrix_buffer_->kD = vec4f(material.Kd, displayNormalMaps && !material.isSkybox ? material.HasNormalMap() : false);
	matrix_buffer_->kS = vec4f(material.Ks, material.shininess);
	dxdevice_context->Unmap(sceneMaterialBuffer, 0);
}

void OurTestScene::InitLightBuffer() {
	HRESULT hr;
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(LightBuffer);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;

	ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &lightBuffer));
}

void OurTestScene::UpdateLightBuffer() {
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	auto matrix_buffer_ = (LightBuffer*)resource.pData;
	matrix_buffer_->lightSources = lightBufferStruct.lightSources;
	matrix_buffer_->numberOfLights = lightBufferStruct.numberOfLights;
	dxdevice_context->Unmap(lightBuffer, 0);

	for (auto vector : matrix_buffer_->lightSources)
		std::cout << "Vector: " << vector;
	std::cout << std::endl;
}