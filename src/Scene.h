
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
	// 
	// CBuffer client-side definitions
	// These must match the corresponding shader definitions 
	//

	struct TransformationBuffer
	{
		mat4f ModelToWorldMatrix;
		mat4f WorldToViewMatrix;
		mat4f ProjectionMatrix;
	};

	struct LightAndCameraBuffer
	{
		mat4f LightMatrix;
		mat4f CameraMatrix;
	};

	struct PhongBuffer
	{
		//TODO fill with data
	};

	//
	// Scene content
	//
	Camera* camera;

	Cube* cube;
	OBJModel* sponza;

	OBJModel* planet;
	OBJModel* handSatelite;
	OBJModel* inverseHandSatelite;
	OBJModel* wheelSatelite;

	std::map<std::string, OBJModel*> models = std::map<std::string, OBJModel*>();

	OBJModel* star;
	Cube* smallPlanet;
	OBJModel* moon;
	OBJModel* ship;

	mat4f mStar;
	mat4f mPlanet;
	mat4f mMoon;
	mat4f mShip;

	mat4f lightSource;


	// Model-to-world transformation matrices
	mat4f Msponza;
	mat4f Mquad;

	// World-to-view matrix
	mat4f Mview;
	// Projection matrix
	mat4f Mproj;

	// Misc
	float angle = 0;			// A per-frame updated rotation angle (radians)...
	float angle_vel = fPI / 2;	// ...and its velocity (radians/sec)
	float camera_vel = 5.0f;	// Camera movement velocity in units/s
	float fps_cooldown = 0;

	void InitTransformationBuffer();

	void UpdateTransformationBuffer(
		mat4f ModelToWorldMatrix,
		mat4f WorldToViewMatrix,
		mat4f ProjectionMatrix);

	void InitLightAndCameraBuffer();
	void UpdateLightAndCameraBuffer(const mat4f& LightMatrix, const mat4f& CameraMatrix);

public:
	OurTestScene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	void Init() override;

	void Update(float dt, InputHandler* input_handler) override;
	void UpdateCamera(float dt, InputHandler* input_handler);

	void Render() override;

	void Release() override;

	void WindowResize(
		int window_width,
		int window_height) override;
};

#endif