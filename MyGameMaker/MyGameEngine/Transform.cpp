#include "Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/compatibility.hpp>


void Transform::translate(const vec3& v) {
    _dirty = true;
    _mat = glm::translate(_mat, v);
}

void Transform::rotateYaw(double radians) {
    yaw += radians; 
    updateRotationMatrix(); 
}

void Transform::rotatePitch(double radians) {
    pitch += radians; 
    updateRotationMatrix(); 
}

void Transform::setScale(const vec3& scale) {
    m_scale = scale;
    updateScaleMatrix();
}

void Transform::updateScaleMatrix() {
    _mat = glm::scale(_mat, m_scale);
}

vec3& Transform::scale() {
    return m_scale;
}

void Transform::rotateRoll(double radians) {
    roll += radians;
    updateRotationMatrix(); 
}

void Transform::rotate(double rads, const vec3& v) {
    _dirty = true;
    _mat = glm::rotate(_mat, rads, v);
}

void Transform::updateRotationMatrix() {
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);
    float cosPitch = cos(pitch);
    float sinPitch = sin(pitch);
    float cosRoll = cos(roll);
    float sinRoll = sin(roll);

    mat4 yawMatrix = {
        cosYaw, 0, sinYaw, 0,
        0, 1, 0, 0,
        -sinYaw, 0, cosYaw, 0,
        0, 0, 0, 1
    };

    mat4 pitchMatrix = {
        1, 0, 0, 0,
        0, cosPitch, -sinPitch, 0,
        0, sinPitch, cosPitch, 0,
        0, 0, 0, 1
    };

    mat4 rollMatrix = {
        cosRoll, -sinRoll, 0, 0,
        sinRoll, cosRoll, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    // Reiniciar la matriz de transformación antes de aplicar las rotaciones
    _mat = mat4(1.0f);

    // Aplicar las rotaciones en el orden correcto
    _mat = rollMatrix * pitchMatrix * yawMatrix * _mat;

    // Aplicar la escala
    updateScaleMatrix();
}

void Transform::alignCamera(const vec3& worldUp) {

    vec3 fwd = glm::normalize(_fwd);
    vec3 right = glm::normalize(glm::cross(worldUp, fwd));
    vec3 up = glm::cross(fwd, right);


    _left = right;
    _up = up;
    _fwd = fwd;
    _pos = _pos;
    _mat = mat4(vec4(_left, 0.0f), vec4(_up, 0.0f), vec4(_fwd, 0.0f), vec4(_pos, 1.0f));
}

void Transform::lookAt(const vec3& target) {
    _fwd = glm::normalize(  _pos- target);
    _left = glm::normalize(glm::cross(vec3(0, 1, 0), _fwd));
    _up = glm::cross(_fwd, _left);

    _mat[0] = vec4(_left, 0.0);
    _mat[1] = vec4(_up, 0.0);
    _mat[2] = vec4(-_fwd, 0.0);
    _mat[3] = vec4(_pos, 1.0);
}

glm::vec3 Transform::extractEulerAngles(const glm::mat4& mat) {
    glm::vec3 forward(mat[2][0], mat[2][1], mat[2][2]);
    glm::vec3 up(mat[1][0], mat[1][1], mat[1][2]);
    glm::vec3 left(mat[0][0], mat[0][1], mat[0][2]);

    float yaw = atan2(forward.x, forward.z);

    float pitch = atan2(-forward.y, sqrt(forward.x * forward.x + forward.z * forward.z));

    float roll = atan2(left.y, up.y);

    return glm::vec3(glm::degrees(pitch), glm::degrees(yaw), glm::degrees(roll));
}

glm::vec3 Transform::extractScale(const glm::mat4& mat) {
    glm::vec3 left(mat[0][0], mat[0][1], mat[0][2]);
    glm::vec3 up(mat[1][0], mat[1][1], mat[1][2]);
    glm::vec3 forward(mat[2][0], mat[2][1], mat[2][2]);

    float scaleX = glm::length(left);
    float scaleY = glm::length(up);
    float scaleZ = glm::length(forward);

    return glm::vec3(scaleX, scaleY, scaleZ);
}

void Transform::setPos(const vec3& pos) {
    _pos = pos;
    _mat[3] = vec4(_pos, 1.0f);
}

void Transform::setRotation(const vec3& eulerAngles) {
    if (eulerAngles.x != std::numeric_limits<float>::quiet_NaN()) {
        pitch = eulerAngles.x;
    }
    if (eulerAngles.y != std::numeric_limits<float>::quiet_NaN()) {
        yaw = eulerAngles.y;
    }
    if (eulerAngles.z != std::numeric_limits<float>::quiet_NaN()) {
        roll = eulerAngles.z;
    }
    updateRotationMatrix();
}


// --- Métodos de parenting ---
void Transform::setParent(Transform* parent) {
    if (_parent) {
        _parent->removeChild(this);
    }

    _parent = parent;

    if (_parent) {
        _parent->addChild(this);
    }

    update(); // Recalcular transformaciones globales
}

Transform* Transform::getParent() const {
    return _parent;
}

void Transform::addChild(Transform* child) {
    if (child && std::find(_children.begin(), _children.end(), child) == _children.end()) {
        _children.push_back(child);
        child->_parent = this; // Evita bucle redundante
        child->update();       // Actualiza transformaciones
    }
}


void Transform::removeChild(Transform* child) {
    _children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
    if (child) {
        child->_parent = nullptr;
        child->update(); // Recalcula sin padre
    }
}

const std::vector<Transform*>& Transform::getChildren() const {
    return _children;
}

// --- Transformaciones locales ---
void Transform::setLocalPos(const vec3& pos) {
    _localPos = pos;
    update();
}

void Transform::setLocalRotation(const vec3& eulerAngles) {
    _localYaw = glm::radians(eulerAngles.y);
    _localPitch = glm::radians(eulerAngles.x);
    _localRoll = glm::radians(eulerAngles.z);
    update();
}

void Transform::setLocalScale(const vec3& scale) {
    _localScale = scale;
    update();
}

const mat4& Transform::getLocalMatrix() const {
    return _localMat;
}

// --- Transformaciones globales ---
vec3 Transform::getGlobalPos() const {
    return vec3(_globalMat[3]);
}

mat4 Transform::getGlobalMatrix() const {
    return _globalMat;
}

void Transform::updateGlobalMatrix() {
    if (_parent) {
        _globalMat = _parent->getGlobalMatrix() * _localMat;
    }
    else {
        _globalMat = _localMat;
    }
}

void Transform::propagateGlobalChanges() {
    for (Transform* child : _children) {
        if (child) {
            child->updateGlobalMatrix();
            child->propagateGlobalChanges();
        }
    }
}
// --- Actualización general ---
void Transform::update() {
    // Actualiza la matriz local combinando posición, rotación y escala
    mat4 rotationMatrix = glm::yawPitchRoll(static_cast<float>(_localYaw), static_cast<float>(_localPitch), static_cast<float>(_localRoll));
    _localMat = glm::translate(mat4(1.0f), _localPos) * rotationMatrix * glm::scale(mat4(1.0f), _localScale);

    // Calcula la matriz global
    updateGlobalMatrix();

    // Propaga los cambios a los hijos
    propagateGlobalChanges();
}

