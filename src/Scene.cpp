
#include "Scene.h"

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
	// + init other CBuffers
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)window_width / window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	camera->moveTo({ 0, 0, 5 });

	// Create objects
	cube = new Cube(dxdevice, dxdevice_context, 1.337f);
	sponza = new OBJModel("assets/crytek-sponza/sponza.obj", dxdevice, dxdevice_context);

	star = new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context);
	smallPlanet = new Cube(dxdevice, dxdevice_context, 3);
	moon = new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context);

	ship = new OBJModel("assets/longship/longship.obj", dxdevice, dxdevice_context);


	models.push_back(new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context));
	models.back()->SetTransform({ 0.0f, 0.0f, -10 }, { 0.0f, 1.0f, 0.0f }, 2.0f);

	models.push_back(new OBJModel("assets/hand/hand.obj", dxdevice, dxdevice_context));
	models.back()->SetTransform({ 1.6f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f });
	models.back()->AddParentModel(models.front());

	models.push_back(new OBJModel("assets/tyre/Tyre.obj", dxdevice, dxdevice_context));
	models.back()->SetTransform({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 0.1f);
	models.back()->AddParentModel(models.at(models.size() - 2));

	models.push_back(new OBJModel("assets/hand/hand.obj", dxdevice, dxdevice_context));
	models.back()->SetTransform({ 1.6f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f });
	models.back()->AddParentModel(models.front());
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	InputHandler* input_handler)
{
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
	
	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	// Quad model-to-world transformation
	Mquad = mat4f::translation(2, 0, 0) *			// No translation
		mat4f::rotation(-angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	// Sponza model-to-world transformation
	Msponza = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%

	mStar = mat4f::translation(0, 3, 0) *			
		mat4f::scaling(1.5, 1.5, 1.5);

	mPlanet = mat4f::rotation(-angle, 0.0f, 1.0f, 0.0f) *
		mat4f::translation(5, 0, 0) *
		mat4f::rotation(cos(angle) - sin(angle), 0.3f, 0.1f) *
		mat4f::scaling(0.5, 0.5, 0.5);

	mMoon = mat4f::rotation(angle * 2, 0.0f, 1.0f, 0.0f) *
		mat4f::translation(3, 0, 0) *
		mat4f::scaling(0.5, 0.5, 0.5);

	mShip = mat4f::rotation(angle , 1.0f, 0.0f, 0.0f) *
		mat4f::translation(0, 2, 0) *
		mat4f::scaling(0.2f);

	mMoon = mStar * mPlanet * mMoon;
	mPlanet = mStar * mPlanet;
	mShip = mStar * mShip;

	//TODO fixa
	//planet->SetAngle(angle);
	//handSatelite->SetAngle(angle/2);
	//inverseHandSatelite->SetAngle(angle / 3);
	//wheelSatelite->SetAngle(angle);

	for (auto model : models) {
		model->SetAngle(angle);
		model->UpdateTransform();
	}
		
	//TODO fixa


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

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	dxdevice_context->VSSetConstantBuffers(0, 1, &transformation_buffer);

	// Obtain the matrices needed for rendering from the camera
	Mview = camera->get_WorldToViewMatrix();
	Mproj = camera->get_ProjectionMatrix();

	// Load matrices + the Quad's transformation to the device and render it
	UpdateTransformationBuffer(Mquad, Mview, Mproj);
	cube->Render();


	for (auto model : models) {
		UpdateTransformationBuffer(model->transform, Mview, Mproj);
		model->Render();
	}

	UpdateTransformationBuffer(mStar, Mview, Mproj);
	star->Render();
	UpdateTransformationBuffer(mPlanet, Mview, Mproj);
	smallPlanet->Render();
	UpdateTransformationBuffer(mMoon, Mview, Mproj);
	moon->Render();
	UpdateTransformationBuffer(mShip, Mview, Mproj);
	ship->Render();



	// Load matrices + Sponza's transformation to the device and render it
	UpdateTransformationBuffer(Msponza, Mview, Mproj);
	sponza->Render();
}

void OurTestScene::Release()
{
	//SAFE_DELETE(quad);
	SAFE_DELETE(cube);
	SAFE_DELETE(sponza);
	SAFE_DELETE(camera);

	SAFE_RELEASE(transformation_buffer);
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