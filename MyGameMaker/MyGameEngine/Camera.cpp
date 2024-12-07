#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

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
        // near
        Plane(_transform.fwd(), _transform.pos() + _transform.fwd() * zNear),
        // far
        Plane(-_transform.fwd(), _transform.pos() + _transform.fwd() * zFar),
        // left
        Plane(glm::normalize(glm::dvec3(glm::rotate(glm::dmat4(1.0), h_fov, _transform.up()) * glm::dvec4(-_transform.left(), 0.0))), _transform.pos()),
        // right
        Plane(glm::normalize(glm::dvec3(glm::rotate(glm::dmat4(1.0), -h_fov, _transform.up()) * glm::dvec4(_transform.left(), 0.0))), _transform.pos()),
        // top
        Plane(glm::normalize(glm::dvec3(glm::rotate(glm::dmat4(1.0), -v_fov, _transform.left()) * glm::dvec4(-_transform.up(), 0.0))), _transform.pos()),
        // bottom
        Plane(glm::normalize(glm::dvec3(glm::rotate(glm::dmat4(1.0), v_fov, _transform.left()) * glm::dvec4(_transform.up(), 0.0))), _transform.pos())
    };
}
