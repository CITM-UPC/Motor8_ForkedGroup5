#pragma once

#include "types.h"

#include <vector>

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

    Transform operator*(const mat4& other) { return _mat * other; }
    Transform operator*(const Transform& other) { return _mat * other._mat; }

private:
    Transform* _parent = nullptr;                      // Puntero al padre
    std::vector<Transform*> _children;                // Lista de hijos
    mat4 _localMat = mat4(1.0f);                      // Transformación local
    mat4 _globalMat = mat4(1.0f);                     // Transformación global

    vec3 _localPos = vec3(0.0f);                      // Posición local
    vec3 _localScale = vec3(1.0f);                    // Escala local
    double _localYaw = 0.0, _localPitch = 0.0, _localRoll = 0.0; // Rotación local

    void updateGlobalMatrix();                        // Calcula la matriz global
    void propagateGlobalChanges();                    // Propaga cambios a los hijos
public:    

    // Parenting
    void setParent(Transform* parent);                // Asigna un padre
    Transform* getParent() const;                     // Obtiene el padre
    void addChild(Transform* child);                  // Añade un hijo
    void removeChild(Transform* child);               // Elimina un hijo
    const std::vector<Transform*>& getChildren() const; // Obtiene los hijos

    // Métodos para manipular la transformación local
    void setLocalPos(const vec3& pos);
    void setLocalRotation(const vec3& eulerAngles);
    void setLocalScale(const vec3& scale);
    const mat4& getLocalMatrix() const;

    // Métodos para obtener transformaciones globales
    vec3 getGlobalPos() const;
    mat4 getGlobalMatrix() const;

    void update();
};

