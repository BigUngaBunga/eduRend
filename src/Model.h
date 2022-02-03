
//
//  Model.h
//
//  Carl Johan Gribel 2016, cjgribel@gmail.com
//

#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "stdafx.h"
#include <vector>
#include "vec\vec.h"
#include "vec\mat.h"
#include "ShaderBuffers.h"
#include "Drawcall.h"
#include "OBJLoader.h"
#include "Texture.h"

using namespace linalg;

class Model
{
protected:
	// Pointers to the current device and device context
	ID3D11Device* const			dxdevice;
	ID3D11DeviceContext* const	dxdevice_context;

	// Pointers to the class' vertex & index arrays
	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;

public:

	Model(
		ID3D11Device* dxdevice, 
		ID3D11DeviceContext* dxdevice_context) 
		:	dxdevice(dxdevice),
			dxdevice_context(dxdevice_context)
	{ }

	//
	// Abstract render method: must be implemented by derived classes
	//
	virtual void Render() const = 0;

	//
	// Destructor
	//
	virtual ~Model()
	{ 
		SAFE_RELEASE(vertex_buffer);
		SAFE_RELEASE(index_buffer);
	}
};

class QuadModel : public Model
{
	unsigned nbr_indices = 0;

public:

	QuadModel(
		ID3D11Device* dx3ddevice,
		ID3D11DeviceContext* dx3ddevice_context);

	virtual void Render() const;

	~QuadModel() = default;
};

class OBJModel : public Model
{
	// index ranges, representing drawcalls, within an index array
	struct IndexRange
	{
		unsigned int start;
		unsigned int size;
		unsigned ofs;
		int mtl_index;
	};
private:
	std::vector<IndexRange> index_ranges;
	std::vector<Material> materials;


	vec3f translation = { 0, 0, 0 };
	vec3f rotation = { 0, 0, 0 };
	vec3f scale = { 1, 1, 1 };
	float angle = 0.0f;
	float angleSpeed = 1.0f;

	void append_materials(const std::vector<Material>& mtl_vec)
	{
		materials.insert(materials.end(), mtl_vec.begin(), mtl_vec.end());
	}

public:
	mat4f transform;
	std::vector<OBJModel*> parents = std::vector<OBJModel*>();

	OBJModel(
		const std::string& objfile,
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context);

	virtual void Render() const;

private:
	mat4f GetBaseTransform() const;
	mat4f GetUnscaledTransform() const;
	void SetTranslation(const vec3f& newTranslation);
	void SetRotation(const vec3f& newRotation);
	void SetScale(const vec3f& newScale);
	void SetScale(const float& newScale);
public:
	void AddParentModel(OBJModel* parent);
	void UpdateTransform();
	void UpdateUnscaledTransform();
	void SetAngle(const float& newAngle);
	void SetAngleSpeed(const float& newAngleSpeed);
	void SetTransform(const vec3f& newTranslation, const vec3f& newRotation, const vec3f& newScale);
	void SetTransform(const vec3f& newTranslation, const vec3f& newRotation, const float& newScale = 1);

	mat4f* GetTransform();

	~OBJModel();
};

#endif