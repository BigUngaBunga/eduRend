
//
//  Drawcall.h
//
//  Carl Johan Gribel 2016-2021, cjgribel@gmail.com
//

#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "stdafx.h"
#include "vec/vec.h"

#include "Texture.h"

using namespace linalg;

struct Vertex
{
	vec3f Pos;
	vec3f Normal, Tangent, Binormal;
	vec2f TexCoord;
};

//
// Phong-esque material
//
struct Material
{
	// Color components (ambient, diffuse & specular),
	// with default values
    vec3f Ka = {0.1f,0.1f,0.1f}, Kd = {0.2f, 0.1f, 0.4f}, Ks = {1,1,1};
    
	float shininess = 10.0f;
	float hasNormalMap = 1.0f;
	float isSkybox = 0.0f;

	std::string name;

	std::string defaultDiffuseTexture = "assets/textures/floor_diffuse.JPG";
	std::string defaultNormalMap = "assets/textures/floor_bump.JPG";
	// File paths to textures
	std::string Kd_texture_filename = defaultDiffuseTexture;
	std::string normal_texture_filename = defaultNormalMap;
	// + more texture types (extend OBJLoader::LoadMaterials if needed)

	// Device textures
	Texture diffuse_texture;
	Texture normal_map;
	// + other texture types

	bool HasNormalMap() const {
		bool hasDefaultTexture = Kd_texture_filename == defaultDiffuseTexture;
		bool hasDefaultNormalMap = defaultNormalMap == normal_texture_filename;
		return (hasDefaultTexture && hasDefaultNormalMap) || (!hasDefaultTexture && !hasDefaultNormalMap);
	}
};

static Material DefaultMaterial = Material();

typedef std::unordered_map<std::string, Material> MaterialHash;

struct Triangle 
{ 
	unsigned vi[3]; 
};

struct Quad
{ 
	unsigned vi[4];
};

struct Drawcall
{
    std::string group_name;
    int mtl_index = -1;
    std::vector<Triangle> tris;
    std::vector<Quad> quads;
    
	// Make sortable w.r.t. material
    bool operator < (const Drawcall& dc) const
    {
        return mtl_index < dc.mtl_index;
    }
};

#endif
