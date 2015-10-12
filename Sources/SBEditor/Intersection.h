/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once

#include "Vec2.h"

namespace sb {
	//Forward Declarations
	template<typename T>
	struct Option;

	struct Line;
	struct Ray;
	struct Vec2;
	struct Rect;
	struct LineSegment;
	struct Circle;
	struct Polygon;
	//Intersection Info
	struct IntersectionInfo {
	public:
		bool empty;
		Vec2 normal;
		float penetration;
		IntersectionInfo() {
			empty = true;
			penetration = 0;
		}
		IntersectionInfo(const Vec2& normal, float penetration) {
			this->empty = false;
			this->normal = normal;
			this->penetration = penetration;
		}
	};
	//Intersection Classes
	class Intersection {
	public:
		static Option<Vec2> get(const Ray& a, const Ray& b);
		static Option<Vec2> get(const Ray& a, const LineSegment& b);
		static Option<Vec2> get(const Ray& a, const Line& b);
		static Option<Vec2> get(const Ray& a, const Rect& b);
		static Option<Vec2> get(const Ray& a, const Circle& b);
		static Option<Vec2> get(const Ray& a, const Polygon& b);
		static Option<Vec2> get(const Line& a, const Line& b);
		static Option<Vec2> get(const LineSegment& a, const LineSegment& b);
		static IntersectionInfo get(const Rect& a, const Rect& b);
		static IntersectionInfo get(const Circle& a, const Circle& b);
		static IntersectionInfo get(const Rect& a, const Circle& b);
		static IntersectionInfo get(const Polygon& a, const Polygon& b);
		static IntersectionInfo get(const Polygon& a, const Rect& b);
		static IntersectionInfo get(const Polygon& a, const Circle& b);
		static bool test(const Ray& a, const Vec2& b);
		static bool test(const Line& a, const Vec2& b);
		static bool test(const LineSegment& a, const Vec2& b);
		static bool test(const Rect& a, const Vec2& b);
		static bool test(const Rect& a, const Rect& b);
		static bool test(const Circle& a, const Circle& b);
		static bool test(const Circle& a, const Vec2& b);
		static bool test(const Polygon& a, const Vec2& b);
	};
}
