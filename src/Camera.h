
//
//  Camera.h
//
//	Basic camera class
//

#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "vec\vec.h"
#include "vec\mat.h"

using namespace linalg;

class Camera
{
public:
	// Aperture attributes
	float vfov, aspect;	
	
	// Clip planes in view space coordinates
	// Evrything outside of [zNear, zFar] is clipped away on the GPU side
	// zNear should be > 0
	// zFar should depend on the size of the scene
	// This range should be kept as tight as possibly to improve
	// numerical precision in the z-buffer
	float zNear, zFar;	
						
	vec3f position;
	vec3f rotation;
	vec3f scale;
	float rotationSpeed;

	Camera(
		float vfov,
		float aspect,
		float zNear,
		float zFar):
		vfov(vfov), aspect(aspect), zNear(zNear), zFar(zFar)
	{
		position = vec3f_zero;
		rotation = vec3f_zero;
		scale = { 1, 1, 1 };
		rotationSpeed = 0.069f;
	}

	// Move to an absolute position
	//
	void moveTo(const vec3f& p)
	{
		position = p;
	}

	// Move relatively
	//
	void move(const vec3f& v)
	{
		position += GetRotation().get_3x3() * v;
	}

	void rotate(const vec3f& newRotation) {
		this->rotation += newRotation * rotationSpeed;
		rotation.x = clamp(rotation.x, -PI / 2, PI / 2);
	}

	void SetZeroRoll() {
		rotation.z = 0;
	}

	void Scale(const vec3f& newScale) { scale += newScale; }

	void ResetScale() { scale = {1, 1, 1}; }

	mat4f GetRotation() const {
		return mat4f::rotation(rotation.z, rotation.y, rotation.x);
	}

	// Return World-to-View matrix for this camera
	//
	mat4f get_WorldToViewMatrix() const
	{
		// Assuming a camera's position and rotation is defined by matrices T(p) and R,
		// the View-to-World transform is T(p)*R (for a first-person style camera).
		//
		// World-to-View then is the inverse of T(p)*R;
		//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
		// Since now there is no rotation, this matrix is simply T(-p)
		
		mat4f rotationMatrix = GetRotation();
		rotationMatrix.transpose();

		return rotationMatrix * mat4f::translation(-position) * mat4f::scaling(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
	}

	mat4f get_ViewToWorldMatrix() const {return mat4f::translation(position) * GetRotation() * mat4f::scaling(scale);}

	// Matrix transforming from View space to Clip space
	// In a performance sensitive situation this matrix should be precomputed
	// if possible
	//
	mat4f get_ProjectionMatrix() const
	{
		return mat4f::projection(vfov, aspect, zNear, zFar);
	}

	vec4f GetWorldPosition() const {
		mat4f transform = get_ViewToWorldMatrix();
		return vec4f(transform.m14, transform.m24, transform.m34, transform.m44);
	}

	vec4f GetPosition() const {
		mat4f transform = get_WorldToViewMatrix();
		return vec4f(transform.m14, transform.m24, transform.m34, transform.m44);
	}
};

#endif