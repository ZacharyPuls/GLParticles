#pragma once

#include "opengl.h"

class Transform
{
public:
	Transform() = default;


	Transform(const glm::vec3& scale, const glm::vec3& rotationOrigin, const glm::quat& rotation,
		const glm::vec3& translation)
		: scale_(scale),
		  rotationOrigin_(rotationOrigin),
		  rotation_(rotation),
		  translation_(translation)
	{
	}

	glm::vec3 Scale() const
	{
		return scale_;
	}

	void SetScale(const glm::vec3& scale)
	{
		scale_ = scale;
	}

	glm::vec3 RotationOrigin() const
	{
		return rotationOrigin_;
	}

	void SetRotationOrigin(const glm::vec3& rotationOrigin)
	{
		rotationOrigin_ = rotationOrigin;
	}

	glm::quat Rotation() const
	{
		return rotation_;
	}

	void SetRotation(const glm::quat& rotation)
	{
		rotation_ = rotation;
	}

	glm::vec3 Translation() const
	{
		return translation_;
	}

	void SetTranslation(const glm::vec3& translation)
	{
		translation_ = translation;
	}

private:
	glm::vec3 scale_;
	glm::vec3 rotationOrigin_;
	glm::quat rotation_;
	glm::vec3 translation_;
};
