#pragma once

#include "types.h"

class Transform {
public:
    mutable bool _dirty{ false };
private:
    union {
        mat4 _mat = mat4(1.0);
        struct {
            vec3 _left; mat4::value_type _left_w;
            vec3 _up; mat4::value_type _up_w;
            vec3 _fwd; mat4::value_type _fwd_w;
            vec3 _pos; mat4::value_type _pos_w;
        };
    };

    vec3 m_scale;

    // Variables para los ángulos de rotación
    double yaw = 0.0;
    double pitch = 0.0;
    double roll = 0.0;

public:
    const auto& mat() const { return _mat; }
    const auto& left() const { return _left; }
    const auto& up() const { return _up; }
    const auto& fwd() const { return _fwd; }
    const auto& pos() const { return _pos; }
    auto& pos() { return _pos; }

    const auto* data() const { return &_mat[0][0]; }

    Transform() = default;
    Transform(const mat4& mat) : _mat(mat) {}

    void setScale(const vec3& scale);
    vec3& scale();

    void translate(const vec3& v);
    void rotateYaw(double radians);
    void rotatePitch(double radians);
    void rotateRoll(double radians);
    void rotate(double rads, const vec3& v);
    void updateRotationMatrix();
	void updateScaleMatrix();
    void lookAt(const vec3& target);
    void alignCamera(const vec3& worldUp = vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 extractEulerAngles(const glm::mat4& mat);
    glm::vec3 extractScale(const glm::mat4& mat);

    void setPos(const vec3& pos); // Declaración de setPos
    void setRotation(const vec3& eulerAngles);// Declaración de setRotation

    Transform operator*(const mat4& other) { return Transform(_mat * other); }
    Transform operator*(const Transform& other) { return Transform(_mat * other._mat); }
};

