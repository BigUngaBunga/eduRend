
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
private:
	enum RotationState
	{
		Normal, First, Twice
	};

	vec3f translation = { 0, 0, 0 };
	vec3f rotation = { 0, 0, 0 };
	vec3f secondRotation = { 0, 0, 0 };
	vec3f scale = { 1, 1, 1 };
	float angle = 0.0f;
	float angleSpeed = 1.0f;
	RotationState rotationState;


protected:
	
	// Pointers to the current device and device context
	ID3D11Device* const			dxdevice;
	ID3D11DeviceContext* const	dxdevice_context;

	// Pointers to the class' vertex & index arrays
	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;

	mat4f GetBaseTransform() const;
	void SetScale(const vec3f& newScale);
	void SetScale(const float& newScale);
	void LoadTexture(Material& material);


public:

	mat4f transform;
	std::vector<Model*> parents = std::vector<Model*>();

	void AddParentModel(Model* parent);
	void UpdateTransform();
	void UpdateUnscaledTransform();
	void SetRotateState(bool rotateFirst, bool rotateNormal);
	void SetAngle(const float& newAngle);
	void SetAngleSpeed(const float& newAngleSpeed);
	void SetTransform(const vec3f& newTranslation, const vec3f& newRotation, const vec3f& newScale);
	void SetTransform(const vec3f& newTranslation, const vec3f& newRotation, const float& newScale = 1);
	void SetRotation(const vec3f& newRotation, bool setSecondRotation = false);

	void SetTranslation(const vec3f& newTranslation);
	vec4f GetTranslation() const;
	mat4f* GetTransform();

	void CalculateTangentAndBinormal(Vertex& vertex0, Vertex& vertex1, Vertex& vertex2) const;

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

	void append_materials(const std::vector<Material>& mtl_vec)
	{
		materials.insert(materials.end(), mtl_vec.begin(), mtl_vec.end());
	}

public:


	OBJModel(
		const std::string& objfile,
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context);

	virtual void Render() const;
	void RenderIndexRange(const int& indexRangeIndex) const;
	int GetIndexRangeSize() const;
	const Material& GetMaterial(const int& indexRangeIndex) const;
	~OBJModel();
};

#endif