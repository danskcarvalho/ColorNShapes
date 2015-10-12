/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once

#include "Utils.h"
#include "Vec2.h"
#include "Polygon.h"
#include "Circle.h"
#include "Line.h"
#include "LineSegment.h"
#include "Ray.h"
#include "Rect.h"

namespace sb {
	struct Matrix3x3 {
	public:
		//Elements
		union {
			struct {
				float e00;
				float e10;
				float e20;
				float e01;
				float e11;
				float e21;
				float e02;
				float e12;
				float e22;
			};
			float elements[9];
		};
		//Constants
		static const Matrix3x3 identity;
		//Constructors
		inline Matrix3x3() {
			elements[0] = elements[1] = elements[2] = 0;
			elements[3] = elements[4] = elements[5] = 0;
			elements[6] = elements[7] = elements[8] = 0;
		}
		inline Matrix3x3(float e00, float e01, float e02,
						 float e10, float e11, float e12,
						 float e20, float e21, float e22) {
			this->e00 = e00;
			this->e01 = e01;
			this->e02 = e02;
			this->e10 = e10;
			this->e11 = e11;
			this->e12 = e12;
			this->e20 = e20;
			this->e21 = e21;
			this->e22 = e22;
		}
		//Pseudo-constructors
		inline static Matrix3x3 fromTranslation(const Vec2& translation) {
			Matrix3x3 m;
			m.e00 = 1;
			m.e10 = 0;
			m.e20 = 0;
			m.e01 = 0;
			m.e11 = 1;
			m.e21 = 0;
			m.e02 = translation.x;
			m.e12 = translation.y;
			m.e22 = 1;
			return m;
		}
		inline static Matrix3x3 fromScale(float sx, float sy) {
			Matrix3x3 m;
			m.e00 = sx;
			m.e10 = 0;
			m.e20 = 0;
			m.e01 = 0;
			m.e11 = sy;
			m.e21 = 0;
			m.e02 = 0;
			m.e12 = 0;
			m.e22 = 1;
			return m;
		}
		inline static Matrix3x3 fromTranslation(float tx, float ty) {
			Matrix3x3 m;
			m.e00 = 1;
			m.e10 = 0;
			m.e20 = 0;
			m.e01 = 0;
			m.e11 = 1;
			m.e21 = 0;
			m.e02 = tx;
			m.e12 = ty;
			m.e22 = 1;
			return m;
		}
		inline static Matrix3x3 fromScale(const Vec2& scale) {
			Matrix3x3 m;
			m.e00 = scale.x;
			m.e10 = 0;
			m.e20 = 0;
			m.e01 = 0;
			m.e11 = scale.y;
			m.e21 = 0;
			m.e02 = 0;
			m.e12 = 0;
			m.e22 = 1;
			return m;
		}
		inline static Matrix3x3 fromRotation(float angle) {
			Matrix3x3 m;
			auto cos0 = cosf(angle);
			auto sin0 = sinf(angle);
			m.e00 = cos0;
			m.e10 = sin0;
			m.e20 = 0;
			m.e01 = -sin0;
			m.e11 = cos0;
			m.e21 = 0;
			m.e02 = 0;
			m.e12 = 0;
			m.e22 = 1;
			return m;
		}
		inline static Matrix3x3 fromSkew(const Vec2& skew) {
			Matrix3x3 m;
			m.e00 = 1;
			m.e10 = skew.y;
			m.e20 = 0;
			m.e01 = skew.x;
			m.e11 = 1;
			m.e21 = 0;
			m.e02 = 0;
			m.e12 = 0;
			m.e22 = 1;
			return m;
		}
		inline static Matrix3x3 fromSkew(float kx, float ky) {
			Matrix3x3 m;
			m.e00 = 1;
			m.e10 = ky;
			m.e20 = 0;
			m.e01 = kx;
			m.e11 = 1;
			m.e21 = 0;
			m.e02 = 0;
			m.e12 = 0;
			m.e22 = 1;
			return m;
		}
		//Access
		inline float element(size_t row, size_t column) const {
			assert(row >= 0 && row < 3);
			assert(column >= 0 && column < 3);
			return elements[column * 3 + row];
		}
		inline float& element(size_t row, size_t column) {
			assert(row >= 0 && row < 3);
			assert(column >= 0 && column < 3);
			return elements[column * 3 + row];
		}
		//Information
		inline Vec2 xVector() const {
			return Vec2(e00, e10);
		}
		inline void setXVector(const Vec2& value) {
			e00 = value.x;
			e10 = value.y;
		}
		inline Vec2 yVector() const {
			return Vec2(e01, e11);
		}
		inline void setYVector(const Vec2& value) {
			e01 = value.x;
			e11 = value.y;
		}
		inline Vec2 translation() const {
			return Vec2(e02, e12);
		}
		inline void setTranslation(const Vec2& value) {
			e02 = value.x;
			e12 = value.y;
		}
		inline void setTranslation(float tx, float ty) {
			e02 = tx;
			e12 = ty;
		}
		inline Vec2 scale() const {
			return Vec2(xVector().length(), yVector().length());
		}
		inline void setScale(const Vec2& value) {
			auto newX = xVector().normalized();
			auto newY = yVector().normalized();
			newX *= value.x;
			newY *= value.y;
			setXVector(newX);
			setYVector(newY);
		}
		inline void setScale(float sx, float sy) {
			auto newX = xVector().normalized();
			auto newY = yVector().normalized();
			newX *= sx;
			newY *= sy;
			setXVector(newX);
			setYVector(newY);
		}
		inline Vec2 rotation() const {
			return Vec2(Vec2::right.angleBetween(xVector(), RotationDirection::ccw),
						Vec2::right.angleBetween(yVector(), RotationDirection::ccw));
		}
		inline void setRotation(const Vec2& value) {
			const float xLen = xVector().length();
			const float yLen = yVector().length();
			setXVector(Vec2::right.rotatedBy(value.x) * xLen);
			setYVector(Vec2::right.rotatedBy(value.y) * yLen);
		}
		inline void setRotation(float rx, float ry) {
			const float xLen = xVector().length();
			const float yLen = yVector().length();
			setXVector(Vec2::right.rotatedBy(rx) * xLen);
			setYVector(Vec2::right.rotatedBy(ry) * yLen);
		}
		inline void setRotation(float rotation) {
			setRotation(rotation, rotation + SbPI / 2.0f);
		}
		inline void set(const Vec2& translation, const Vec2& scale, const Vec2& rotation) {
			auto xv = Vec2::right * scale.x;
			auto yv = Vec2::right * scale.y;
			xv.rotateBy(rotation.x);
			yv.rotateBy(rotation.y);
			setXVector(xv);
			setYVector(yv);
			setTranslation(translation);
		}
		//More Pseudo Constructors
		inline static Matrix3x3 fromRotation(const Vec2& angles) {
			Matrix3x3 m = Matrix3x3::identity;
			m.setRotation(angles);
			return m;
		}
		inline static Matrix3x3 fromRotation(float rx, float ry) {
			Matrix3x3 m = Matrix3x3::identity;
			m.setRotation(rx, ry);
			return m;
		}
		//Operations
		inline float determinant() const {
			return e00 * (e11 * e22 - e12 * e21) - e01 * (e22 * e10 - e12 * e20) + e02 * (e10 * e21 - e11 * e20);
		}
		inline Matrix3x3 inverse() const {
			const auto a = e00, b = e01, c = e02;
			const auto d = e10, e = e11, f = e12;
			const auto g = e20, h = e21, i = e22;
			const auto A = (e*i - f*h);
			const auto B = -(d*i - f*g);
			const auto C = (d*h - e*g);
			const auto D = -(b*i - c*h);
			const auto E = (a*i - c*g);
			const auto F = -(a*h - b*g);
			const auto G = (b*f - c*e);
			const auto H = -(a*f - c*d);
			const auto I = (a*e - b*d);
			auto result = Matrix3x3(A, D, G, B, E, H, C, F, I);
			auto determinant = this->determinant();
			assert(determinant != 0);
			determinant = 1.0f / determinant;
			for (size_t i = 0; i < 9; i++)
				result.elements[i] *= determinant;
			return result;
		}
		inline void invert() {
			Matrix3x3 inv = this->inverse();
			*this = inv;
		}
		inline Matrix3x3 transposed() const {
			auto result = Matrix3x3();
			result.e00 = e00;
			result.e01 = e10;
			result.e02 = e20;
			result.e10 = e01;
			result.e11 = e11;
			result.e12 = e21;
			result.e20 = e02;
			result.e21 = e12;
			result.e22 = e22;
			return result;
		}
		inline void transpose() {
			auto result = this->transposed();
			*this = result;
		}
		//Test
		inline bool isSkewed(bool almost = false) {
			auto angle = dot(xVector(), yVector());
			return almost ? aeq(angle, 0) : angle == 0;
		}
		inline bool isZero(bool almost = false) const {
			for (size_t i = 0; i < 9; i++) {
				if (almost) {
					if (!aeq(elements[i], 0))
						return false;
				}
				else {
					if (elements[i] != 0)
						return false;
				}
			}
			return true;
		}
		inline bool isIdentity(bool almost = false) const {
			for (size_t i = 0; i < 9; i++) {
				if (almost) {
					if (i == 0 || i == 4 || i == 8) {
						if (!aeq(elements[i], 1))
							return false;
					}
					else {
						if (!aeq(elements[i], 0))
							return false;
					}
				}
				else {
					if (i == 0 || i == 4 || i == 8) {
						if (elements[i] != 1)
							return false;
					}
					else {
						if (elements[i] != 0)
							return false;
					}
				}
			}
			return true;
		}
		inline bool isInvertible(bool almost = false) const {
			return almost ? !aeq(determinant(), 0) : determinant() != 0;
		}
		//Transformations
		inline void transformVector(Vec2* v) const {
			const auto vc = *v;
			v->x = e00 * vc.x + e01 * vc.y;
			v->y = e10 * vc.x + e11 * vc.y;
		}
		inline Vec2 transformedVector(const Vec2& v) const {
			Vec2 vc;
			vc.x = e00 * v.x + e01 * v.y;
			vc.y = e10 * v.x + e11 * v.y;
			return vc;
		}
		inline void transformPoint(Vec2* v) const {
			const auto vc = *v;
			v->x = e00 * vc.x + e01 * vc.y + e02;
			v->y = e10 * vc.x + e11 * vc.y + e12;
		}
		inline Vec2 transformedPoint(const Vec2& v) const {
			Vec2 vc;
			vc.x = e00 * v.x + e01 * v.y + e02;
			vc.y = e10 * v.x + e11 * v.y + e12;
			return vc;
		}
		inline Polygon transformedPolygon(const Polygon& p) const {
			return p.map([&](const Vec2& v) { return this->transformedPoint(v); }, true);
		}
		inline void transformCircle(Circle* c) const {
			auto p1 = c->center();
			auto p2 = c->center() + Vec2::right * c->radius();
			transformPoint(&p1);
			transformPoint(&p2);
			c->setCenter(p1);
			c->setRadius(distance(p1, p2));
		}
		inline Circle transformedCircle(const Circle& c) const {
			Circle o = c;
			transformCircle(&o);
			return o;
		}
		inline void transformLine(Line* l) const {
			auto p1 = l->pointA();
			auto p2 = l->pointB();
			transformPoint(&p1);
			transformPoint(&p2);
			l->setPoints(p1, p2);
		}
		inline Line transformedLine(const Line& l) const {
			auto o = l;
			transformLine(&o);
			return o;
		}
		inline void transformLineSegment(LineSegment* ls) const {
			auto p1 = ls->pointA();
			auto p2 = ls->pointB();
			transformPoint(&p1);
			transformPoint(&p2);
			ls->setPointA(p1);
			ls->setPointB(p2);
		}
		inline LineSegment transformedLineSegment(const LineSegment& ls) const {
			auto o = ls;
			transformLineSegment(&o);
			return o;
		}
		inline void transformRay(Ray* r) const {
			auto p = r->point();
			auto d = r->direction();
			transformPoint(&p);
			transformVector(&d);
			r->setPoint(p);
			r->setDirection(d);
		}
		inline Ray transformedRay(const Ray& r) const {
			auto o = r;
			transformRay(&o);
			return o;
		}
		inline void transformRect(Rect* r) const {
			Vec2 pts[4] = { r->topLeft(), r->topRight(), r->bottomLeft(), r->bottomRight() };
			for (size_t i = 0; i < 4; i++)
				transformPoint(&pts[i]);
			Rect nr = Rect(pts, 4);
			r->setCenter(nr.center());
			r->setSize(nr.size());
		}
		inline Rect transformedRect(const Rect& r) const {
			Vec2 pts[4] = { r.topLeft(), r.topRight(), r.bottomLeft(), r.bottomRight() };
			for (size_t i = 0; i < 4; i++)
				transformPoint(&pts[i]);
			return Rect(pts, 4);
		}
		//Description
		inline std::string toString() const {
			std::string d = "{";

			d += "{";
			d += std::to_string(e00); d += ", "; d += std::to_string(e01); d += ", "; d += std::to_string(e02); d += ", ";
			d += "}";

			d += "{";
			d += std::to_string(e10); d += ", "; d += std::to_string(e11); d += ", "; d += std::to_string(e12); d += ", ";
			d += "}";

			d += "{";
			d += std::to_string(e20); d += ", "; d += std::to_string(e21); d += ", "; d += std::to_string(e22); d += ", ";
			d += "}";

			d += "}";
			return d;
		}
		//Operations
		inline void operator*=(const Matrix3x3& m2) {
			Matrix3x3 m1 = *this;

			e00 = m1.e00 * m2.e00 + m1.e01 * m2.e10 + m1.e02 * m2.e20;
			e01 = m1.e00 * m2.e01 + m1.e01 * m2.e11 + m1.e02 * m2.e21;
			e02 = m1.e00 * m2.e02 + m1.e01 * m2.e12 + m1.e02 * m2.e22;

			e10 = m1.e10 * m2.e00 + m1.e11 * m2.e10 + m1.e12 * m2.e20;
			e11 = m1.e10 * m2.e01 + m1.e11 * m2.e11 + m1.e12 * m2.e21;
			e12 = m1.e10 * m2.e02 + m1.e11 * m2.e12 + m1.e12 * m2.e22;

			e20 = m1.e20 * m2.e00 + m1.e21 * m2.e10 + m1.e22 * m2.e20;
			e21 = m1.e20 * m2.e01 + m1.e21 * m2.e11 + m1.e22 * m2.e21;
			e22 = m1.e20 * m2.e02 + m1.e21 * m2.e12 + m1.e22 * m2.e22;
		}
		inline void operator*=(float s) {
			e00 *= s;
			e01 *= s;
			e02 *= s;

			e10 *= s;
			e11 *= s;
			e12 *= s;

			e20 *= s;
			e21 *= s;
			e22 *= s;
		}
	};
	inline Matrix3x3 operator *(const Matrix3x3& m1, const Matrix3x3& m2) {
		auto result = Matrix3x3();

		result.e00 = m1.e00 * m2.e00 + m1.e01 * m2.e10 + m1.e02 * m2.e20;
		result.e01 = m1.e00 * m2.e01 + m1.e01 * m2.e11 + m1.e02 * m2.e21;
		result.e02 = m1.e00 * m2.e02 + m1.e01 * m2.e12 + m1.e02 * m2.e22;

		result.e10 = m1.e10 * m2.e00 + m1.e11 * m2.e10 + m1.e12 * m2.e20;
		result.e11 = m1.e10 * m2.e01 + m1.e11 * m2.e11 + m1.e12 * m2.e21;
		result.e12 = m1.e10 * m2.e02 + m1.e11 * m2.e12 + m1.e12 * m2.e22;

		result.e20 = m1.e20 * m2.e00 + m1.e21 * m2.e10 + m1.e22 * m2.e20;
		result.e21 = m1.e20 * m2.e01 + m1.e21 * m2.e11 + m1.e22 * m2.e21;
		result.e22 = m1.e20 * m2.e02 + m1.e21 * m2.e12 + m1.e22 * m2.e22;

		return result;
	}
	inline Matrix3x3 operator *(const Matrix3x3& m1, float s) {
		auto result = Matrix3x3();

		result.e00 = m1.e00 * s;
		result.e01 = m1.e01 * s;
		result.e02 = m1.e02 * s;

		result.e10 = m1.e10 * s;
		result.e11 = m1.e11 * s;
		result.e12 = m1.e12 * s;

		result.e20 = m1.e20 * s;
		result.e21 = m1.e21 * s;
		result.e22 = m1.e22 * s;

		return result;
	}
	inline Matrix3x3 operator *(float s, const Matrix3x3& m1) {
		auto result = Matrix3x3();

		result.e00 = m1.e00 * s;
		result.e01 = m1.e01 * s;
		result.e02 = m1.e02 * s;

		result.e10 = m1.e10 * s;
		result.e11 = m1.e11 * s;
		result.e12 = m1.e12 * s;

		result.e20 = m1.e20 * s;
		result.e21 = m1.e21 * s;
		result.e22 = m1.e22 * s;

		return result;
	}
	inline bool operator ==(const Matrix3x3& m1, const Matrix3x3& m2) {
		for (size_t i = 0; i < 9; i++) {
			if (m1.elements[i] != m2.elements[i])
				return false;
		}
		return true;
	}
	inline bool operator !=(const Matrix3x3& m1, const Matrix3x3& m2) {
		for (size_t i = 0; i < 9; i++) {
			if (m1.elements[i] != m2.elements[i])
				return true;
		}
		return false;
	}
	inline bool operator >(const Matrix3x3& m1, const Matrix3x3& m2) {
		for (size_t i = 0; i < 9; i++) {
			if (m1.elements[i] == m2.elements[i])
				continue;
			if (m1.elements[i] > m2.elements[i])
				return true;
			else
				return false;
		}
		return false;
	}
	inline bool operator <(const Matrix3x3& m1, const Matrix3x3& m2) {
		for (size_t i = 0; i < 9; i++) {
			if (m1.elements[i] == m2.elements[i])
				continue;
			if (m1.elements[i] < m2.elements[i])
				return true;
			else
				return false;
		}
		return false;
	}
	inline bool operator >=(const Matrix3x3& m1, const Matrix3x3& m2) {
		for (size_t i = 0; i < 9; i++) {
			if (m1.elements[i] == m2.elements[i])
				continue;
			if (m1.elements[i] > m2.elements[i])
				return true;
			else
				return false;
		}
		return true;
	}
	inline bool operator <=(const Matrix3x3& m1, const Matrix3x3& m2) {
		for (size_t i = 0; i < 9; i++) {
			if (m1.elements[i] == m2.elements[i])
				continue;
			if (m1.elements[i] < m2.elements[i])
				return true;
			else
				return false;
		}
		return true;
	}
	inline bool aeq(const Matrix3x3& m1, const Matrix3x3& m2) {
		for (size_t i = 0; i < 9; i++) {
			if (!aeq(m1.elements[i], m2.elements[i]))
				return false;
		}
		return true;
	}
}
namespace std {
	template<>
	class hash < sb::Matrix3x3 > {
	public:
		size_t operator()(const sb::Matrix3x3& m) const {
			return sb::bitwiseHash((const unsigned char*)&m, sizeof(sb::Matrix3x3));
		}
	};
}

