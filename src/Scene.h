
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "stdafx.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Model.h"
#include "Cube.h"
#include "Texture.h"


#include <map>
// New files
// Material
// Texture <- stb

// TEMP


class Scene
{
protected:
	ID3D11Device*			dxdevice;
	ID3D11DeviceContext*	dxdevice_context;
	int						window_width;
	int						window_height;

public:

	Scene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	virtual void Init() = 0;

	virtual void Update(
		float dt,
		InputHandler* input_handler) = 0;
	
	virtual void Render() = 0;
	
	virtual void Release() = 0;

	virtual void WindowResize(
		int window_width,
		int window_height);
};

class OurTestScene : public Scene
{
	//
	// Constant buffers (CBuffers) for data that is sent to shaders
	//

	// CBuffer for transformation matrices
	ID3D11Buffer* transformation_buffer = nullptr;
	// + other CBuffers
	ID3D11Buffer* lightAndCameraBuffer = nullptr;
	ID3D11Buffer* sceneMaterialBuffer = nullptr;
	// 
	// CBuffer client-side definitions
	// These must match the corresponding shader definitions 
	//

	ID3D11SamplerState* sampler;
	D3D11_SAMPLER_DESC samplerDescriptor;

	struct TransformationBuffer
	{
		mat4f ModelToWorldMatrix;
		mat4f WorldToViewMatrix;
		mat4f ProjectionMatrix;
		vec4f WorldPosition;
	};

	struct LightAndCameraBuffer
	{
		vec4f LightPosition;
		vec4f CameraPosition;
	};

	struct PhongMaterial {
		vec4f kA;
		vec4f kD;
		vec4f kS;
	};

	struct SamplerDescriptionSettings {
		int filterType = 0;
		int AddressModeType = 0;
		bool wasChanged;

		void ChangeFilter(){
			++filterType;
			wasChanged = true;
		}
		void ChangeAddressMode() {
			++AddressModeType;
			wasChanged = true;
		}
	};

	//
	// Scene content
	//
	Camera* camera;
	OBJModel* sponza;

	std::map<std::string, Model*> models = std::map<std::string, Model*>();

	// Model-to-world transformation matrices
	mat4f Msponza;

	// World-to-view matrix
	mat4f Mview;
	// Projection matrix
	mat4f Mproj;

	// Misc
	float angle = 0;			// A per-frame updated rotation angle (radians)...
	float angle_vel = fPI / 2;	// ...and its velocity (radians/sec)
	float camera_vel = 10.0f;	// Camera movement velocity in units/s
	float fps_cooldown = 0;
	SamplerDescriptionSettings samplerDescriptionSettings;

	void UpdateSamplerDescription();

	void InitTransformationBuffer();

	void UpdateTransformationBuffer(
		mat4f ModelToWorldMatrix,
		mat4f WorldToViewMatrix,
		mat4f ProjectionMatrix);

	void InitLightAndCameraBuffer();
	void UpdateLightAndCameraBuffer(const vec4f& LightMatrix, const vec4f& CameraMatrix);

	void InitMaterialBuffer();
	void UpdateMaterialBuffer(const Material& material);

	void InitiateModels();

public:
	OurTestScene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	void Init() override;

	void Update(float dt, InputHandler* input_handler) override;
	void UpdateInput(float dt, InputHandler* input_handler);

	void Render() override;

	void Release() override;

	void WindowResize(
		int window_width,
		int window_height) override;
};

#endif