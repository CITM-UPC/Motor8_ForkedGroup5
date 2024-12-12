#include "FrustrumManager.h"
#include <glm/gtc/type_ptr.hpp>



bool FrustrumManager::isInsideFrustum(const BoundingBox& bbox, const std::list<Plane>& frustumPlanes) {
    for (const auto& plane : frustumPlanes) {
        if (plane.distance(bbox.v000()) < 0 &&
            plane.distance(bbox.v001()) < 0 &&
            plane.distance(bbox.v010()) < 0 &&
            plane.distance(bbox.v011()) < 0 &&
            plane.distance(bbox.v100()) < 0 &&
            plane.distance(bbox.v101()) < 0 &&
            plane.distance(bbox.v110()) < 0 &&
            plane.distance(bbox.v111()) < 0) {
            return false;
        }
    }
    return true;
}


void FrustrumManager::drawFrustum(const GameObject& camera) {
    //auto planes = camera.frustumPlanes();
    auto planes = camera.GetComponent<CameraComponent>()->camera().frustumPlanes();
    std::vector<glm::vec3> frustumCorners = {
        glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1),
        glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1),
        glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1),
        glm::vec3(1, 1, 1), glm::vec3(-1, 1, 1)
    };

    glm::mat4 invProjView = glm::inverse(camera.GetComponent<CameraComponent>()->camera().projection() * camera.GetComponent<CameraComponent>()->camera().view());
    for (auto& corner : frustumCorners) {
        glm::vec4 transformedCorner = invProjView * glm::vec4(corner, 1.0f);
        corner = glm::vec3(transformedCorner) / transformedCorner.w;
    }

    glBegin(GL_LINES);
    for (int i = 0; i < 4; ++i) {
        glVertex3fv(glm::value_ptr(frustumCorners[i]));
        glVertex3fv(glm::value_ptr(frustumCorners[(i + 1) % 4]));
        glVertex3fv(glm::value_ptr(frustumCorners[i + 4]));
        glVertex3fv(glm::value_ptr(frustumCorners[(i + 1) % 4 + 4]));
        glVertex3fv(glm::value_ptr(frustumCorners[i]));
        glVertex3fv(glm::value_ptr(frustumCorners[i + 4]));
    }
    glEnd();
}
