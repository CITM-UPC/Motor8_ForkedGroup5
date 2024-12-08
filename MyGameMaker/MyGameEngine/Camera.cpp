#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/rotate_vector.hpp>

glm::dmat4 Camera::projection() const {
	return glm::perspective(fov, aspect, zNear, zFar);
}

glm::dmat4 Camera::view() const {
	return glm::lookAt(_transform.pos(), _transform.pos() + _transform.fwd(), _transform.up());
}

std::list<Plane> Camera::frustumPlanes() const {
	const auto h_fov = fov;
	const auto v_fov = fov / aspect;
	return {
		//near
		Plane(_transform.fwd(), _transform.pos() + _transform.fwd() * zNear),
		//far
		Plane(-_transform.fwd(), _transform.pos() + _transform.fwd() * zFar),
		//left
		Plane(glm::rotate(-_transform.left(), h_fov, _transform.up()), _transform.pos()),
		//right
		Plane(glm::rotate(_transform.left(), -h_fov, _transform.up()), _transform.pos()),
		//top
		Plane(glm::rotate(-_transform.up(), -v_fov, _transform.left()), _transform.pos()),
		//bottom
		Plane(glm::rotate(_transform.up(), v_fov, _transform.left()), _transform.pos())
	};
}