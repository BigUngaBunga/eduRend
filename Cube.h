#pragma once
#ifndef CUBE_H
#define CUBE_H

#include "Model.h"
#include <array>

class Cube : public Model {
	
private:
	unsigned nbr_indices = 0;

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	void AddASquare();
	void AddVertex(Vertex& vertex, const vec2f& textureCoordinate);
	void CreateSquare(const vec3f& normal, const vec3f& vectorA, const vec3f& vectorB, const vec3f& vectorC, const vec3f& vectorD);
	void InitializeRender();
public:
	Cube(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context, const float length);
	virtual void Render() const;
};

#endif // CUBE_H
