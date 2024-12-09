#pragma once

#include <glm/glm.hpp>

using mat4 = glm::dmat4;
using vec4 = glm::dvec4;
using vec3 = glm::dvec3;
using vec2 = glm::dvec2;

using color1 = glm::u8vec1;
using color2 = glm::u8vec2;
using color3 = glm::u8vec3;
using color4 = glm::u8vec4;

namespace Colors {
	const color4 White(255, 255, 255, 255);
	const color4 Red( 255, 0, 0, 255);
	const color4 Green( 0, 255, 0, 255);
	const color4 Blue( 0, 0, 255, 255);
};

struct Point {
	vec3 p;
	Point(const vec3& v3) : p(v3) {}
};

inline Point operator*(const mat4& m, const Point& p) {
	const auto v3 =  m * vec4(p.p, 1);
	return Point(vec3(v3) / v3.w);
}

struct Vector {
	vec3 v;
	Vector(const vec3& v3) : v(v3) {}
};

inline Vector operator*(const mat4& m, const Vector& v) { return Vector(vec3(m * vec4(v.v, 0))); }

struct Plane {
	Vector normal;
	Point point;
	Plane(const Vector& normal, const Point& point) : normal(normal), point(point) {}
	// Función para calcular la distancia de un punto al plano
	double distance(const Point& p) const {
		return glm::dot(normal.v, p.p - point.p);
	}
};

inline Plane operator*(const mat4& m, const Plane& plane) { return Plane(m * plane.normal, m * plane.point); }