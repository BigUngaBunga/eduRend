
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
	// + init other CBuffers
}

//
// Called once at initialization
//
void OurTestScene::InitiateModels() {
	sponza = new OBJModel("assets/crytek-sponza/sponza.obj", dxdevice, dxdevice_context);

	models.emplace("star", new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context));
	models.emplace("smallPlanet", new Cube(dxdevice, dxdevice_context, 3));
	models.emplace("moon", new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context));
	models.emplace("longship", new OBJModel("assets/longship/longship.obj", dxdevice, dxdevice_context));

	models["smallPlanet"]->AddParentModel(models["star"]);
	models["moon"]->AddParentModel(models["smallPlanet"]);
	models["longship"]->AddParentModel(models["star"]);

	models["star"]->SetTransform({ 0, 3, 0 }, { 0, 0, 0 }, 1.5);
	models["smallPlanet"]->SetTransform({ 5, 0, 0 }, { 0.4f, 0.2f, -0.5f }, 0.5);
	models["smallPlanet"]->SetRotation({ 0, 1, 0 }, true);
	models["moon"]->SetTransform({ 3, 0, 0 }, { 0, 1, 0 }, 0.5);
	models["longship"]->SetTransform({ 0, 1, 0 }, { 0, 0, 0 }, 0.4f);

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

	InitiateModels();
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(float dt, InputHandler* input_handler)
{
	UpdateCamera(dt, input_handler);
	
	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	// Sponza model-to-world transformation
	Msponza = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%

	for (auto keyValue : models)
		keyValue.second->SetAngle(angle);

	for (auto keyValue : models)
		keyValue.second->UpdateTransform();

	models["light"]->SetTranslation({ 20 , 10, 20});
	//models["light"]->SetTranslation({ 20 * sin(angle / 10), 10, 20 * cos(angle / 10) });

	// Increment the rotation angle.
	angle += angle_vel * dt;

	// Print fps
	fps_cooldown -= dt;
	if (fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
//		printf("fps %i\n", (int)(1.0f / dt));
		fps_cooldown = 2.0;
	}
}

void OurTestScene::UpdateCamera(float dt, InputHandler* input_handler) {
	float cameraSpeed = input_handler->IsKeyPressed(Keys::Shift) ? camera_vel * 5 : camera_vel;

	//Camera rotation
	camera->rotate({ -(float)input_handler->GetMouseDeltaY() * dt, -(float)input_handler->GetMouseDeltaX() * dt, 0 });
	if (input_handler->IsKeyPressed(Keys::Q))
		camera->rotate({ 0, 0, cameraSpeed * dt * 3.0f });
	if (input_handler->IsKeyPressed(Keys::E))
		camera->rotate({ 0, 0, -cameraSpeed * dt * 3.0f });
	if (input_handler->IsKeyPressed(Keys::Tab))
		camera->SetZeroRoll();


	// Basic camera control
	if (input_handler->IsKeyPressed(Keys::Up) || input_handler->IsKeyPressed(Keys::W))
		camera->move({ 0.0f, 0.0f, -cameraSpeed * dt });
	if (input_handler->IsKeyPressed(Keys::Down) || input_handler->IsKeyPressed(Keys::S))
		camera->move({ 0.0f, 0.0f, cameraSpeed * dt });
	if (input_handler->IsKeyPressed(Keys::Right) || input_handler->IsKeyPressed(Keys::D))
		camera->move({ cameraSpeed * dt, 0.0f, 0.0f });
	if (input_handler->IsKeyPressed(Keys::Left) || input_handler->IsKeyPressed(Keys::A))
		camera->move({ -cameraSpeed * dt, 0.0f, 0.0f });
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

	vec4f lightPosition;
	lightPosition = { models["light"]->GetTransform()->m14, models["light"]->GetTransform()->m24, models["light"]->GetTransform()->m34, 1 };
	UpdateLightAndCameraBuffer(lightPosition, camera->GetWorldPosition());

	// Obtain the matrices needed for rendering from the camera
	Mview = camera->get_WorldToViewMatrix();
	Mproj = camera->get_ProjectionMatrix();


	for (const auto& keyValue : models) {
		if (auto model = dynamic_cast<OBJModel*>(keyValue.second)) {
			for (auto const &material : model->GetMaterials()) {
				UpdateTransformationBuffer(model->transform, Mview, Mproj);
				UpdateMaterialBuffer(material);
				keyValue.second->Render();
			}
		}
		else if(keyValue.second != nullptr) {
			UpdateTransformationBuffer(keyValue.second->transform, Mview, Mproj);
			UpdateMaterialBuffer(keyValue.second->GetMaterial());
			keyValue.second->Render();
		}
	}

	

	// Load matrices + Sponza's transformation to the device and render it
	//UpdateTransformationBuffer(Msponza, Mview, Mproj);
	//sponza->Render();

	for (auto const& material : sponza->GetMaterials()) {
		UpdateTransformationBuffer(Msponza, Mview, Mproj);
		UpdateMaterialBuffer(material);
		sponza->Render();
	}
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
	// + release other CBuffers
}

void OurTestScene::WindowResize(int window_width, int window_height)
{
	if (camera)
		camera->aspect = float(window_width) / window_height;

	Scene::WindowResize(window_width, window_height);
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

void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
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

	//std::cout << LightPosition << std::endl;

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
	matrix_buffer_->kA = vec4f(material.Ka, 1);
	matrix_buffer_->kD = vec4f(material.Kd, 1);
	matrix_buffer_->kS = vec4f(material.Ks, 1);
	dxdevice_context->Unmap(sceneMaterialBuffer, 0);
}