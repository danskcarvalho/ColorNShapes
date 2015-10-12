/**
Copyright (C) 2014 Danilo Carvalho - All Rights Reserved

You can't use, distribute or modify this code without my permission.
*/

#pragma once
class shape_implementation;
class SpatialTree_implementation;

namespace sb {
	enum class ShapeType {
		circle,
		Rect,
		Polygon
	};

	class SpatialTree;
	class Shape;
	struct Circle;
	struct Rect;
	struct Polygon;
	struct Matrix3x3;

	class Shape {
	public:
		//Friend class
		friend class ::SpatialTree_implementation;
		//Constructors
		static Shape* fromCircle(const Circle& c);
		static Shape* fromRect(const Rect& r);
		static Shape* fromPolygon(const Polygon& p);
		//Destructors
		virtual ~Shape();
		//Info
		ShapeType type() const;
		Rect bounds() const;
		//Accessors
		const Circle& circle() const;
		const Rect& Rect() const;
		const Polygon& Polygon() const;
		//Masks
		const size_t typeMask() const;
		void setTypeMask(size_t value);
		const size_t collisionMask() const;
		void setCollisionMask(size_t value);
		//Static/Dynamic
		bool isDynamic() const;
		//Parent
		SpatialTree* parent() const;
	private:
		//Constructors
		Shape();
		//Apply Transforms
		void applyTransform(const Matrix3x3& m);
		//Set
		void setDynamic(bool value);
		void setParent(SpatialTree* value);
		//Implementation
		shape_implementation* m_impl;
	};
}

