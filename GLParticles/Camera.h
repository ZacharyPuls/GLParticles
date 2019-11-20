#pragma once

class Camera
{
public:
	Camera() = default;

	Camera(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up, const float fovY, const float aspect,
	       const float near, const float far)
		: eye_(eye),
		  target_(target),
		  up_(up),
		  fovY_(fovY),
		  aspect_(aspect),
		  zNear_(near),
		  zFar_(far)
	{
	}

	glm::mat4 View() const
	{
		return glm::lookAt(eye_, target_, up_);
	}

	glm::mat4 Projection() const
	{
		return glm::perspective(fovY_, aspect_, zNear_, zFar_);
	}


	glm::vec3 Eye() const
	{
		return eye_;
	}

	void SetEye(const glm::vec3& eye)
	{
		eye_ = eye;
	}

	void SetTarget(const glm::vec3& target)
	{
		target_ = target;
	}

	void SetUp(const glm::vec3& xes)
	{
		up_ = xes;
	}

	void SetFovY(float fovY)
	{
		fovY_ = fovY;
	}

	void SetAspect(float aspect)
	{
		aspect_ = aspect;
	}

	void SetZNear(float zNear)
	{
		zNear_ = zNear;
	}

	void SetZFar(float zFar)
	{
		zFar_ = zFar;
	}

private:
    glm::vec3 eye_;
    glm::vec3 target_;
    glm::vec3 up_;

    float fovY_;
    float aspect_;
    float zNear_;
    float zFar_;
};
