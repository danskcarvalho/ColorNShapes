/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once

#include "Vec2.h"

#define SbMaxCollisions 12
#define SbCellSize 4.0f

class SpatialTree_implementation;

namespace sb {
	struct Matrix3x3;
	struct Ray;
	struct Rect;
	class SpatialTree;
	class Shape;

	struct ExIntersectionInfo {
		Shape* bodyA;
		Shape* bodyB;
		Vec2 normal;
		float penetration;
		bool empty;
		ExIntersectionInfo() {
			this->empty = true;
			this->bodyA = nullptr;
			this->bodyB = nullptr;
			this->penetration = 0;
		}
		ExIntersectionInfo(Shape* bodyA, Shape* bodyB, const Vec2& normal, float penetration) {
			this->empty = false;
			this->bodyA = bodyA;
			this->bodyB = bodyB;
			this->normal = normal;
			this->penetration = penetration;
		}
	};

	struct IntersectionQueryResult {
		size_t count;
		ExIntersectionInfo intersections[SbMaxCollisions];
		IntersectionQueryResult() {
			count = 0;
		}
		void pushInfo(const ExIntersectionInfo& ii) {
			assert(count != SbMaxCollisions);
			intersections[count] = ii;
			count++;
		}
		bool isFull() const {
			return count == SbMaxCollisions;
		}
	};

	struct RayCastResult {
	public:
		bool empty;
		Shape* intersected;
		Vec2 point;
		float parameter;
		RayCastResult() {
			empty = true;
			intersected = nullptr;
			parameter = 0;
		}
		RayCastResult(Shape* s, const Vec2& v, float p) {
			empty = false;
			intersected = s;
			point = v;
			parameter = p;
		}
	};

	struct RangeQueryResult {
	public:
		size_t count;
		Shape* shapes[SbMaxCollisions];
		RangeQueryResult() {
			for (size_t i = 0; i < SbMaxCollisions; i++)
				shapes[i] = nullptr;
			count = 0;
		}
		void pushShape(Shape* s) {
			assert(count != SbMaxCollisions);
			shapes[count] = s;
			count++;
		}
		bool isFull() const {
			return count == SbMaxCollisions;
		}
		void clear() {
			for (size_t i = 0; i < SbMaxCollisions; i++)
				shapes[i] = nullptr;
			count = 0;
		}
	};

	enum StaticDynamicMask {
		sdmDynamic = 1,
		sdmStatic = 2,
		sdmAll = 3
	};

	class SpatialTree {
	public:
		//Constructors
		static SpatialTree* create();
		//Destructor
		virtual ~SpatialTree();
		//Modifying
		void addStaticNode(Shape* s) const;
		void addDynamicNode(Shape* s) const;
		void removeNode(Shape* s) const;
		void transform(Shape* s, const Matrix3x3& m);
		//Queries
		RayCastResult rayCast(const Ray& r, size_t mask = std::numeric_limits<size_t>::max(), StaticDynamicMask sdFilter = sdmAll) const;
		void rangeQuery(RangeQueryResult* result, const Rect& r, size_t mask = std::numeric_limits<size_t>::max(), StaticDynamicMask sdFilter = sdmAll) const;
		void pickQuery(RangeQueryResult* result, const Vec2& pt, size_t mask = std::numeric_limits<size_t>::max(), StaticDynamicMask sdFilter = sdmAll) const;
		void intersectionQuery(IntersectionQueryResult* result, Shape* bodyA, StaticDynamicMask sdFilter = sdmAll) const;
	private:
		SpatialTree();
		SpatialTree_implementation* m_impl;
	};
}

